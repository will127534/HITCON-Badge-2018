//
// AnyLedger - Created by Bogdan Djukic on 11-04-2018. Inspired by Okada Takahiro, https://github.com/kopanitsa/web3-arduino.
// Licensed under GNU GENERAL PUBLIC LICENSE.
//

#include "Transaction.h"

#include "ByteConverter.h"
#include "CryptoHelper.h"
#include "RlpEncoder.h"

#include "secp256k1_recovery.h"
#include "secp256k1_c.h"
#include "main_impl.h"

#include <vector>
#include <cstdlib>
#include <stdarg.h>
#include <stdint.h>

#define SIGNATURE_LENGTH 64

static secp256k1_context *ctx;

Transaction::Transaction() {

    ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
}

void Transaction::setPrivateKey(const uint8_t *key) {
    privateKey = key;
}

vector<uint8_t> Transaction::getRaw(vector<uint8_t> nonce, vector<uint8_t> gasPrice, vector<uint8_t>  gasLimit,
                                    vector<uint8_t> to, vector<uint8_t> value, vector<uint8_t> data, uint32_t chainId) {
    uint8_t signature[SIGNATURE_LENGTH];
    memset(signature, 0, SIGNATURE_LENGTH);
    int recoveryId[1] = {0};

    GenerateSignature(signature, recoveryId, nonce, gasPrice, gasLimit, to, value, data, chainId);

    vector<uint8_t>param = RlpEncodeForRawTransaction(nonce,
                                                      gasPrice,
                                                      gasLimit,
                                                      to,
                                                      value,
                                                      data,
                                                      chainId,
                                                      signature,
                                                      recoveryId[0]);

    return param;
}

void Transaction::GenerateSignature(uint8_t* signature,
                                 int* recoveryId,
                                 vector<uint8_t> nonce, vector<uint8_t> gasPrice, vector<uint8_t>  gasLimit,
                                 vector<uint8_t> to, vector<uint8_t> value, vector<uint8_t> data, uint32_t chainId) {
    // Encode
    vector<uint8_t> rlpEncoded = RlpEncode(nonce, gasPrice, gasLimit, to, value, data, chainId);
    // Hash
    vector<uint8_t> transactionHash = CryptoHelper::keccak256(&rlpEncoded[0], rlpEncoded.size());
    // Sign
    Sign(&transactionHash[0], signature, recoveryId);
}


vector<uint8_t> Transaction::RlpEncode(vector<uint8_t> nonce, vector<uint8_t> gasPrice, vector<uint8_t>  gasLimit,
                                    vector<uint8_t> to, vector<uint8_t> value, vector<uint8_t> data, uint32_t chainIdStr) {

    vector<uint8_t> outputNonce = RlpEncoder::encodeItemWithVector(nonce);
    vector<uint8_t> outputGasPrice = RlpEncoder::encodeItemWithVector(gasPrice);
    vector<uint8_t> outputGasLimit = RlpEncoder::encodeItemWithVector(gasLimit);
    vector<uint8_t> outputTo = RlpEncoder::encodeItemWithVector(to);
    vector<uint8_t> outputValue = RlpEncoder::encodeItemWithVector(value);
    vector<uint8_t> outputData = RlpEncoder::encodeItemWithVector(data);

    vector<uint8_t> encoded = RlpEncoder::encodeWholeHeaderWithVector(
      outputNonce.size() +
      outputGasPrice.size() +
      outputGasLimit.size() +
      outputTo.size() +
      outputValue.size() +
      outputData.size());

    encoded.insert(encoded.end(), outputNonce.begin(), outputNonce.end());
    encoded.insert(encoded.end(), outputGasPrice.begin(), outputGasPrice.end());
    encoded.insert(encoded.end(), outputGasLimit.begin(), outputGasLimit.end());
    encoded.insert(encoded.end(), outputTo.begin(), outputTo.end());
    encoded.insert(encoded.end(), outputValue.begin(), outputValue.end());
    encoded.insert(encoded.end(), outputData.begin(), outputData.end());

    return encoded;
}

void Transaction::Sign(uint8_t* hash, uint8_t* sig, int* recid) {
  secp256k1_nonce_function noncefn = secp256k1_nonce_function_rfc6979;
  void* data_ = NULL;

  secp256k1_ecdsa_recoverable_signature signature;
  memset(&signature, 0, sizeof(signature));

  if (secp256k1_ecdsa_sign_recoverable(ctx, &signature, hash,  privateKey, noncefn, data_) == 0) {
      return;
  }

  secp256k1_ecdsa_recoverable_signature_serialize_compact(ctx, &sig[0], &recid[0], &signature);
}

vector<uint8_t> Transaction::RlpEncodeForRawTransaction(
                             vector<uint8_t> nonce, vector<uint8_t> gasPrice, vector<uint8_t>  gasLimit,
                             vector<uint8_t> to, vector<uint8_t> value, vector<uint8_t> data, uint32_t chainIdStr, uint8_t* sig, uint8_t recoveryId) {

    vector<uint8_t> signature;
    for (int i = 0; i < SIGNATURE_LENGTH; i++) {
        signature.push_back(sig[i]);
    }

    vector<uint8_t> outputNonce = RlpEncoder::encodeItemWithVector(nonce);
    vector<uint8_t> outputGasPrice = RlpEncoder::encodeItemWithVector(gasPrice);
    vector<uint8_t> outputGasLimit = RlpEncoder::encodeItemWithVector(gasLimit);
    vector<uint8_t> outputTo = RlpEncoder::encodeItemWithVector(to);
    vector<uint8_t> outputValue = RlpEncoder::encodeItemWithVector(value);
    vector<uint8_t> outputData = RlpEncoder::encodeItemWithVector(data);

    vector<uint8_t> R;
    R.insert(R.end(), signature.begin(), signature.begin() + (SIGNATURE_LENGTH / 2));
    vector<uint8_t> S;
    S.insert(S.end(), signature.begin() + (SIGNATURE_LENGTH / 2), signature.end());
    vector<uint8_t> V;
    V.push_back((uint8_t)(recoveryId + 27));

    vector<uint8_t> outputR = RlpEncoder::encodeItemWithVector(R);
    vector<uint8_t> outputS = RlpEncoder::encodeItemWithVector(S);
    vector<uint8_t> outputV = RlpEncoder::encodeItemWithVector(V);

    vector<uint8_t> encoded = RlpEncoder::encodeWholeHeaderWithVector(
                                        outputNonce.size() +
                                        outputGasPrice.size() +
                                        outputGasLimit.size() +
                                        outputTo.size() +
                                        outputValue.size() +
                                        outputData.size() +
                                        outputV.size() +
                                        outputR.size() +
                                        outputS.size());

    encoded.insert(encoded.end(), outputNonce.begin(), outputNonce.end());
    encoded.insert(encoded.end(), outputGasPrice.begin(), outputGasPrice.end());
    encoded.insert(encoded.end(), outputGasLimit.begin(), outputGasLimit.end());
    encoded.insert(encoded.end(), outputTo.begin(), outputTo.end());
    encoded.insert(encoded.end(), outputValue.begin(), outputValue.end());
    encoded.insert(encoded.end(), outputData.begin(), outputData.end());

    encoded.insert(encoded.end(), outputV.begin(), outputV.end());
    encoded.insert(encoded.end(), outputR.begin(), outputR.end());
    encoded.insert(encoded.end(), outputS.begin(), outputS.end());

    return encoded;
}
