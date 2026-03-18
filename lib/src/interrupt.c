/* Copyright 2023-2026 Codasip s.r.o.    */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "baremetal/interrupt.h"

#include "baremetal/common.h"
#include "baremetal/csr.h"
#include "baremetal/interrupt_low.h"
#include "baremetal/mp.h"
#include "baremetal/platform.h"
#include "baremetal/priv.h"
#include "baremetal/verbose.h"

#ifdef TARGET_HAS_PLIC
    #include <baremetal/plic.h>
#elif defined(TARGET_HAS_PIC)
    #include "baremetal/pic.h"
#elif defined(TARGET_HAS_CLIC)
    #include "baremetal/clic.h"
#endif

#include <stddef.h>
#include <stdint.h>

static void bm_exception_print_details(xlen_t offset)
{
    bm_warn("Exception handler was called:");

    switch (offset)
    {
        case BM_EXCEPTION_IAM:
            bm_warn("Instruction address misaligned.");
            break;
        case BM_EXCEPTION_IAF:
            bm_warn("Instruction access fault.");
            break;
        case BM_EXCEPTION_II:
            bm_warn("Illegal instruction.");
            break;
        case BM_EXCEPTION_BREAK:
            bm_warn("Breakpoint exception.");
            break;
        case BM_EXCEPTION_LAM:
            bm_warn("Load address misaligned.");
            break;
        case BM_EXCEPTION_LAF:
            bm_warn("Load access fault.");
            break;
        case BM_EXCEPTION_SAM:
            bm_warn("Store address misaligned.");
            break;
        case BM_EXCEPTION_SAF:
            bm_warn("Store access fault.");
            break;
        case BM_EXCEPTION_ECALL_U:
            bm_warn("ECALL from user mode.");
            break;
        case BM_EXCEPTION_ECALL_S:
            bm_warn("ECALL from supervisor mode.");
            break;
        case BM_EXCEPTION_ECALL_M:
            bm_warn("ECALL from machine mode.");
            break;
        case BM_EXCEPTION_IPF:
            bm_warn("Instruction page fault");
            break;
        case BM_EXCEPTION_LPF:
            bm_warn("Load page fault.");
            break;
        case BM_EXCEPTION_SPF:
            bm_warn("Store page fault.");
            break;
#ifdef __CHERI_PURE_CAPABILITY__
        case BM_EXCEPTION_CHERI:
            bm_warn("Cheri exception.");
            break;
#endif
        default:
            bm_warn("Unknown exception " BM_FMT_XLEN, offset);
            break;
    }

    xlen_t csr_val = 0;

    BM_CSR_READ(BM_CSR_MCAUSE, csr_val);
    bm_warn("  CSR mcause: " BM_FMT_XLEN, csr_val);

#ifdef __CHERI_PURE_CAPABILITY__
    uint8_t *mepcc_val;
    BM_CSR_READ_CAP(mepcc, mepcc_val);
    bm_warn("  CSR mepcc:  %#p", mepcc_val);

#else
    BM_CSR_READ(BM_CSR_MEPC, csr_val);
    bm_warn("  CSR mepc:   " BM_FMT_XLEN, csr_val);
#endif

    BM_CSR_READ(BM_CSR_MTVAL, csr_val);
    bm_warn("  CSR mtval:  " BM_FMT_XLEN, csr_val);
}

/** \brief Table with handlers for individual exception sources */
static bm_intr_handler_t bm_exc_handler_table[BM_EXCEPTION_NUMBER] = {0};

/** \brief Table with handlers for individual interrupt sources */
static bm_intr_handler_t bm_interrupt_handler_table[BM_CORE_INTERRUPT_NUMBER] = {0};

#if (BM_EXT_INTERRUPT_NUMBER > 0)

/** \brief Table with handlers for individual external interrupt sources */
static bm_intr_handler_t bm_ext_irq_handler_table[BM_EXT_INTERRUPT_NUMBER] = {0};

/** \brief Internal handler for external interrupts */
void bm_ext_irq_handler(bm_register_file_t *stacked_regs)
{
    int pending = bm_ext_irq_claim();

    if (pending < 0)
    {
        if (pending == -1)
        {
            // no interrupt pending
            return;
        }
        bm_fatal("Interrupt claiming failed with error code %d", pending);
    }

    unsigned int irq = (unsigned int)pending;
    bm_fatal_check_index(irq, bm_ext_irq_handler_table);
    bm_intr_handler_t handler = bm_ext_irq_handler_table[irq];
    if (!handler)
    {
        bm_fatal("External interrupt %u with unset handler was triggered.", irq);
    }

    handler(stacked_regs);

    bm_ext_irq_complete(pending);
}

