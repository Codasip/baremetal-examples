/**
 * Codasip s.r.o.
 *
 * CONFIDENTIAL
 *
 * Copyright 2023 Codasip s.r.o.
 *
 * All Rights Reserved.
 * This file is part of a Codasip product. No part of this file may be use, copied,
 * modified, or distributed except in accordance with the terms contained in the
 * Codasip license agreement under which you obtained this file.
 */

#ifndef MEMORY_H_
#define MEMORY_H_

#include "config.h"

/**
 * \brief Read from a single address in memory repeatedly
 *
 * \param[in] address         Which address to read from
 */
void read_single_address(volatile width_t *address);

/**
 * \brief Read from a single address in memory repeatedly
 * Generates a fence instruction after each read instruction.
 *
 * \param[in] address         Which address to read from
 */
void read_single_address_fenced(volatile width_t *address);

/**
 * \brief Read from consecutive addresses in memory
 *
 * \param[in] address         Which address to start reading at
 */
void read_consecutive_addresses(volatile width_t *address);

/**
 * \brief Read from consecutive addresses in memory
 * Generates a fence instruction after each read instruction.
 *
 * \param[in] address         Which address to start reading at
 */
void read_consecutive_addresses_fenced(volatile width_t *address);

/**
 * \brief Write to a single address in memory repeatedly
 *
 * \param[in] address         Which address to write to
 */
void write_single_address(volatile width_t *address);

/**
 * \brief Write to a single address in memory repeatedly
 * Generates a fence instruction after each write instruction.
 *
 * \param[in] address         Which address to write to
 */
void write_single_address_fenced(volatile width_t *address);

/**
 * \brief Write to consecutive addresses in memory
 *
 * \param[in] address         Which address to start writing at
 */
void write_consecutive_addresses(volatile width_t *address);

/**
 * \brief Write to consecutive addresses in memory
 * Generates a fence instruction after each write instruction.
 *
 * \param[in] address         Which address to start writing at
 */
void write_consecutive_addresses_fenced(volatile width_t *address);

#endif /* MEMORY_H_ */
