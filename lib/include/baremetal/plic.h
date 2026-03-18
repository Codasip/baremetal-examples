/* Copyright 2023-2026 Codasip s.r.o.    */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef BAREMETAL_PLIC_H
#define BAREMETAL_PLIC_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Maximum numbers of sources, targets (defined by specification)
#define PLIC_MAX_SOURCES        1023
#define PLIC_MAX_TARGETS        15872
#define PLIC_MAX_PACKED_SOURCES ((PLIC_MAX_SOURCES + 31) / 32)

struct bm_plic_regs {
    // Interrupt source 0 does not exist
    uint8_t                 _reserved1[0x4];
    volatile uint32_t       PRIO[PLIC_MAX_SOURCES];
    const volatile uint32_t PENDING[PLIC_MAX_PACKED_SOURCES];
    uint8_t                 _reserved2[0xF80];
    // ENABLE registers start at (1 + 1023 + 32 + 992) * 4 = 0x2000 offset
    volatile uint32_t ENABLE[PLIC_MAX_TARGETS][PLIC_MAX_PACKED_SOURCES];
    uint8_t           _reserved3[0xE000];
    // Context registers start at 0x2000 + (15872 * 32 + 14336) * 4 = 0x200000 offset
    /** \brief Structure representing PLIC context configuration registers */
    struct bm_plic_context {
        volatile uint32_t THRESHOLD;
        union {
            const volatile uint32_t CLAIM;
            volatile uint32_t       COMPLETE;
        };
        // Each THRESHOLD register is aligned to (2 + 1022) * 4 = 0x1000
        uint8_t _reserved1[0xFF8];
    } CONTEXT[PLIC_MAX_TARGETS];
};

/** \brief Structure describing PLIC peripheral registers */
typedef struct bm_plic_regs bm_plic_regs_t;

/** \brief Structure holding data neccessary to service the PLIC peripheral. */
typedef struct {
    bm_plic_regs_t *regs; ///< Pointer to the peripheral registers
} bm_plic_t;

/**
 * \brief Set the interrupt priority
 *
 * \param plic PLIC device
 * \param ext_irq_id External interrupt identifier
 * \param priority Interrupt priority value (0 means disabled)
 */
void bm_plic_set_priority(bm_plic_t *plic, unsigned ext_irq_id, uint32_t priority);

/**
 * \brief Get the interrupt priority
 *
 * \param plic PLIC device
 * \param ext_irq_id External interrupt identifier
 *
 * \return Interrupt priority value (0 means disabled)
 */
uint32_t bm_plic_get_priority(bm_plic_t *plic, unsigned ext_irq_id);

/**
 * \brief Set the hart threshold
 *
 * \param plic PLIC device
 * \param context Hart identifier
 * \param threshold Threshold value above which interrupts are driven
 */
void bm_plic_set_threshold(bm_plic_t *plic, unsigned context, uint32_t threshold);

/**
 * \brief Get the hart threshold
 *
 * \param plic PLIC device
 * \param context Hart identifier
 *
 * \return Threshold value above which interrupts are driven
 */
uint32_t bm_plic_get_threshold(bm_plic_t *plic, unsigned context);

/**
 * \brief Enable the interrupt source connected to the given hart
 *
 * \param plic PLIC device
 * \param context Hart identifier
 * \param ext_irq_id External interrupt identifier
 * \param en Enable or disable bit
 */
void bm_plic_set_enable(bm_plic_t *plic, unsigned context, unsigned ext_irq_id, bool en);

/**
 * \brief Enable the interrupt sources connected to the given hart
 *
 * \param plic PLIC device
 * \param context Hart identifier
 * \param index Source, or rather, register index
 * \param source_mask Value that should be written to registers pointed to by index
 */
void bm_plic_set_enables(bm_plic_t *plic, unsigned context, unsigned index, uint32_t source_mask);

/**
 * \brief Get the interrupt enable bit connected to the hart
 *
 * \param plic PLIC device
 * \param context Hart identifier
 * \param ext_irq_id External interrupt identifier
 *
 * \return Whether the interrupt is enabled for the given hart
 */
bool bm_plic_get_enable(bm_plic_t *plic, unsigned context, unsigned ext_irq_id);

/**
 * \brief Get the interrupt enable bits connected to the hart
 *
 * \param plic PLIC device
 * \param context Hart identifier
 * \param index Index of register with enable bits
 *
 * \return Enable bit-vector for given interrupts on given hart
 */
uint32_t bm_plic_get_enables(bm_plic_t *plic, unsigned context, unsigned index);

/**
 * \brief Get the highest pending, unclaimed interrupt identifier, and claim it
 *
 * \param plic PLIC device
 * \param context Hart identifier
 *
 * \return External interrupt identifier or -1
 */
int bm_plic_claim(bm_plic_t *plic, unsigned context);

/**
 * \brief Complete the given interrupt
 *
 * \param context Hart identifier
 * \param ext_irq_id External interrupt identifier
 */
void bm_plic_complete(bm_plic_t *plic, unsigned context, unsigned ext_irq_id);

#ifdef __cplusplus
}
#endif

#endif /* BAREMETAL_PLIC_H */