#endif // BM_EXT_INTERRUPT_NUMBER > 0

// clang-format off
#ifdef __CHERI_PURE_CAPABILITY__
#if RISCV_XLEN == 32
    #define BM_STORE      "sc"
    #define BM_LOAD       "lc"
    #define BM_STORE_F    "fsw"
    #define BM_LOAD_F     "flw"

#elif RISCV_XLEN == 64
    #define BM_STORE      "sc"
    #define BM_LOAD       "lc"
    #define BM_STORE_F    "fsd"
    #define BM_LOAD_F     "fld"

#else
    #error "unsupported RISCV_XLEN"
#endif /* RISCV_XLEN == */

/* Load/store register from/to stack with correct bm_register_file_t offsets */
#define LS_REG_OFFSET(instr, reg) __asm__ volatile(instr " c" #reg ", %0 (csp)\n"               \
                                                   :: "i"(offsetof(bm_register_file_t, reg)))

#define LS_REG_OFFSET_F(instr, reg) __asm__ volatile(instr " " #reg ", %0 (csp)\n"              \
                                                     :: "i"(offsetof(bm_register_file_t, reg)))

#else
#if RISCV_XLEN == 32
    #define BM_STORE      "sw"
    #define BM_LOAD       "lw"
    #define BM_STORE_F    "fsw"
    #define BM_LOAD_F     "flw"

#elif RISCV_XLEN == 64
    #define BM_STORE      "sd"
    #define BM_LOAD       "ld"
    #define BM_STORE_F    "fsd"
    #define BM_LOAD_F     "fld"

#else
    #error "unsupported RISCV_XLEN"
#endif /* RISCV_XLEN == */

/* Load/store register from/to stack with correct bm_register_file_t offsets */
#define LS_REG_OFFSET(instr, reg) __asm__ volatile(instr " " #reg ", %0 (sp)\n"                 \
                                                   :: "i"(offsetof(bm_register_file_t, reg)))

#define LS_REG_OFFSET_F LS_REG_OFFSET
#endif

/* The registers saved and restored should match the ones in the bm_register_file_t structure */
#define TARGET_SAVE_EMB_REGS        \
    LS_REG_OFFSET(BM_STORE, ra);    \
    LS_REG_OFFSET(BM_STORE, t0);    \
    LS_REG_OFFSET(BM_STORE, t1);    \
    LS_REG_OFFSET(BM_STORE, t2);    \
    LS_REG_OFFSET(BM_STORE, a0);    \
    LS_REG_OFFSET(BM_STORE, a1);    \
    LS_REG_OFFSET(BM_STORE, a2);    \
    LS_REG_OFFSET(BM_STORE, a3);    \
    LS_REG_OFFSET(BM_STORE, a4);    \
    LS_REG_OFFSET(BM_STORE, a5);

#define TARGET_LOAD_EMB_REGS        \
    LS_REG_OFFSET(BM_LOAD, ra);     \
    LS_REG_OFFSET(BM_LOAD, t0);     \
    LS_REG_OFFSET(BM_LOAD, t1);     \
    LS_REG_OFFSET(BM_LOAD, t2);     \
    LS_REG_OFFSET(BM_LOAD, a0);     \
    LS_REG_OFFSET(BM_LOAD, a1);     \
    LS_REG_OFFSET(BM_LOAD, a2);     \
    LS_REG_OFFSET(BM_LOAD, a3);     \
    LS_REG_OFFSET(BM_LOAD, a4);     \
    LS_REG_OFFSET(BM_LOAD, a5);

#define TARGET_SAVE_ALL_REGS        \
    TARGET_SAVE_EMB_REGS            \
    LS_REG_OFFSET(BM_STORE, a6);    \
    LS_REG_OFFSET(BM_STORE, a7);    \
    LS_REG_OFFSET(BM_STORE, t3);    \
    LS_REG_OFFSET(BM_STORE, t4);    \
    LS_REG_OFFSET(BM_STORE, t5);    \
    LS_REG_OFFSET(BM_STORE, t6);

