/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "file.h"
#include "loader.h"
#include "parser.h"

#include <baremetal/common.h>
#include <baremetal/csr.h>
#include <baremetal/gpio.h>
#include <baremetal/id_reg.h>
#include <baremetal/interrupt_low.h>
#include <baremetal/mem_barrier.h>
#include <baremetal/mp.h>
#include <baremetal/platform.h>
#include <baremetal/time.h>
#include <stdlib.h>
#include <tiny_printf/printf.h>

#define CONFIG_FILE_PATH "/config.txt"

#ifdef TARGET_LINUX_SUPPORT
    #define LINUX_SUPPORT_STR "enabled"
#else
    #define LINUX_SUPPORT_STR "disabled"
#endif

/* Symbols defined in linker script */
extern int _start;
extern int _end;
extern int __data_begin;
int __attribute__((weak)) __data_rom_end = 0;

static xlen_t start_addr = (uintptr_t)(&_start);
static xlen_t end_addr   = (uintptr_t)(&_end);
static xlen_t ram_start  = (uintptr_t)(&__data_begin);
static xlen_t rom_end    = (uintptr_t)(&__data_rom_end);

static const unsigned gpio_switches[] = {TARGET_GPIO_PORT_SWITCH0,
                                         TARGET_GPIO_PORT_SWITCH1,
                                         TARGET_GPIO_PORT_SWITCH2,
                                         TARGET_GPIO_PORT_SWITCH3,
                                         TARGET_GPIO_PORT_SWITCH4,
                                         TARGET_GPIO_PORT_SWITCH5,
                                         TARGET_GPIO_PORT_SWITCH6,
                                         TARGET_GPIO_PORT_SWITCH7};

#ifdef TARGET_LINUX_SUPPORT
// See https://github.com/riscv-software-src/opensbi/blob/master/include/sbi/fw_dynamic.h
typedef struct {
    unsigned long magic;
    unsigned long version;
    unsigned long next_addr;
    unsigned long next_mode;
    unsigned long options;
    unsigned long boot_hart;
} fw_dynamic_info_t;

typedef struct {
    xlen_t boot_addr;
    // Flattened device tree address
    xlen_t fdt_addr;
    // OpenSBI next address
    xlen_t next_addr;
} boot_config_t;

typedef void (*payload_func_t)(xlen_t, xlen_t, xlen_t);
#else
typedef struct {
    xlen_t boot_addr;
} boot_config_t;

typedef void (*payload_func_t)(void);
#endif

void check_ready(void *arg)
{
    (void)arg;
}

void start_payload(void *arg)
{
    unsigned       hart_num       = bm_get_hartid();
    boot_config_t *config         = (boot_config_t *)arg;
    payload_func_t launch_payload = (payload_func_t)config->boot_addr;

    bm_exec_fence();
    bm_exec_fence_i();

#ifdef TARGET_LINUX_SUPPORT
    // Set arguments for OpenSBI, which are passed in the same registers as function arguments.
    // See RISC-V calling conventions - https://riscv.org/wp-content/uploads/2015/01/riscv-calling.pdf
    // and OpenSBI docs - https://github.com/riscv-software-src/opensbi/blob/master/docs/firmware/fw.md
    // Note, that the parameters differ for different OpenSBI Firmwares. If less parameters are required,
    // the remaining registers will not influence further operation.

    fw_dynamic_info_t fw_info = {.magic     = 0x4942534f,
                                 .version   = 0x2,
                                 .next_addr = config->next_addr,
                                 .next_mode = 0x1,
                                 .options   = 0x0,
                                 .boot_hart = 0x0};

    xlen_t info_p = config->next_addr ? (xlen_t)&fw_info : 0;

    bm_exec_fence();

    launch_payload((xlen_t)hart_num, config->fdt_addr, info_p);
#else
    launch_payload();
#endif
}

static void exit_with_error(void)
{
    printf("CPU reset required!\n");
    exit(1);
}

void __attribute__((aligned(64))) trap_handler(void)
{
    printf("FSBL trap handler entered.\n");
    printf(" - CSR mcause:        " BM_FMT_XLEN "\n", bm_csr_read(BM_CSR_MCAUSE));
    printf(" - CSR mstatus:       " BM_FMT_XLEN "\n", bm_csr_read(BM_CSR_MSTATUS));
    printf(" - CSR mepc:          " BM_FMT_XLEN "\n", bm_csr_read(BM_CSR_MEPC));
    printf(" - CSR mtval:         " BM_FMT_XLEN "\n", bm_csr_read(BM_CSR_MTVAL));
    exit_with_error();
}

