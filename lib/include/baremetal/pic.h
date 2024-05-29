/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef BAREMETAL_PIC_H
#define BAREMETAL_PIC_H

#include "baremetal/csr.h"

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Get the interrupt ID of the highest pending interrupt source
 *
 * \return Interrupt ID on success -1 on error
 */
int bm_pic_get_irq(void);

/**
 * \brief Clear given pending interrupt source
 */
void bm_pic_clear_irq(unsigned ext_irq_id);

/**
 * \brief Enable specific external interrupt source (unmask it in CSR)
 *
 * \param ext_irq_id External interrupt ID to enable
 */
void bm_pic_enable_source(unsigned ext_irq_id);

/**
 * \brief Disable specific external interrupt source (mask it in CSR)
 *
 * \param ext_irq_id External interrupt ID to disable
 */
void bm_pic_disable_source(unsigned ext_irq_id);

/**
 * \brief Enable all external interrupt sources
 */
void bm_pic_enable_all(void);

/**
 * \brief Disable all external interrupt sources
 */
void bm_pic_disable_all(void);

#ifdef __cplusplus
}
#endif

#endif /* BAREMETAL_PIC_H */
