/**
 * @file esp_rsa_gen.h
 * @author Kasper Nyhus
 * @brief
 * @version 0.1
 * @date 2023-06-25
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include "esp_err.h"

#include "mbedtls/ctr_drbg.h"
#include "mbedtls/pk.h"

typedef struct {
    mbedtls_pk_context pk;
    mbedtls_ctr_drbg_context ctr_drbg;
    int key_size;
    int exponent;
} esp_rsa_t;

/**
 * @brief Initialize RSA object
 *
 * @param rsa instance
 * @param key_size RSA key size
 * @param exponent eg. 0x10001
 * @return esp_err_t ESP_OK on success
 */
esp_err_t esp_rsa_init(esp_rsa_t* rsa, size_t key_size, int exponent);

/**
 * @brief
 *
 * @param rsa
 */
void esp_rsa_deinit(esp_rsa_t* rsa);

/**
 * @brief Generate an RSA key pair
 *
 * @param rsa instance
 * @return esp_err_t ESP_OK on success
 */
esp_err_t esp_rsa_gen_keys(esp_rsa_t* rsa);

/**
 * @brief Export the public key
 *
 * @param rsa instance
 * @param buffer to write key to (PEM format)
 * @param size of buffer
 * @return esp_err_t ESP_OK on success
 */
esp_err_t esp_rsa_get_pubkey(esp_rsa_t* rsa, unsigned char* buffer, size_t size);

/**
 * @brief Export private key
 *
 * @param rsa instance
 * @param buffer  to write key to (PEM format)
 * @param size of buffer
 * @return esp_err_t ESP_OK on success
 */
esp_err_t esp_rsa_get_private_key(esp_rsa_t* rsa, unsigned char* buffer, size_t size);

/**
 * @brief Encrypt a message using the public key
 *
 * @param rsa instance
 * @param in message to encrypt
 * @param in_length length of message in bytes
 * @param out cipher text output
 * @return esp_err_t ESP_OK on success
 */
esp_err_t esp_rsa_encrypt(esp_rsa_t* rsa, void* in, size_t in_length, void* out);

/**
 * @brief Decrypt a message using the private key
 *
 * @param rsa instance
 * @param in cipher text input
 * @param out message output
 * @param out_size size of output buffer to hold decoded message
 * @return esp_err_t ESP_OK on success
 */
esp_err_t esp_rsa_decrypt(esp_rsa_t* rsa, void* in, void* out, size_t out_size);
