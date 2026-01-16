/* Copyright 2023-2025 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include "fatfs/ff.h"
#include "file.h"
#include "loader.h"
#include "parser.h"
#include "sys_hw.h"

#include <baremetal/common.h>
#include <baremetal/csr.h>
#include <baremetal/gpio.h>
#include <baremetal/id_reg.h>
#include <baremetal/interrupt_low.h>
#include <baremetal/mem_barrier.h>
#include <baremetal/mp.h>
#include <baremetal/platform.h>
#include <baremetal/time.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tiny_printf/printf.h>

#define CONFIG_FILE_PATH "/config.txt"

#ifndef __STRINGIFY
    #define __STRINGIFY(s) #s
#endif

#ifndef STRINGIFY
    #define STRINGIFY(s) __STRINGIFY(s)
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

// OpenSBI dynamic firmware information block as defined in
// https://github.com/riscv-software-src/opensbi/blob/master/include/sbi/fw_dynamic.h
#define OPENSBI_FW_DYNAMIC_INFO_MAGIC_VALUE 0x4942534f // 'OSBI' ASCII String
#define OPENSBI_FW_DYNAMIC_INFO_VERSION_2   0x2
#define OPENSBI_FW_DYNAMIC_INFO_NEXT_MODE_U 0x0
#define OPENSBI_FW_DYNAMIC_INFO_NEXT_MODE_S 0x1
#define OPENSBI_FW_DYNAMIC_INFO_NEXT_MODE_M 0x3
typedef struct {
    xlen_t magic;
    xlen_t version;
    xlen_t next_addr;
    xlen_t next_mode;
    xlen_t options;
    xlen_t boot_hart;
} opensbi_fw_dynamic_info_t;

typedef struct {
    xlen_t boot_addr;
    // Flattened device tree address
    xlen_t fdt_addr;
    // OpenSBI next address
    xlen_t next_addr;
} boot_config_t;

typedef void (*payload_func_t)(xlen_t, xlen_t, xlen_t);

void check_ready(void *arg)
{
    (void)arg;
}

static void start_payload(void *arg)
{
    boot_config_t *config         = (boot_config_t *)arg;
    payload_func_t launch_payload = (payload_func_t)config->boot_addr;
    xlen_t         ftd            = config->fdt_addr;
    unsigned int   hart_num       = bm_get_hartid();

    // Set arguments for OpenSBI, which are passed in the same registers as function arguments.
    // See RISC-V calling conventions - https://riscv.org/wp-content/uploads/2015/01/riscv-calling.pdf
    // and OpenSBI docs - https://github.com/riscv-software-src/opensbi/blob/master/docs/firmware/fw.md
    // Note, that the parameters differ for different OpenSBI Firmwares. If less parameters are required,
    // the remaining registers will not influence further operation.
    opensbi_fw_dynamic_info_t opensbi_fw_dynamic_info = {
        .magic     = OPENSBI_FW_DYNAMIC_INFO_MAGIC_VALUE,
        .version   = OPENSBI_FW_DYNAMIC_INFO_VERSION_2,
        .next_addr = config->next_addr,
        .next_mode = OPENSBI_FW_DYNAMIC_INFO_NEXT_MODE_S,
        .options   = 0x0,
        .boot_hart = 0x0,
    };

    bm_exec_fence();
    bm_exec_fence_i();

    launch_payload((xlen_t)hart_num, ftd, (xlen_t)&opensbi_fw_dynamic_info);
}

static void exit_with_error(void)
{
    printf("CPU reset required!\n");
    exit(1);
}

/*
 * Technically, this trap handler function could also be made static, because
 * it's referenced only here when settig up the vector. However, having a
 * public symbol is more convenient for debugging purposes.
 */
void __attribute__((aligned(64))) trap_handler(void)
{
    printf("FSBL trap handler entered.\n");

    xlen_t csr_val = 0;

    BM_CSR_READ(BM_CSR_MCAUSE, csr_val);
    printf(" - CSR mcause:        " BM_FMT_XLEN "\n", csr_val);

    BM_CSR_READ(BM_CSR_MSTATUS, csr_val);
    printf(" - CSR mstatus:       " BM_FMT_XLEN "\n", csr_val);

    BM_CSR_READ(BM_CSR_MEPC, csr_val);
    printf(" - CSR mepc:          " BM_FMT_XLEN "\n", csr_val);

    BM_CSR_READ(BM_CSR_MTVAL, csr_val);
    printf(" - CSR mtval:         " BM_FMT_XLEN "\n", csr_val);

    exit_with_error();
}

#ifdef TARGET_UART
    #define MAX_BIN_FILES    (40)
    #define MAX_FILENAME_LEN (64)
    #define BIN_DIR          "/"
    #define BIN_BOOT_ADDRESS (0x20000000)