#define TARGET_LOAD_ALL_REGS        \
    TARGET_LOAD_EMB_REGS            \
    LS_REG_OFFSET(BM_LOAD, a6);     \
    LS_REG_OFFSET(BM_LOAD, a7);     \
    LS_REG_OFFSET(BM_LOAD, t3);     \
    LS_REG_OFFSET(BM_LOAD, t4);     \
    LS_REG_OFFSET(BM_LOAD, t5);     \
    LS_REG_OFFSET(BM_LOAD, t6);

#define TARGET_SAVE_FLOAT_REGS            \
    LS_REG_OFFSET_F(BM_STORE_F, ft0);     \
    LS_REG_OFFSET_F(BM_STORE_F, ft1);     \
    LS_REG_OFFSET_F(BM_STORE_F, ft2);     \
    LS_REG_OFFSET_F(BM_STORE_F, ft3);     \
    LS_REG_OFFSET_F(BM_STORE_F, ft4);     \
    LS_REG_OFFSET_F(BM_STORE_F, ft5);     \
    LS_REG_OFFSET_F(BM_STORE_F, ft6);     \
    LS_REG_OFFSET_F(BM_STORE_F, ft7);     \
    LS_REG_OFFSET_F(BM_STORE_F, fa0);     \
    LS_REG_OFFSET_F(BM_STORE_F, fa1);     \
    LS_REG_OFFSET_F(BM_STORE_F, fa2);     \
    LS_REG_OFFSET_F(BM_STORE_F, fa3);     \
    LS_REG_OFFSET_F(BM_STORE_F, fa4);     \
    LS_REG_OFFSET_F(BM_STORE_F, fa5);     \
    LS_REG_OFFSET_F(BM_STORE_F, fa6);     \
    LS_REG_OFFSET_F(BM_STORE_F, fa7);     \
    LS_REG_OFFSET_F(BM_STORE_F, ft8);     \
    LS_REG_OFFSET_F(BM_STORE_F, ft9);     \
    LS_REG_OFFSET_F(BM_STORE_F, ft10);    \
    LS_REG_OFFSET_F(BM_STORE_F, ft11);

#define TARGET_LOAD_FLOAT_REGS            \
    LS_REG_OFFSET_F(BM_LOAD_F, ft0);      \
    LS_REG_OFFSET_F(BM_LOAD_F, ft1);      \
    LS_REG_OFFSET_F(BM_LOAD_F, ft2);      \
    LS_REG_OFFSET_F(BM_LOAD_F, ft3);      \
    LS_REG_OFFSET_F(BM_LOAD_F, ft4);      \
    LS_REG_OFFSET_F(BM_LOAD_F, ft5);      \
    LS_REG_OFFSET_F(BM_LOAD_F, ft6);      \
    LS_REG_OFFSET_F(BM_LOAD_F, ft7);      \
    LS_REG_OFFSET_F(BM_LOAD_F, fa0);      \
    LS_REG_OFFSET_F(BM_LOAD_F, fa1);      \
    LS_REG_OFFSET_F(BM_LOAD_F, fa2);      \
    LS_REG_OFFSET_F(BM_LOAD_F, fa3);      \
    LS_REG_OFFSET_F(BM_LOAD_F, fa4);      \
    LS_REG_OFFSET_F(BM_LOAD_F, fa5);      \
    LS_REG_OFFSET_F(BM_LOAD_F, fa6);      \
    LS_REG_OFFSET_F(BM_LOAD_F, fa7);      \
    LS_REG_OFFSET_F(BM_LOAD_F, ft8);      \
    LS_REG_OFFSET_F(BM_LOAD_F, ft9);      \
    LS_REG_OFFSET_F(BM_LOAD_F, ft10);     \
    LS_REG_OFFSET_F(BM_LOAD_F, ft11);
// clang-format on

/**
 * \brief Helper macros to save/restore all other registers on the stack
 */
#ifdef __riscv_32e
    #define TARGET_SAVE_REGS TARGET_SAVE_EMB_REGS
    #define TARGET_LOAD_REGS TARGET_LOAD_EMB_REGS
#elif defined(__riscv_flen)
    #define TARGET_SAVE_REGS \
        TARGET_SAVE_ALL_REGS \
        TARGET_SAVE_FLOAT_REGS
    #define TARGET_LOAD_REGS \
        TARGET_LOAD_ALL_REGS \
        TARGET_LOAD_FLOAT_REGS
