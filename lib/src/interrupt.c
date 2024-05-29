/* Copyright 2023 Codasip s.r.o.         */
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
#endif

#include <stdint.h>

/** \brief Table with handlers for individual exception sources */
static void (*bm_exc_handler_table[16])(void) = {0};

/** \brief Table with handlers for individual interrupt sources */
static void (*bm_interrupt_handler_table[16])(void) = {0};

/** \brief Table with handlers for individual external interrupt sources */
static void (*bm_ext_irq_handler_table[32])(void) = {0};

/** \brief Internal handler for external interrupts */
void bm_ext_irq_handler(void)
{
    int pending = bm_ext_irq_claim();

    if (pending == -1)
    {
        return;
    }

    if (!bm_ext_irq_handler_table[pending])
    {
        bm_error("An external interrupt with unset handler was triggered.");
    }

    bm_ext_irq_handler_table[pending]();

    bm_ext_irq_complete(pending);
}

// clang-format off
#if __riscv_xlen == 32
    #define BM_WORD_SIZE  "4"
    #define BM_STORE      "sw"
    #define BM_LOAD       "lw"
    #define BM_STORE_F    "fsw"
    #define BM_LOAD_F     "flw"
#else
    #define BM_WORD_SIZE  "8"
    #define BM_STORE      "sd"
    #define BM_LOAD       "ld"
    #define BM_STORE_F    "fsd"
    #define BM_LOAD_F     "fld"
#endif

#define TARGET_SAVE_EMB_REGS                            \
    BM_STORE " x2, 1 * " BM_WORD_SIZE " (x1)\n"   \
    BM_STORE " x3, 2 * " BM_WORD_SIZE " (x1)\n"   \
    BM_STORE " x4, 3 * " BM_WORD_SIZE " (x1)\n"   \
    BM_STORE " x5, 4 * " BM_WORD_SIZE " (x1)\n"   \
    BM_STORE " x6, 5 * " BM_WORD_SIZE " (x1)\n"   \
    BM_STORE " x7, 6 * " BM_WORD_SIZE " (x1)\n"   \
    BM_STORE " x8, 7 * " BM_WORD_SIZE " (x1)\n"   \
    BM_STORE " x9, 8 * " BM_WORD_SIZE " (x1)\n"   \
    BM_STORE " x10, 9 * " BM_WORD_SIZE " (x1)\n"  \
    BM_STORE " x11, 10 * " BM_WORD_SIZE " (x1)\n" \
    BM_STORE " x12, 11 * " BM_WORD_SIZE " (x1)\n" \
    BM_STORE " x13, 12 * " BM_WORD_SIZE " (x1)\n" \
    BM_STORE " x14, 13 * " BM_WORD_SIZE " (x1)\n" \
    BM_STORE " x15, 14 * " BM_WORD_SIZE " (x1)\n"

#define TARGET_LOAD_EMB_REGS                            \
    BM_LOAD " x2, 1 * " BM_WORD_SIZE " (x1)\n"    \
    BM_LOAD " x3, 2 * " BM_WORD_SIZE " (x1)\n"    \
    BM_LOAD " x4, 3 * " BM_WORD_SIZE " (x1)\n"    \
    BM_LOAD " x5, 4 * " BM_WORD_SIZE " (x1)\n"    \
    BM_LOAD " x6, 5 * " BM_WORD_SIZE " (x1)\n"    \
    BM_LOAD " x7, 6 * " BM_WORD_SIZE " (x1)\n"    \
    BM_LOAD " x8, 7 * " BM_WORD_SIZE " (x1)\n"    \
    BM_LOAD " x9, 8 * " BM_WORD_SIZE " (x1)\n"    \
    BM_LOAD " x10, 9 * " BM_WORD_SIZE " (x1)\n"   \
    BM_LOAD " x11, 10 * " BM_WORD_SIZE " (x1)\n"  \
    BM_LOAD " x12, 11 * " BM_WORD_SIZE " (x1)\n"  \
    BM_LOAD " x13, 12 * " BM_WORD_SIZE " (x1)\n"  \
    BM_LOAD " x14, 13 * " BM_WORD_SIZE " (x1)\n"  \
    BM_LOAD " x15, 14 * " BM_WORD_SIZE " (x1)\n"