static char     bin_filenames[MAX_BIN_FILES][MAX_FILENAME_LEN];
static uint32_t bin_filenames_num = 0;
static char     input[16];

static int get_bin_files(const char *directory_path)
{
    DIR     dir;
    FILINFO entry;

    if (f_findfirst(&dir, &entry, directory_path, "*.bin") != FR_OK)
    {
        printf("Failed to open directory %s.\n", directory_path);
        return -1;
    }

    for (bin_filenames_num = 0; bin_filenames_num < MAX_BIN_FILES;)
    {
        if (strlen(entry.fname) == 0)
        {
            /* End of file list */
            break;
        }

        // Make sure it's a regular file
        if ((entry.fattrib & (AM_SYS | AM_DIR)) == 0)
        {
            // Check if the file has a ".bin" extension
            const char *filename = entry.fname;
            size_t      len      = strlen(filename);
            if (len > 4 && strcmp(filename + len - 4, ".bin") == 0)
            {
                if (directory_path[strlen(directory_path) - 1] == '/')
                {
                    snprintf(bin_filenames[bin_filenames_num],
                             MAX_FILENAME_LEN,
                             "%s%s",
                             directory_path,
                             filename);
                }
                else
                {
                    snprintf(bin_filenames[bin_filenames_num],
                             MAX_FILENAME_LEN,
                             "%s/%s",
                             directory_path,
                             filename);
                }
                bin_filenames_num++;
            }
        }

        if (f_findnext(&dir, &entry) != FR_OK)
        {
            break;
        }
    }

    f_closedir(&dir);
    return bin_filenames_num;
}

static int load_via_uart_menu(boot_config_t *config)
{
    uint32_t    selection = 0;
    const char *cli_ret;

    printf("Looking for *.bin files in %s.\n", BIN_DIR);
    int ret = get_bin_files(BIN_DIR);
    if ((ret < 0) || (bin_filenames_num == 0))
    {
        printf("Failed to find any *.bin files in %s.\n", BIN_DIR);
        return -1;
    }

    uint32_t i;
    do
    {
        printf("*.bin files in " BIN_DIR ":\n");

        for (i = 0; i < bin_filenames_num; i++)
        {
            printf("%3li %s\n", i, bin_filenames[i]);
        }

        printf("Select file to load and run at 0x%x: ", BIN_BOOT_ADDRESS);

        //i = scanf("%lu", &selection); too much code for the ROM!
        // Do this the unsafe way:
        cli_ret   = cli_gets(input, sizeof(input));
        selection = atoi(input);
        printf("\nYou selected: %i -> %s\n", selection, bin_filenames[selection]);

    } while (cli_ret == NULL || selection >= bin_filenames_num);

    if (load_sdcard_payload(BIN_BOOT_ADDRESS, bin_filenames[selection]))
    {
        printf("\nFailed to load payload '%s'.\n", bin_filenames[selection]);
        return -1;
    }

    config->boot_addr = BIN_BOOT_ADDRESS;
    return 0;
}

#endif /* TARGET_UART */

static int process_entry(entry_t *entry, boot_config_t *config)
{
    static bm_gpio_t *gpio = NULL;
    if (!gpio)
    {
        gpio = (bm_gpio_t *)target_peripheral_get(BM_PERIPHERAL_GPIO_LEDS_SWITCHES);
    }

    if ((entry->gpio >= 0) && !bm_gpio_read(gpio, gpio_switches[entry->gpio]))
    {
        printf("\nPayload '%s' skipped due to GPIO settings.\n", entry->path);
        return 0;
    }

    long payload_size = get_file_size(entry->path);
    if (payload_size < 0)
    {
        printf("\nFailed to get payload size for '%s'.\n", entry->path);
        return -1;
    }

    if ((entry->load_addr <= end_addr) &&
        (((rom_end > ram_start) ? start_addr : ram_start) <= (entry->load_addr + payload_size)))
    {
        printf("\nFailed to load payload '%s' due to overlap with FSBL RAM address space.\n",
               entry->path);
        return -1;
    }

    if (load_sdcard_payload(entry->load_addr, entry->path))
    {
        printf("\nFailed to load payload '%s'.\n", entry->path);
        return -1;
    }

    if (entry->flags & ENTRY_FLAG_BOOT)
    {
        config->boot_addr = entry->load_addr;
    }
    else if (entry->flags & ENTRY_FLAG_FDT)
    {
        config->fdt_addr = entry->load_addr;
    }
    else if (entry->flags & ENTRY_FLAG_NXT)
    {
        config->next_addr = entry->load_addr;
    }

    return 0;
}