#else
    #define TARGET_SAVE_REGS TARGET_SAVE_ALL_REGS
    #define TARGET_LOAD_REGS TARGET_LOAD_ALL_REGS
#endif

#define TARGET_STACK_SIZE (sizeof(bm_register_file_t))

/**
 * \brief Internal interrupt/exception handler routine
 *
 * - Update the variable current privilege mode (bm_current_mode), and save the previous value.
 * - Check whether interrupt or an exception triggered the handler.
 * - Check whether a user function was registered for the interrupt/exception cause:
 *   - if yes, call the user function,
     - otherwise, report an error.
 * - Restore bm_current_mode to the previous privilege mode value.
 *
 * \param new_mode Privilege mode the routine runs in
 */
void bm_managed_handler_inner(bm_priv_mode_t new_mode, bm_register_file_t *stacked_regs)
{
    // Update internal variable holding privilege mode
    bm_priv_mode_t prev_mode = bm_current_mode;
    bm_current_mode          = new_mode;

    // Get the value of [m,s,u]cause register
    xlen_t cause = bm_priv_csr_read(bm_get_priv_mode(), BM_PRIV_CSR_XCAUSE);

    // Get the offset in handler tables by clearing highest cause bit (interrupt bit)
#ifdef TARGET_HAS_CLIC
    xlen_t offset_mask = 0xfff;
#else
    xlen_t offset_mask = ~(xlen_t)0 >> 1;
#endif
    xlen_t offset = cause & offset_mask;

    if (cause >> (RISCV_XLEN - 1))
    {
        if (offset >= BM_ARRAY_ELEMENTS(bm_interrupt_handler_table))
        {
            bm_fatal("Encountered interrupt " BM_FMT_XLEN "is out of handled range", offset);
        }

        // Handling an interrupt, highest cause bit is 1
        bm_intr_handler_t handler = bm_interrupt_handler_table[offset];
        if (!handler)
        {
            bm_fatal("Interrupt " BM_FMT_XLEN " with unset handler was triggered.", offset);
        }
        // Call the configured handler
        handler(stacked_regs);
    }
    else
    {
        if (offset >= BM_ARRAY_ELEMENTS(bm_exc_handler_table))
        {
            bm_fatal("Encountered exception cause " BM_FMT_XLEN " is out of handled range", offset);
        }

        // Handling an exception, highest cause bit is 0
        bm_intr_handler_t handler = bm_exc_handler_table[offset];
        if (!handler)
        {
            bm_exception_print_details(offset);
            bm_fatal("Fatal, ending execution.");
        }
        // Call the configured handler
        handler(stacked_regs);
    }

    // Write original privilege mode value to the internal variable
    bm_current_mode = prev_mode;
}

// clang-format off
/**
 * \brief Helper macro for creating default handler functions for different privilege modes
 * - Save all registers on the current stack.
 * - Call internal C function to handle the interrupt with a copy of the current stack pointer
 *   which points to the saved registers in a bm_register_file_t structure on the stack.
 * - Restore all registers from the current stack.
 * - Exit the interrupt handler using mret, sret or uret instruction.
 */
#ifdef __CHERI_PURE_CAPABILITY__
#define CREATE_DEFAULT_HANDLER(name, priv_mode, ret)                        \
    void __attribute__((naked, aligned(TRAP_HANDLER_ALIGNMENT))) name(void) \
    {                                                                       \
        __asm__ volatile("caddi csp, csp, -%0\n"                            \
                         :: "i"(TARGET_STACK_SIZE));                        \
                         TARGET_SAVE_REGS                                   \
        __asm__ volatile("cmv  ct0, %0\n"                                   \
                         "cmv  ca1, csp\n"                                  \
                         "li   a0,  %1\n"                                   \
                         "jalr ct0\n"                                       \
                         :: "C"(bm_managed_handler_inner),                  \
                         "i"(priv_mode));                                   \
                         TARGET_LOAD_REGS                                   \
        __asm__ volatile("caddi csp, csp, %0\n"                             \
                         #ret                                               \
                         :: "i"(TARGET_STACK_SIZE));                        \
    }