#define TARGET_SAVE_ALL_REGS                             \
    TARGET_SAVE_EMB_REGS                                 \
    BM_STORE " x16, 15 * " BM_WORD_SIZE " (x1)\n"  \
    BM_STORE " x17, 16 * " BM_WORD_SIZE " (x1)\n"  \
    BM_STORE " x18, 17 * " BM_WORD_SIZE " (x1)\n"  \
    BM_STORE " x19, 18 * " BM_WORD_SIZE " (x1)\n"  \
    BM_STORE " x20, 19 * " BM_WORD_SIZE " (x1)\n"  \
    BM_STORE " x21, 20 * " BM_WORD_SIZE " (x1)\n"  \
    BM_STORE " x22, 21 * " BM_WORD_SIZE " (x1)\n"  \
    BM_STORE " x23, 22 * " BM_WORD_SIZE " (x1)\n"  \
    BM_STORE " x24, 23 * " BM_WORD_SIZE " (x1)\n"  \
    BM_STORE " x25, 24 * " BM_WORD_SIZE " (x1)\n"  \
    BM_STORE " x26, 25 * " BM_WORD_SIZE " (x1)\n"  \
    BM_STORE " x27, 26 * " BM_WORD_SIZE " (x1)\n"  \
    BM_STORE " x28, 27 * " BM_WORD_SIZE " (x1)\n"  \
    BM_STORE " x29, 28 * " BM_WORD_SIZE " (x1)\n"  \
    BM_STORE " x30, 29 * " BM_WORD_SIZE " (x1)\n"  \
    BM_STORE " x31, 30 * " BM_WORD_SIZE " (x1)\n"

#define TARGET_LOAD_ALL_REGS                             \
    TARGET_LOAD_EMB_REGS                                 \
    BM_LOAD " x16, 15 * " BM_WORD_SIZE " (x1)\n"   \
    BM_LOAD " x17, 16 * " BM_WORD_SIZE " (x1)\n"   \
    BM_LOAD " x18, 17 * " BM_WORD_SIZE " (x1)\n"   \
    BM_LOAD " x19, 18 * " BM_WORD_SIZE " (x1)\n"   \
    BM_LOAD " x20, 19 * " BM_WORD_SIZE " (x1)\n"   \
    BM_LOAD " x21, 20 * " BM_WORD_SIZE " (x1)\n"   \
    BM_LOAD " x22, 21 * " BM_WORD_SIZE " (x1)\n"   \
    BM_LOAD " x23, 22 * " BM_WORD_SIZE " (x1)\n"   \
    BM_LOAD " x24, 23 * " BM_WORD_SIZE " (x1)\n"   \
    BM_LOAD " x25, 24 * " BM_WORD_SIZE " (x1)\n"   \
    BM_LOAD " x26, 25 * " BM_WORD_SIZE " (x1)\n"   \
    BM_LOAD " x27, 26 * " BM_WORD_SIZE " (x1)\n"   \
    BM_LOAD " x28, 27 * " BM_WORD_SIZE " (x1)\n"   \
    BM_LOAD " x29, 28 * " BM_WORD_SIZE " (x1)\n"   \
    BM_LOAD " x30, 29 * " BM_WORD_SIZE " (x1)\n"   \
    BM_LOAD " x31, 30 * " BM_WORD_SIZE " (x1)\n"

