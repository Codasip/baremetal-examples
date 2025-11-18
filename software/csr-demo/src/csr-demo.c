/* Copyright 2023-2025 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/common.h>
#include <baremetal/csr.h>
#include <baremetal/interrupt.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Represents a register field - slice with defined meaning
 */
typedef struct {
    unsigned    offset; /**< Number of the lowest bit of the field */
    unsigned    size;   /**< Number of bits the field occupies */
    const char *name;   /**< Field name */
} register_field_t;

static const register_field_t MSTATUS_FIELDS[] = {
    {1,  1, "SIE" },
    {3,  1, "MIE" },
    {5,  1, "SPIE"},
    {6,  1, "UBE" },
    {7,  1, "MPIE"},
    {8,  1, "SPP" },
    {9,  2, "VS"  },
    {11, 2, "MPP" },
    {13, 2, "FS"  },
    {15, 2, "XS"  },
    {17, 1, "MPRV"},
    {18, 1, "SUM" },
    {19, 1, "MXR" },
    {20, 1, "TVM" },
    {21, 1, "TW"  },
    {22, 1, "TSR" },
#if RISCV_XLEN == 64
    {32, 2, "UXL" },
    {34, 2, "SXL" },
    {36, 1, "SBE" },
    {37, 1, "MBE" },
    {63, 1, "SD"  },
#endif
};

static const register_field_t MISA_FIELDS[] = {
    {0,              1, "EXT_A"},
    {1,              1, "EXT_B"},
    {2,              1, "EXT_C"},
    {3,              1, "EXT_D"},
    {4,              1, "EXT_E"},
    {5,              1, "EXT_F"},
    {8,              1, "EXT_I"},
    {12,             1, "EXT_M"},
    {13,             1, "EXT_N"},
    {18,             1, "EXT_S"},
    {20,             1, "EXT_U"},
    {RISCV_XLEN - 2, 2, "MXL"  },
};

static const unsigned MSTATUS_FIELDS_COUNT = sizeof(MSTATUS_FIELDS) / sizeof(MSTATUS_FIELDS[0]);
static const unsigned MISA_FIELDS_COUNT    = sizeof(MISA_FIELDS) / sizeof(MISA_FIELDS[0]);

static void print_register_field(const register_field_t *field, xlen_t reg_value)
{
    unsigned long value = ((reg_value >> field->offset) & ((1UL << field->size) - 1));

    printf("    %5s : %2lu\n", field->name, value);
}

int main(void)
{
    puts("Welcome to the CSR demo!");

    xlen_t csr_val = 0;

    BM_CSR_READ(BM_CSR_MVENDORID, csr_val);
    printf("  - CSR mvendorid : " BM_FMT_XLEN "\n", csr_val);

    BM_CSR_READ(BM_CSR_MARCHID, csr_val);
    printf("  - CSR marchid   : " BM_FMT_XLEN "\n", csr_val);

    BM_CSR_READ(BM_CSR_MIMPID, csr_val);
    printf("  - CSR mimpid    : " BM_FMT_XLEN "\n", csr_val);

    BM_CSR_READ(BM_CSR_MISA, csr_val);
    printf("  - CSR %-10s: " BM_FMT_XLEN "\n", "misa", csr_val);
    for (unsigned i = 0; i < MISA_FIELDS_COUNT; i++)
    {
        print_register_field(&MISA_FIELDS[i], csr_val);
    }

    BM_CSR_READ(BM_CSR_MSTATUS, csr_val);
    printf("  - CSR %-10s: " BM_FMT_XLEN "\n", "mstatus", csr_val);
    for (unsigned i = 0; i < MSTATUS_FIELDS_COUNT; i++)
    {
        print_register_field(&MSTATUS_FIELDS[i], csr_val);
    }

    printf("### Enabling interrupts to show a change in 'mstatus' ...\n");
    bm_interrupt_init(BM_PRIV_MODE_MACHINE);

    BM_CSR_READ(BM_CSR_MSTATUS, csr_val);
    printf("  - CSR %-10s: " BM_FMT_XLEN "\n", "mstatus", csr_val);
    for (unsigned i = 0; i < MSTATUS_FIELDS_COUNT; i++)
    {
        print_register_field(&MSTATUS_FIELDS[i], csr_val);
    }

    puts("Bye.");
    return EXIT_SUCCESS;
}
