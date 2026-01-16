/* Copyright 2024 Codasip s.r.o.         */
/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef BAREMETAL_AEAD_H
#define BAREMETAL_AEAD_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct bm_aead_regs {
    const volatile uint32_t FEATURES; ///< (@ 0x0000) Features register
    uint8_t                 _reserved1[0xfc];
    const volatile uint32_t STATUS;      ///< (@ 0x0100) Status register
    const volatile uint32_t FIFO_STATUS; ///< (@ 0x0104) FIFO status register
    uint8_t                 _reserved2[0xf8];
    volatile uint32_t       CONFIG;  ///< (@ 0x0200) Configuration register
    volatile uint32_t       COMMAND; ///< (@ 0x0204) Command register
    uint8_t                 _reserved3[0x2f8];
    volatile uint32_t       PAYLOAD_SIZE; ///< (@ 0x0500) Payload size register
    uint8_t                 _reserved4[0x0fc];
    volatile uint32_t       AD_SIZE; ///< (@ 0x0600) Additional data size register
    uint8_t                 _reserved5[0x0fc];
    volatile uint32_t       KEY[0x40];      ///< (@ 0x0700) Key registers
    volatile uint32_t       NONCE[0x40];    ///< (@ 0x0800) Nonce registers
    volatile uint32_t       BLOCK_IN[0x40]; ///< (@ 0x0900) Block in registers
    const volatile uint32_t DATA_OUT[0x40]; ///< (@ 0x0a00) Data out registers
    const volatile uint32_t TAG[0x40];      ///< (@ 0x0b00) Tag registers
};

/** \brief Structure describing AEAD peripheral registers */
typedef struct bm_aead_regs bm_aead_regs_t;

/** \brief Block size options */
typedef enum {
    BM_AEAD_BLOCK_SIZE_64  = 0x0,
    BM_AEAD_BLOCK_SIZE_128 = 0x1,
} bm_aead_block_size_t;

/** \brief Key size options */
typedef enum {
    BM_AEAD_KEY_SIZE_128 = 0x0,
    BM_AEAD_KEY_SIZE_160 = 0x1,
    BM_AEAD_KEY_SIZE_192 = 0x2,
    BM_AEAD_KEY_SIZE_256 = 0x3,
} bm_aead_key_size_t;

/** \brief Nonce size options */
typedef enum {
    BM_AEAD_NONCE_SIZE_16  = 0x0,
    BM_AEAD_NONCE_SIZE_24  = 0x1,
    BM_AEAD_NONCE_SIZE_32  = 0x2,
    BM_AEAD_NONCE_SIZE_48  = 0x3,
    BM_AEAD_NONCE_SIZE_64  = 0x4,
    BM_AEAD_NONCE_SIZE_96  = 0x5,
    BM_AEAD_NONCE_SIZE_128 = 0x6,
} bm_aead_nonce_size_t;

/** \brief Tag size options */
typedef enum {
    BM_AEAD_TAG_SIZE_32  = 0x0,
    BM_AEAD_TAG_SIZE_48  = 0x1,
    BM_AEAD_TAG_SIZE_64  = 0x2,
    BM_AEAD_TAG_SIZE_80  = 0x3,
    BM_AEAD_TAG_SIZE_96  = 0x4,
    BM_AEAD_TAG_SIZE_112 = 0x5,
    BM_AEAD_TAG_SIZE_128 = 0x6,
} bm_aead_tag_size_t;

/** \brief Structure holding data neccessary to service a AEAD device. */
typedef struct {
    bm_aead_regs_t      *regs;       ///< Pointer to the peripheral registers
    bm_aead_block_size_t block_size; ///< Block size
    bm_aead_key_size_t   key_size;   ///< Key size
    bm_aead_nonce_size_t nonce_size; ///< Nonce size
    bm_aead_tag_size_t   tag_size;   ///< Tag size
} bm_aead_t;

/** \brief Structure holding AEAD command parameters */
typedef struct {
    bool     decrypt; // True for decryption, False for encryption
    uint8_t *key;
    uint8_t *nonce;
    uint8_t *payload;
    uint32_t payload_size;
    uint8_t *additional_data;
    uint32_t ad_size;
    uint8_t *data_out;
    uint8_t *tag;
} bm_aead_command_t;

/**
 * \brief Function to initialize the AEAD peripheral
 *
 * \param aead Pointer to the AEAD driver structure linked to the peripheral to initialize.
 */
void bm_aead_init(bm_aead_t *aead);

/**
 * \brief Function to run a single command in AEAD core
 *
 * \param aead Pointer to the AEAD driver structure
 * \param command Command to run
 */
void bm_aead_run(bm_aead_t *aead, bm_aead_command_t *command);

#ifdef __cplusplus
}
#endif

#endif /* BAREMETAL_AEAD_H */