#define TARGET_SAVE_FLOAT_REGS                             \
    BM_STORE_F " f0, 31 * " BM_WORD_SIZE " (x1)\n"   \
    BM_STORE_F " f1, 32 * " BM_WORD_SIZE " (x1)\n"   \
    BM_STORE_F " f2, 33 * " BM_WORD_SIZE " (x1)\n"   \
    BM_STORE_F " f3, 34 * " BM_WORD_SIZE " (x1)\n"   \
    BM_STORE_F " f4, 35 * " BM_WORD_SIZE " (x1)\n"   \
    BM_STORE_F " f5, 36 * " BM_WORD_SIZE " (x1)\n"   \
    BM_STORE_F " f6, 37 * " BM_WORD_SIZE " (x1)\n"   \
    BM_STORE_F " f7, 38 * " BM_WORD_SIZE " (x1)\n"   \
    BM_STORE_F " f8, 39 * " BM_WORD_SIZE " (x1)\n"   \
    BM_STORE_F " f9, 40 * " BM_WORD_SIZE " (x1)\n"   \
    BM_STORE_F " f10, 41 * " BM_WORD_SIZE " (x1)\n"  \
    BM_STORE_F " f11, 42 * " BM_WORD_SIZE " (x1)\n"  \
    BM_STORE_F " f12, 43 * " BM_WORD_SIZE " (x1)\n"  \
    BM_STORE_F " f13, 44 * " BM_WORD_SIZE " (x1)\n"  \
    BM_STORE_F " f14, 45 * " BM_WORD_SIZE " (x1)\n"  \
    BM_STORE_F " f15, 46 * " BM_WORD_SIZE " (x1)\n"  \
    BM_STORE_F " f16, 47 * " BM_WORD_SIZE " (x1)\n"  \
    BM_STORE_F " f17, 48 * " BM_WORD_SIZE " (x1)\n"  \
    BM_STORE_F " f18, 49 * " BM_WORD_SIZE " (x1)\n"  \
    BM_STORE_F " f19, 50 * " BM_WORD_SIZE " (x1)\n"  \
    BM_STORE_F " f20, 51 * " BM_WORD_SIZE " (x1)\n"  \
    BM_STORE_F " f21, 52 * " BM_WORD_SIZE " (x1)\n"  \
    BM_STORE_F " f22, 53 * " BM_WORD_SIZE " (x1)\n"  \
    BM_STORE_F " f23, 54 * " BM_WORD_SIZE " (x1)\n"  \
    BM_STORE_F " f24, 55 * " BM_WORD_SIZE " (x1)\n"  \
    BM_STORE_F " f25, 56 * " BM_WORD_SIZE " (x1)\n"  \
    BM_STORE_F " f26, 57 * " BM_WORD_SIZE " (x1)\n"  \
    BM_STORE_F " f27, 58 * " BM_WORD_SIZE " (x1)\n"  \
    BM_STORE_F " f28, 59 * " BM_WORD_SIZE " (x1)\n"  \
    BM_STORE_F " f29, 60 * " BM_WORD_SIZE " (x1)\n"  \
    BM_STORE_F " f30, 61 * " BM_WORD_SIZE " (x1)\n"  \
    BM_STORE_F " f31, 62 * " BM_WORD_SIZE " (x1)\n"

#define TARGET_LOAD_FLOAT_REGS                             \
    BM_LOAD_F " f0, 31 * " BM_WORD_SIZE " (x1)\n"    \
    BM_LOAD_F " f1, 32 * " BM_WORD_SIZE " (x1)\n"    \
    BM_LOAD_F " f2, 33 * " BM_WORD_SIZE " (x1)\n"    \
    BM_LOAD_F " f3, 34 * " BM_WORD_SIZE " (x1)\n"    \
    BM_LOAD_F " f4, 35 * " BM_WORD_SIZE " (x1)\n"    \
    BM_LOAD_F " f5, 36 * " BM_WORD_SIZE " (x1)\n"    \
    BM_LOAD_F " f6, 37 * " BM_WORD_SIZE " (x1)\n"    \
    BM_LOAD_F " f7, 38 * " BM_WORD_SIZE " (x1)\n"    \
    BM_LOAD_F " f8, 39 * " BM_WORD_SIZE " (x1)\n"    \
    BM_LOAD_F " f9, 40 * " BM_WORD_SIZE " (x1)\n"    \
    BM_LOAD_F " f10, 41 * " BM_WORD_SIZE " (x1)\n"   \
    BM_LOAD_F " f11, 42 * " BM_WORD_SIZE " (x1)\n"   \
    BM_LOAD_F " f12, 43 * " BM_WORD_SIZE " (x1)\n"   \
    BM_LOAD_F " f13, 44 * " BM_WORD_SIZE " (x1)\n"   \
    BM_LOAD_F " f14, 45 * " BM_WORD_SIZE " (x1)\n"   \
    BM_LOAD_F " f15, 46 * " BM_WORD_SIZE " (x1)\n"   \
    BM_LOAD_F " f16, 47 * " BM_WORD_SIZE " (x1)\n"   \
    BM_LOAD_F " f17, 48 * " BM_WORD_SIZE " (x1)\n"   \
    BM_LOAD_F " f18, 49 * " BM_WORD_SIZE " (x1)\n"   \
    BM_LOAD_F " f19, 50 * " BM_WORD_SIZE " (x1)\n"   \
    BM_LOAD_F " f20, 51 * " BM_WORD_SIZE " (x1)\n"   \
    BM_LOAD_F " f21, 52 * " BM_WORD_SIZE " (x1)\n"   \
    BM_LOAD_F " f22, 53 * " BM_WORD_SIZE " (x1)\n"   \
    BM_LOAD_F " f23, 54 * " BM_WORD_SIZE " (x1)\n"   \
    BM_LOAD_F " f24, 55 * " BM_WORD_SIZE " (x1)\n"   \
    BM_LOAD_F " f25, 56 * " BM_WORD_SIZE " (x1)\n"   \
    BM_LOAD_F " f26, 57 * " BM_WORD_SIZE " (x1)\n"   \
    BM_LOAD_F " f27, 58 * " BM_WORD_SIZE " (x1)\n"   \
    BM_LOAD_F " f28, 59 * " BM_WORD_SIZE " (x1)\n"   \
    BM_LOAD_F " f29, 60 * " BM_WORD_SIZE " (x1)\n"   \
    BM_LOAD_F " f30, 61 * " BM_WORD_SIZE " (x1)\n"   \
    BM_LOAD_F " f31, 62 * " BM_WORD_SIZE " (x1)\n"