#else
#define CREATE_DEFAULT_HANDLER(name, priv_mode, ret)                        \
    void __attribute__((naked, aligned(TRAP_HANDLER_ALIGNMENT))) name(void) \
    {                                                                       \
        __asm__ volatile("addi sp, sp, -%0\n"                               \
                         :: "i"(TARGET_STACK_SIZE));                        \
                         TARGET_SAVE_REGS                                   \
        __asm__ volatile("la   t0, %0\n"                                    \
                         "li   a0, %1\n"                                    \
                         "mv   a1, sp\n"                                    \
                         "jalr t0\n"                                        \
                         :: "i"(bm_managed_handler_inner),                  \
                         "i"(priv_mode));                                   \
                         TARGET_LOAD_REGS                                   \
        __asm__ volatile("addi sp, sp, %0\n"                                \
                         #ret                                               \
                         :: "i"(TARGET_STACK_SIZE));                        \
    }
#endif
// clang-format on

/**
 * \brief Separate trap vector for each privilege mode
 */
CREATE_DEFAULT_HANDLER(bm_managed_handler_m, BM_PRIV_MODE_MACHINE, mret)
#ifdef TARGET_EXT_S
CREATE_DEFAULT_HANDLER(bm_managed_handler_s, BM_PRIV_MODE_SUPERVISOR, sret)
#endif
#ifdef TARGET_EXT_N
CREATE_DEFAULT_HANDLER(bm_managed_handler_u, BM_PRIV_MODE_USER, uret)
#endif

// The following is an example on how to use the interrupt attributes.
// This is here to show a simple way to handle interrupts.
// Using these works for most Baremetal Examples except 'rdtime' and 'ecall-demo' which require
// access to the saved registers.
//
// void __attribute__((interrupt("machine"), aligned(64))) bm_managed_handler_m(void)
// {
//     bm_register_file_t stacked_regs_dummy = {0};
//     bm_managed_handler_inner(BM_PRIV_MODE_MACHINE, &stacked_regs_dummy);
// }
//
// #ifdef TARGET_EXT_S
// void __attribute__((interrupt("supervisor"), aligned(64))) bm_managed_handler_s(void)
// {
//     bm_register_file_t stacked_regs_dummy = {0};
//     bm_managed_handler_inner(BM_PRIV_MODE_SUPERVISOR, &stacked_regs_dummy);
// }
// #endif
//
// #ifdef TARGET_EXT_N
// void __attribute__((interrupt("user"), aligned(64))) bm_managed_handler_u(void)
// {
//     bm_register_file_t stacked_regs_dummy = {0};
//     bm_managed_handler_inner(BM_PRIV_MODE_USER, &stacked_regs_dummy);
// }
// #endif

void bm_interrupt_set_handler(bm_interrupt_source_t source, bm_intr_handler_t func)
{
#ifdef TARGET_HAS_CLIC
    unsigned int idx = bm_clic_get_irq_id_for_source(source);
    bm_fatal_check_index(idx, bm_interrupt_handler_table);
    bm_interrupt_handler_table[idx] = func;
#else
    bm_fatal_check_index(source, bm_interrupt_handler_table);
    bm_interrupt_handler_table[source] = func;
#endif
}

void bm_exception_set_handler(bm_exception_source_t source, bm_intr_handler_t func)
{
    bm_fatal_check_index(source, bm_exc_handler_table);
    bm_exc_handler_table[source] = func;
}

void bm_ext_irq_set_handler(unsigned ext_irq_id, bm_intr_handler_t func)
{
#ifdef TARGET_HAS_CLIC
    unsigned int idx = bm_clic_get_ext_irq_id(ext_irq_id);
    bm_fatal_check_index(idx, bm_interrupt_handler_table);
    bm_interrupt_handler_table[idx] = func;
#elif (BM_EXT_INTERRUPT_NUMBER > 0)
    bm_fatal_check_index(ext_irq_id, bm_ext_irq_handler_table);
    bm_ext_irq_handler_table[ext_irq_id] = func;
#else
    (void)func; // unused
    bm_fatal("External interrupts not supported, can't set handler for %u.", ext_irq_id);
#endif
}