static int load_payloads(boot_config_t *config)
{
    // try to access the config file from the SD-card.
    int ret = init_parser(CONFIG_FILE_PATH);
    if (ret != 0)
    {
        printf("Failed to open FSBL configuration file %s.\n", CONFIG_FILE_PATH);
#ifdef TARGET_UART
        // fall back to a boot menu
        return load_via_uart_menu(config);
#else
        return -1;
#endif
    }

    entry_t entry;
    while (parse_entry(&entry))
    {
        ret = process_entry(&entry, config);
        if (ret != 0)
        {
            printf("Processing configuration file failed\n");
            ret = -1;
            break;
        }
    }
    finish_parser();
    return ret;
}

static void get_misa_string(xlen_t misa, char *out)
{
    unsigned pos = 0;

    out[pos++] = 'r';
    out[pos++] = 'v';

    switch (misa >> (RISCV_XLEN - 2))
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
    // setup trap handler
    bm_interrupt_tvec_setup(BM_PRIV_MODE_MACHINE, (xlen_t)trap_handler, BM_INTERRUPT_MODE_DIRECT);

    printf("\n\n----[ Welcome to the Codasip FSBL ]----\n\n");
    printf("Build version:        %s\n", BUILD_VERSION);
    printf("Build ID:             %s\n", BUILD_ID);
    printf("\n");
    printf("Target configuration:\n");
    printf(" - Core:              %s\n", TARGET_CORE_NAME);
    printf(" - Platform:          %s\n", TARGET_PLATFORM_NAME);
    printf(" - Frequency:         %u MHz\n", (unsigned)TARGET_CLK_FREQ / 1000000);
    printf(" - Number of HARTs:   %u\n", TARGET_NUM_HARTS);
    printf("\n");
    printf("Machine information:\n");

    xlen_t csr_val = 0;
    char   misa_string[32];

    BM_CSR_READ(BM_CSR_MISA, csr_val);
    get_misa_string(csr_val, misa_string);

    printf(" - ISA string:        %s \n", misa_string);

    BM_CSR_READ(BM_CSR_MISA, csr_val);
    printf(" - CSR misa:          " BM_FMT_XLEN "\n", csr_val);

    BM_CSR_READ(BM_CSR_MVENDORID, csr_val);
    printf(" - CSR mvendorid:     " BM_FMT_XLEN "\n", csr_val);

    BM_CSR_READ(BM_CSR_MARCHID, csr_val);
    printf(" - CSR marchid:       " BM_FMT_XLEN "\n", csr_val);

    BM_CSR_READ(BM_CSR_MIMPID, csr_val);
    printf(" - CSR mimpid:        " BM_FMT_XLEN "\n", csr_val);

    printf("\n");

#ifdef ID_REGISTERS
    bm_id_reg_t *id_reg = (bm_id_reg_t *)target_peripheral_get(BM_PERIPHERAL_ID_REG);

    if (id_reg == NULL)
    {
        puts("Error: Failed to get ID_REG peripheral");
        return EXIT_FAILURE;
    }

    printf("Platform information:\n");

    printf(" - Platform version:  %u.%u\n",
           bm_id_reg_plat_version_get_major_version(id_reg),
           bm_id_reg_plat_version_get_minor_version(id_reg));

    bm_id_core_type_t core_type = bm_id_reg_core_type_get_val(id_reg);
    printf(" - Core type:         %s\n",
           core_type == BM_ID_CORE_A730   ? "A730"
           : core_type == BM_ID_CORE_L110 ? "L110"
           : core_type == BM_ID_CORE_L730 ? "L730"
           : core_type == BM_ID_CORE_L31  ? "L31"
           : core_type == BM_ID_CORE_X730 ? "X730"
           : core_type == BM_ID_CORE_L150 ? "L150"
           : core_type == BM_ID_CORE_V730 ? "V730"
                                          : "Unknown");

    printf(" - Core frequency:    %u MHz\n", bm_id_reg_core_f_get_val(id_reg));

    bm_id_eth_type_t eth_type = bm_id_reg_eth_type_get_val(id_reg);
    printf(" - Ethernet type:     %s\n",
           eth_type == BM_ID_ETH_LITE      ? "Ethernet Lite"
           : eth_type == BM_ID_ETH_GIGABIT ? "Gigabit Ethernet"
                                           : "Unknown");

    printf(" - Features:\n");
    printf("    - CHERI:          %s\n\n",
           bm_id_reg_features_get_cheri_enabled(id_reg) ? "enabled" : "disabled");

    printf(" - Security:          %s\n\n",
           bm_id_reg_security_get_securiry_enabled(id_reg) ? "enabled" : "disabled");
#endif

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

    boot_config_t config = {0};
    int           ret    = load_payloads(&config);
    if (ret < 0)
    {
        printf("Loading payload failed.\n");
        exit_with_error();
    }

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