// clang-format on

/**
 * \brief Helper macros to save/restore all other registers on/from addresses saved in x1 register
 */
#ifdef __riscv_32e
    #define TARGET_SAVE_REGS TARGET_SAVE_EMB_REGS
    #define TARGET_LOAD_REGS TARGET_LOAD_EMB_REGS
    #define TARGET_NUM_REGS  15
#elif defined(__riscv_flen)
    #define TARGET_SAVE_REGS \
        TARGET_SAVE_ALL_REGS \
        TARGET_SAVE_FLOAT_REGS
    #define TARGET_LOAD_REGS \
        TARGET_LOAD_ALL_REGS \
        TARGET_LOAD_FLOAT_REGS
    #define TARGET_NUM_REGS 63
#else
    #define TARGET_SAVE_REGS TARGET_SAVE_ALL_REGS
    #define TARGET_LOAD_REGS TARGET_LOAD_ALL_REGS
    #define TARGET_NUM_REGS  31
#endif

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
static void bm_managed_handler_inner(bm_priv_mode_t new_mode)
{
    // Update internal variable holding privilege mode
    bm_priv_mode_t prev_mode = bm_current_mode;
    bm_current_mode          = new_mode;

    // Get the value of [m,s,u]cause register
    bm_csr_id xcause = bm_priv_get_csr_id(bm_get_priv_mode(), BM_PRIV_CSR_XCAUSE);
    xlen_t    cause  = bm_csr_read(xcause);

    // Get the offset in handler tables by clearing highest cause bit (interrupt bit)
    xlen_t offset = cause & (~(xlen_t)0 >> 1);
    if (offset >= 16)
    {
        bm_error("Encountered cause is out of handled range");
    }

    if (cause >> (__riscv_xlen - 1))
    {
        // Handling an interrupt, highest cause bit is 1
        if (!bm_interrupt_handler_table[offset])
        {
            bm_error("An interrupt with unset handler was triggered.");
        }

        // Call the configured handler
        bm_interrupt_handler_table[offset]();
    }
    else
    {
        // Handling an exception, highest cause bit is 0
        if (!bm_exc_handler_table[offset])
        {
            bm_warn("Exception handler was called:");
            switch (cause)
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
                default:
                    bm_warn("Unknown exception.");
                    break;
            }
            bm_error("Fatal, ending execution.");
        }

        // Call the configured handler
        bm_exc_handler_table[offset]();
    }

    // Write original privilege mode value to the internal variable
    bm_current_mode = prev_mode;
}