void bm_interrupt_install_handlers(bm_priv_mode_t priv_mode)
{
    bm_intr_handler_t handler;
    switch (priv_mode)
    {
        case BM_PRIV_MODE_MACHINE:
            handler = (bm_intr_handler_t)bm_managed_handler_m;
            break;
#ifdef TARGET_EXT_S
        case BM_PRIV_MODE_SUPERVISOR:
            handler = (bm_intr_handler_t)bm_managed_handler_s;
            break;
#endif
#ifdef TARGET_EXT_N
        case BM_PRIV_MODE_USER:
            handler = (bm_intr_handler_t)bm_managed_handler_u;
            break;
#endif
        default:
            bm_fatal("Unsupported privilege mode %d.", priv_mode);
    }
    bm_interrupt_tvec_setup(priv_mode, (xlen_t)handler, BM_INTERRUPT_MODE_DIRECT);

#if (BM_EXT_INTERRUPT_NUMBER > 0)
    bm_interrupt_set_handler(BM_INTERRUPT_MEIP, bm_ext_irq_handler);
    #ifdef TARGET_EXT_S
    bm_interrupt_set_handler(BM_INTERRUPT_SEIP, bm_ext_irq_handler);
    #endif
    #ifdef TARGET_EXT_N
    bm_interrupt_set_handler(BM_INTERRUPT_UEIP, bm_ext_irq_handler);
    #endif
#endif
}

void bm_ext_irq_init(void)
{
#ifdef TARGET_HAS_CLIC
    bm_clic_t *clic = (bm_clic_t *)target_peripheral_get(BM_PERIPHERAL_CLIC);
    bm_clic_init(clic);
#endif
}

void bm_interrupt_init(bm_priv_mode_t priv_mode)
{
    // Setup interrupt controller
    bm_ext_irq_init();

    // Install handlers for our interrupt framework
    bm_interrupt_install_handlers(priv_mode);

    // Enable interrupts for the given privilege mode, specific interrupt
    // sources and individual interrupts still need to be enabled manually in
    // the core, interrupt controllers and periherals.
    bm_interrupt_enable(priv_mode);
}

int bm_ext_irq_claim(void)
{
#ifdef TARGET_HAS_PLIC
    bm_plic_t *plic = (bm_plic_t *)target_peripheral_get(BM_PERIPHERAL_PLIC);
    return bm_plic_claim(plic, bm_get_hartid());
#elif defined(TARGET_HAS_PIC)
    return bm_pic_get_irq();
#else
    return -1;
#endif
}

void bm_ext_irq_complete(unsigned ext_irq_id)
{
#ifdef TARGET_HAS_PLIC
    bm_plic_t *plic = (bm_plic_t *)target_peripheral_get(BM_PERIPHERAL_PLIC);
    bm_plic_complete(plic, bm_get_hartid(), ext_irq_id);
#elif defined(TARGET_HAS_PIC)
    bm_pic_clear_irq(ext_irq_id);
#else
    (void)ext_irq_id;
#endif
}

void bm_ext_irq_enable(unsigned ext_irq_id)
{
#ifdef TARGET_HAS_PLIC
    bm_plic_t *plic = (bm_plic_t *)target_peripheral_get(BM_PERIPHERAL_PLIC);
    bm_plic_set_enable(plic, bm_get_hartid(), ext_irq_id, 1);
    bm_plic_set_priority(plic, ext_irq_id, 1);
#elif defined(TARGET_HAS_CLIC)
    bm_clic_t *clic = (bm_clic_t *)target_peripheral_get(BM_PERIPHERAL_CLIC);
    bm_clic_set_enable(clic, bm_clic_get_ext_irq_id(ext_irq_id), 1);
#elif defined(TARGET_HAS_PIC)
    bm_pic_enable_source(ext_irq_id);
#else
    (void)ext_irq_id;
#endif
}

void bm_ext_irq_disable(unsigned ext_irq_id)
{
#ifdef TARGET_HAS_PLIC
    bm_plic_t *plic = (bm_plic_t *)target_peripheral_get(BM_PERIPHERAL_PLIC);
    bm_plic_set_enable(plic, bm_get_hartid(), ext_irq_id, 0);
#elif defined(TARGET_HAS_CLIC)
    bm_clic_t *clic = (bm_clic_t *)target_peripheral_get(BM_PERIPHERAL_CLIC);
    bm_clic_set_enable(clic, bm_clic_get_ext_irq_id(ext_irq_id), 0);
#elif defined(TARGET_HAS_PIC)
    bm_pic_disable_source(ext_irq_id);
#else
    (void)ext_irq_id;
#endif
}
