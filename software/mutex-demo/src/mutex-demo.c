/* Copyright 2023 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#include <baremetal/common.h>
#include <baremetal/mp.h>
#include <baremetal/mutex.h>
#include <baremetal/time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define DELAY_MS 5000

static bm_mutex_t mutex;

/**
 * \brief Helper function introducing random delay
 */
static void random_delay(unsigned int *seedp, unsigned max)
{
    int rand_number = rand_r(seedp);

    bm_delay_ms(rand_number % max);
}

/**
 * \brief Function executed from all harts, competing for a mutex
 */
void hart_job(bm_hart_func_arg_t arg UNUSED)
{
    unsigned num = bm_get_hartid();

    for (int i = 0; i < 3; ++i)
    {
        // Introduce a random delay, different for each hart
        random_delay(&num, DELAY_MS);
        unsigned tries = 0;
        do
        {
            ++tries;
        } while (bm_mutex_trylock(&mutex));

        // Print out in critical section
        printf("Mutex claimed from hart %u!\n", bm_get_hartid());
        printf("(took %u attempts to get the lock)\n\n", tries);

        // Introduce delay once more
        random_delay(&num, DELAY_MS);

        bm_mutex_unlock(&mutex);
    }
}

int main(void)
{
    puts("Welcome to the mutex demo!\n");

    bm_mutex_init(&mutex);

    bm_hart_execute_all((bm_hart_func_ptr_t)hart_job);

    puts("Bye.");
    return EXIT_SUCCESS;
}
