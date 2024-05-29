/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef BAREMETAL_HPM_H
#define BAREMETAL_HPM_H

#ifndef TARGET_HAS_HPM
    #error HPM not supported for this target.
#endif

#include "baremetal/common.h"

#include <stdint.h>
#include <target_hpm.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Function declarations for target specific operations,
 * definitions can be found in target directories
 */

/**
 * \brief Start counter for the given event
 *
 * \param event HPM event to monitor
 *
 * \return 0 on success -1 otherwise
 */
int bm_hpmcounter_start(bm_hpm_event_t event);

/**
 * \brief Pause counter for the given event
 *
 * \param event HPM event to pause counter for
 *
 * \return 0 on success -1 otherwise
 */
int bm_hpmcounter_pause(bm_hpm_event_t event);

/**
 * \brief Resume counter for the given event
 *
 * \param event HPM event to resume counter for
 *
 * \return 0 on success -1 otherwise
 */
int bm_hpmcounter_resume(bm_hpm_event_t event);

/**
 * \brief Stop counter for the given event
 *
 * \param event HPM event to stop counter for
 *
 * \return 0 on success -1 otherwise
 */
int bm_hpmcounter_stop(bm_hpm_event_t event);

/**
 * \brief Read counter for the given event
 *
 * \param event HPM event to obtain the counter value for
 *
 * \return Value of the counter or -1 on failure
 */
uint64_t bm_hpmcounter_read(bm_hpm_event_t event);

#ifdef __cplusplus
}
#endif

#endif /* BAREMETAL_HPM_H */