boot_config_t load_payloads(void)
{
    if (init_parser(CONFIG_FILE_PATH))
    {
        printf("Failed to open FSBL configuration file " CONFIG_FILE_PATH ".\n");
        exit_with_error();
    }

    xlen_t boot_addr = 0;
#ifdef TARGET_LINUX_SUPPORT
    xlen_t opensbi_next_addr = 0, fdt_addr = 0;
#endif

    bm_gpio_t *gpio = (bm_gpio_t *)target_peripheral_get(BM_PERIPHERAL_GPIO_LEDS_SWITCHES);

    entry_t entry;

    while (parse_entry(&entry))
    {
        char  *path      = entry.path;
        xlen_t load_addr = entry.load_addr;

        if (entry.gpio >= 0 && !bm_gpio_read(gpio, gpio_switches[entry.gpio]))
        {
            printf("\nPayload '%s' skipped due to GPIO settings.\n", path);
            continue;
        }

        long payload_size = get_file_size(path);
        if (payload_size < 0)
        {
            printf("\nFailed to get payload size for '%s'.\n", path);
            exit_with_error();
        }

        if (load_addr <= end_addr &&
            ((rom_end > ram_start) ? start_addr : ram_start) <= (load_addr + payload_size))
        {
            printf("\nFailed to load payload '%s' due to overlap with FSBL RAM address space.\n", path);
            exit_with_error();
        }

        if (load_sdcard_payload(load_addr, path))
        {
            printf("\nFailed to load payload '%s'.\n", path);
            exit_with_error();
        }

        if (!boot_addr && (entry.flags & ENTRY_FLAG_BOOT))
        {
            boot_addr = load_addr;
        }
#ifdef TARGET_LINUX_SUPPORT
        else if (!fdt_addr && (entry.flags & ENTRY_FLAG_FDT))
        {
            fdt_addr = load_addr;
        }
        else if (!opensbi_next_addr && (entry.flags & ENTRY_FLAG_NXT))
        {
            opensbi_next_addr = load_addr;
        }
#endif
    }

    finish_parser();

#ifdef TARGET_LINUX_SUPPORT
    return (boot_config_t){boot_addr, fdt_addr, opensbi_next_addr};
#else
    return (boot_config_t){boot_addr};
#endif
}

void get_misa_string(xlen_t misa, char *out)
{
    unsigned pos = 0;

    out[pos++] = 'r';
    out[pos++] = 'v';

    switch (misa >> (__riscv_xlen - 2))
    {
        case 1:
            out[pos++] = '3';
            out[pos++] = '2';
            break;
        case 2:
            out[pos++] = '6';
            out[pos++] = '4';
            break;
        case 3:
            out[pos++] = '1';
            out[pos++] = '2';
            out[pos++] = '8';
            break;
        default:
            out[pos++] = 'X';
            break;
    }

    const char valid_isa_order[] = "iemafdcbn";

    for (unsigned i = 0; i < sizeof(valid_isa_order) - 1; i++)
    {
        // misa extension fields are ordered alphabetically
        unsigned misa_offset = valid_isa_order[i] - 'a';
        if (misa & (1 << misa_offset))
        {
            out[pos++] = valid_isa_order[i];
        }
    }

    out[pos] = '\0';
}

