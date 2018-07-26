/*
* HMAC-SHA-224/256/384/512 implementation
* Last update: 06/15/2005
* Issue date:  06/15/2005
*
* Copyright (C) 2005 Olivier Gay <olivier.gay@a3.epfl.ch>
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
* 3. Neither the name of the project nor the names of its contributors
*    may be used to endorse or promote products derived from this software
*    without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
* SUCH DAMAGE.
*/

#ifndef HMAC_SHA2_H
#define HMAC_SHA2_H

#include "hal_sha.h"

#ifdef __cplusplus
extern "C" {
#endif
	typedef struct {
		hal_sha256_context_t ctx_inside;
		hal_sha256_context_t ctx_outside;

		/* for hmacreinit */
		hal_sha256_context_t ctx_inside_reinit;
		hal_sha256_context_t ctx_outside_reinit;

		unsigned char block_ipad[HAL_SHA256_BLOCK_SIZE];
		unsigned char block_opad[HAL_SHA256_BLOCK_SIZE];
	} hmacsha256_ctx;

	void hmacsha256_init(hmacsha256_ctx *ctx, const unsigned char *key,
		unsigned int key_size);
	void hmacsha256_reinit(hmacsha256_ctx *ctx);
	void hmacsha256_update(hmacsha256_ctx *ctx, const unsigned char *message,
		unsigned int message_len);
	void hmacsha256_final(hmacsha256_ctx *ctx, unsigned char *mac,
		unsigned int mac_size);
	void hmacsha256(const unsigned char *key, unsigned int key_size,
		const unsigned char *message, unsigned int message_len,
		unsigned char *mac, unsigned mac_size);
#ifdef __cplusplus
}
#endif

#endif /* !HMAC_SHA2_H */