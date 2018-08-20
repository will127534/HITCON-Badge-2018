/* HMAC-SHA-256 functions */
#include "hal_sha.h"
#include "hmac256.h"

void sha256( uint8_t *message, uint8_t len, uint8_t *digest)
{
   	hal_sha256_context_t context = {0};
	hal_sha256_init(&context);
	hal_sha256_append(&context, message, len);
	hal_sha256_end(&context, digest);
}

void hmacsha256_init(hmacsha256_ctx *ctx, const unsigned char *key,
	unsigned int key_size)
{
	unsigned int fill;
	unsigned int num;

	const unsigned char *key_used;
	unsigned char key_temp[HAL_SHA256_DIGEST_SIZE];
	int i;

	if (key_size == HAL_SHA256_BLOCK_SIZE) {
		key_used = key;
		num = HAL_SHA256_BLOCK_SIZE;
	}
	else {
		if (key_size > HAL_SHA256_BLOCK_SIZE) {
			num = HAL_SHA256_DIGEST_SIZE;
			sha256(key, key_size, key_temp);
			key_used = key_temp;
		}
		else { /* key_size > HAL_SHA256_BLOCK_SIZE */
			key_used = key;
			num = key_size;
		}
		fill = HAL_SHA256_BLOCK_SIZE - num;

		memset(ctx->block_ipad + num, 0x36, fill);
		memset(ctx->block_opad + num, 0x5c, fill);
	}

	for (i = 0; i < (int)num; i++) {
		ctx->block_ipad[i] = key_used[i] ^ 0x36;
		ctx->block_opad[i] = key_used[i] ^ 0x5c;
	}

	hal_sha256_init(&ctx->ctx_inside);
	hal_sha256_append(&ctx->ctx_inside, ctx->block_ipad, HAL_SHA256_BLOCK_SIZE);

	hal_sha256_init(&ctx->ctx_outside);
	hal_sha256_append(&ctx->ctx_outside, ctx->block_opad,HAL_SHA256_BLOCK_SIZE);

	/* for hmacreinit */
	memcpy(&ctx->ctx_inside_reinit, &ctx->ctx_inside,
		sizeof(hal_sha256_context_t));
	memcpy(&ctx->ctx_outside_reinit, &ctx->ctx_outside,
		sizeof(hal_sha256_context_t));
}

void hmacsha256_reinit(hmacsha256_ctx *ctx)
{
	memcpy(&ctx->ctx_inside, &ctx->ctx_inside_reinit,
		sizeof(hal_sha256_context_t));
	memcpy(&ctx->ctx_outside, &ctx->ctx_outside_reinit,
		sizeof(hal_sha256_context_t));
}

void hmacsha256_update(hmacsha256_ctx *ctx, const unsigned char *message,
	unsigned int message_len)
{
	//sha256_update(&ctx->ctx_inside, message, message_len);
	hal_sha256_append(&ctx->ctx_inside, message, message_len);
}

void hmacsha256_final(hmacsha256_ctx *ctx, unsigned char *mac,
	unsigned int mac_size)
{
	unsigned char digest_inside[HAL_SHA256_DIGEST_SIZE];
	unsigned char mac_temp[HAL_SHA256_DIGEST_SIZE];

	hal_sha256_end(&ctx->ctx_inside, digest_inside);
	hal_sha256_append(&ctx->ctx_outside, digest_inside, HAL_SHA256_DIGEST_SIZE);
	hal_sha256_end(&ctx->ctx_outside, mac_temp);
	memcpy(mac, mac_temp, mac_size);
}

void hmacsha256(const unsigned char *key, unsigned int key_size,
	const unsigned char *message, unsigned int message_len,
	unsigned char *mac, unsigned mac_size)
{
	hmacsha256_ctx ctx;

	hmacsha256_init(&ctx, key, key_size);
	hmacsha256_update(&ctx, message, message_len);
	hmacsha256_final(&ctx, mac, mac_size);
}