int main(void)
{
    printf("\n\n----[ Welcome to the Codasip FSBL ]----\n\n");
    printf("Build version:        %s\n", BUILD_VERSION);
    printf("Build ID:             %s\n", BUILD_ID);
    printf("\n");
    printf("Target configuration:\n");
    printf(" - Core:              %s\n", TARGET_CORE_NAME);
    printf(" - Platform:          %s\n", TARGET_PLATFORM_NAME);
    printf(" - Frequency:         %u MHz\n", (unsigned)TARGET_CLK_FREQ / 1000000);
    printf(" - Number of HARTs:   %u\n", TARGET_NUM_HARTS);
    printf(" - Linux support:     %s\n", LINUX_SUPPORT_STR);
    printf("\n");
    printf("Machine information:\n");

    char misa_string[32];
    get_misa_string(bm_csr_read(BM_CSR_MISA), misa_string);

    printf(" - ISA string:        %s \n", misa_string);
    printf(" - CSR misa:          " BM_FMT_XLEN "\n", bm_csr_read(BM_CSR_MISA));
    printf(" - CSR mvendorid:     " BM_FMT_XLEN "\n", bm_csr_read(BM_CSR_MVENDORID));
    printf(" - CSR marchid:       " BM_FMT_XLEN "\n", bm_csr_read(BM_CSR_MARCHID));
    printf(" - CSR mimpid:        " BM_FMT_XLEN "\n", bm_csr_read(BM_CSR_MIMPID));
    printf("\n");

    bm_id_reg_t *id_reg = (bm_id_reg_t *)target_peripheral_get(BM_PERIPHERAL_ID_REG);
    if (id_reg)
    {
        printf("Platform information:\n");
        printf(" - Platform version:  %u.%u\n",
               bm_id_get_info(id_reg, BM_ID_PLAT_VERSION_MAJOR),
               bm_id_get_info(id_reg, BM_ID_PLAT_VERSION_MINOR));
        int core_type = bm_id_get_info(id_reg, BM_ID_CORE_TYPE);
        printf(" - Core type:         %s\n",
               core_type == BM_ID_CORE_A730   ? "A730"
               : core_type == BM_ID_CORE_L110 ? "L110"
                                              : "Unknown");
        printf(" - Core frequency:    %u MHz\n", bm_id_get_info(id_reg, BM_ID_CORE_FREQ));
        int eth_type = bm_id_get_info(id_reg, BM_ID_ETH_TYPE);
        printf(" - Ethernet type:     %s\n",
               eth_type == BM_ID_ETH_LITE      ? "Ethernet Lite"
               : eth_type == BM_ID_ETH_GIGABIT ? "Gigabit Ethernet"
                                               : "Unknown");
        int features = bm_id_get_info(id_reg, BM_ID_FEATURES);
        printf(" - Features:\n");
        printf("    - CHERI:          %s\n\n",
               (features & BM_ID_FEATURE_CHERI) ? "enabled" : "disabled");
    }

    printf("Memory occupation:\n");

    if (rom_end > ram_start)
    {
        // Only a single memory region is used
        printf(" - RAM:               " BM_FMT_XLEN " - " BM_FMT_XLEN "\n", start_addr, end_addr);
    }
    else
    {
        printf(" - ROM:               " BM_FMT_XLEN " - " BM_FMT_XLEN "\n", start_addr, rom_end);
        printf(" - RAM:               " BM_FMT_XLEN " - " BM_FMT_XLEN "\n", ram_start, end_addr);
    }
    printf("\n");

    // setup trap handler
    bm_interrupt_tvec_setup(BM_PRIV_MODE_MACHINE, (xlen_t)trap_handler, BM_INTERRUPT_MODE_DIRECT);

    boot_config_t config = load_payloads();

    if (!config.boot_addr)
    {
        printf("Invalid FSBL configuration: missing boot address.\n");
        exit_with_error();
    }

    printf("\nBoot Configuration:\n");
    printf(" - Boot address:      " BM_FMT_XLEN "\n", config.boot_addr);
#ifdef TARGET_LINUX_SUPPORT
    printf(" - FDT address:       " BM_FMT_XLEN "\n", config.fdt_addr);
    printf(" - Next address:      " BM_FMT_XLEN "\n", config.next_addr);
#endif
    printf("\n");

#if (TARGET_NUM_HARTS > 1)
    // check all harts are ready
    for (unsigned i = 1; i < TARGET_NUM_HARTS; ++i)
    {
        bm_hart_start(i, &check_ready, NULL);

        unsigned start_time = bm_get_time_ms();
        while (bm_hart_running(i) && (bm_get_time_ms() < start_time + 1000))
            ;

        if (bm_hart_running(i))
        {
            printf("Failed to examine hart %u!\n\n", i);
        }
    }

    // start job on each hart
    for (unsigned i = 1; i < TARGET_NUM_HARTS; ++i)
    {
        bm_hart_start(i, &start_payload, (void *)&config);
    }
#endif
    start_payload((void *)&config);

    // should not get here
    exit_with_error();
}
