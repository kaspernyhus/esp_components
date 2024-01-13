/**
 * @file esp_rsa_gen.c
 * @author Kasper Nyhus
 * @brief
 * @version 0.1
 * @date 2023-06-25
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <string.h>

#include "esp_rsa.h"
#include "esp_log.h"

#include "mbedtls/rsa.h"
#include <mbedtls/entropy.h>
#include "mbedtls/pk.h"

static const char* TAG = "ESP_RSA";

esp_err_t esp_rsa_init(esp_rsa_t* rsa, size_t key_size, int exponent)
{
    esp_err_t ret = ESP_OK;
    int err;

    mbedtls_entropy_context entropy;

    char *personalization = "rsa generator string";

    mbedtls_ctr_drbg_init(&rsa->ctr_drbg);
    mbedtls_entropy_init(&entropy);

    mbedtls_pk_init(&rsa->pk);

    err = mbedtls_ctr_drbg_seed(&rsa->ctr_drbg, mbedtls_entropy_func, &entropy, (const unsigned char *)personalization, strlen(personalization));
    if (err != 0) {
        ESP_LOGE(TAG, "mbedtls_ctr_drbg_seed failed: %d", err);
        ret = ESP_FAIL;
    }

    err = mbedtls_pk_setup(&rsa->pk, mbedtls_pk_info_from_type(MBEDTLS_PK_RSA));
    if (err != 0) {
        ESP_LOGE(TAG, "mbedtls_pk_setup failed: %d", err);
        ret = ESP_FAIL;
    }

    if (ret == ESP_OK) {
        rsa->key_size = key_size;
        rsa->exponent = exponent;
    }

    mbedtls_entropy_free( &entropy );

    return ret;
}

void esp_rsa_deinit(esp_rsa_t* rsa)
{
    mbedtls_pk_free(&rsa->pk);
    mbedtls_ctr_drbg_free(&rsa->ctr_drbg);
}

esp_err_t esp_rsa_gen_keys(esp_rsa_t* rsa)
{
    esp_err_t ret = ESP_OK;
    int err;

    err = mbedtls_rsa_gen_key(mbedtls_pk_rsa(rsa->pk), mbedtls_ctr_drbg_random, &rsa->ctr_drbg, rsa->key_size, rsa->exponent);
    if (err != 0) {
        ESP_LOGE(TAG, "mbedtls_rsa_gen_key failed: %d", err);
        ret = ESP_FAIL;
    }

    err = mbedtls_rsa_check_pubkey(mbedtls_pk_rsa(rsa->pk));
    if (err != 0) {
        ESP_LOGE(TAG, "mbedtls_rsa_check_pubkey failed: %d", err);
        ret = ESP_FAIL;
    }

    err = mbedtls_rsa_check_privkey(mbedtls_pk_rsa(rsa->pk));
    if (err != 0) {
        ESP_LOGE(TAG, "mbedtls_rsa_check_privkey failed: %d", err);
        ret = ESP_FAIL;
    }
    return ret;
}

esp_err_t esp_rsa_get_pubkey(esp_rsa_t* rsa, unsigned char* buffer, size_t size)
{
    esp_err_t ret = ESP_OK;
    int err;

    err = mbedtls_pk_write_pubkey_pem(&rsa->pk, buffer, size);
    if (err != 0) {
        ESP_LOGE(TAG, "mbedtls_pk_write_pubkey_pem failed: %d", err);
        ret = ESP_FAIL;
    }
    return ret;
}

esp_err_t esp_rsa_get_private_key(esp_rsa_t* rsa, unsigned char* buffer, size_t size)
{
    esp_err_t ret = ESP_OK;
    int err;

    err = mbedtls_pk_write_key_pem(&rsa->pk, buffer, size);
    if (err != 0) {
        ESP_LOGE(TAG, "mbedtls_pk_write_key_pem failed: %d", err);
        ret = ESP_FAIL;
    }
    return ret;
}

esp_err_t esp_rsa_encrypt(esp_rsa_t* rsa, void* in, size_t in_length, void* out)
{
    esp_err_t ret = ESP_OK;
    int err = mbedtls_rsa_pkcs1_encrypt(mbedtls_pk_rsa(rsa->pk), mbedtls_ctr_drbg_random, &rsa->ctr_drbg, MBEDTLS_RSA_PUBLIC, in_length, (unsigned char*)in, (unsigned char*)out);
    if (err != 0) {
        ESP_LOGE(TAG, "mbedtls_rsa_pkcs1_encrypt failed: 0x%X", err);
        ret = ESP_FAIL;
    }
    return ret;
}

esp_err_t esp_rsa_decrypt(esp_rsa_t* rsa, void* in, void* out, size_t out_size)
{
    esp_err_t ret = ESP_OK;
    size_t out_length;

    int err = mbedtls_rsa_pkcs1_decrypt(mbedtls_pk_rsa(rsa->pk), mbedtls_ctr_drbg_random, &rsa->ctr_drbg, MBEDTLS_RSA_PRIVATE, &out_length, (unsigned char*)in, (unsigned char*)out, out_size);

    if (err != 0) {
        ESP_LOGE(TAG, "mbedtls_rsa_pkcs1_decrypt failed: 0x%X", err);
        ret = ESP_FAIL;
    }
    return ret;
}