// clang-format off
/**
 * \brief Helper macro for creating default handler functions for different privilege modes
 *
 * - Save all registers in the dedicated structure for the handlers privilege mode.
 * - Check whether a previous stack pointer is saved for the handlers privilege mode
 *   - if yes, load the saved value to the stack pointer.
 *   - otherwise, we are in handler called from the same privilege mode, and continue with the current stack.
 * - Call internal C function to handle the interrupt.
 * - Restore all registers, including stack pointer, from the dedicated structure.
 * - Exit the interrupt handler using mret, sret or uret instruction.
 */
#define CREATE_DEFAULT_HANDLER(name, priv_mode, scratch, ret) \
    void __attribute__((naked, aligned(16))) name(void)       \
    {                                                         \
        __asm__ volatile("csrw " #scratch ", x1\n"            \
                         "la x1, %0\n"                        \
                         TARGET_SAVE_REGS                     \
                         "mv t0, x1\n"                        \
                         "csrr x1, " #scratch " \n"           \
                         BM_STORE " x1, 0(t0)\n"              \
                         "la t0, %1\n"                        \
                         BM_LOAD " t0, 0 (t0)\n"              \
                         "beqz t0, 1f\n"                      \
                         "mv sp, t0\n"                        \
                         "1:\n"                               \
                         "la t0, %2\n"                        \
                         "li a0, %3\n"                        \
                         "jalr t0\n"                          \
                         "la x1, %0\n"                        \
                         TARGET_LOAD_REGS                     \
                         BM_LOAD " x1, 0(x1)\n"               \
                         #ret                                 \
                         ::"i"(&bm_priv_regs[priv_mode]),     \
                         "i"(&bm_priv_sp[priv_mode]),         \
                         "i"(bm_managed_handler_inner),       \
                         "i"(priv_mode));                     \
    }
// clang-format on

/**
 * \brief Separate trap vector for each privilege mode
 */
CREATE_DEFAULT_HANDLER(bm_managed_handler_m, BM_PRIV_MODE_MACHINE, mscratch, mret)
#ifdef TARGET_EXT_S
CREATE_DEFAULT_HANDLER(bm_managed_handler_s, BM_PRIV_MODE_SUPERVISOR, sscratch, sret)
#endif
#ifdef TARGET_EXT_N
CREATE_DEFAULT_HANDLER(bm_managed_handler_u, BM_PRIV_MODE_USER, uscratch, uret)
#endif

void bm_interrupt_set_handler(bm_interrupt_source_t cause, void (*func)(void))
{
    bm_interrupt_handler_table[cause] = func;
}

void bm_exception_set_handler(bm_exception_source_t cause, void (*func)(void))
{
    bm_exc_handler_table[cause] = func;
}

void bm_ext_irq_set_handler(unsigned ext_irq_id, void (*func)(void))
{
    bm_ext_irq_handler_table[ext_irq_id] = func;
}

void bm_interrupt_init(bm_priv_mode_t priv_mode)
{
    xlen_t handler;
    switch (priv_mode)
    {
        case BM_PRIV_MODE_MACHINE:
            handler = (xlen_t)bm_managed_handler_m;
            break;
#ifdef TARGET_EXT_S
        case BM_PRIV_MODE_SUPERVISOR:
            handler = (xlen_t)bm_managed_handler_s;
            break;
#endif
#ifdef TARGET_EXT_N
        case BM_PRIV_MODE_USER:
            handler = (xlen_t)bm_managed_handler_u;
            break;
#endif
        default:
            bm_error("Unsupported privilege mode.");
    }
    bm_interrupt_tvec_setup(priv_mode, handler, BM_INTERRUPT_MODE_DIRECT);

    bm_interrupt_set_handler(BM_INTERRUPT_MEIP, bm_ext_irq_handler);
#ifdef TARGET_EXT_S
    bm_interrupt_set_handler(BM_INTERRUPT_SEIP, bm_ext_irq_handler);
#endif
#ifdef TARGET_EXT_N
    bm_interrupt_set_handler(BM_INTERRUPT_UEIP, bm_ext_irq_handler);
#endif

    // Global enable for the given privilege mode, individual interrupt sources need to be enabled manually
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
#elif defined(TARGET_HAS_PIC)
    bm_pic_disable_source(ext_irq_id);
#else
    (void)ext_irq_id;
#endif
}
