//
// AnyLedger - Created by Bogdan Djukic on 11-04-2018. Inspired by Okada Takahiro, https://github.com/kopanitsa/web3-arduino.
// Licensed under GNU GENERAL PUBLIC LICENSE.
//

#include "Transaction.h"

#include "ByteConverter.h"
#include "CryptoHelper.h"
#include "RlpEncoder.h"

#include <vector>
#include <cstdlib>
#include <stdarg.h>
#include <stdint.h>

#define SIGNATURE_LENGTH 64

Transaction::Transaction() {

}



vector<uint8_t> Transaction::GenerateSignature(transaction_struct trx) {
    // Encode
    vector<uint8_t> rlpEncoded = RlpEncode(trx);
    // Hash
    vector<uint8_t> transactionHash = CryptoHelper::keccak256(&rlpEncoded[0], rlpEncoded.size());
    // Sign
    return transactionHash;
}


vector<uint8_t> Transaction::RlpEncode(transaction_struct trx) {

    vector<uint8_t> outputNonce = RlpEncoder::encodeItemWithVector(trx.nonce);
    vector<uint8_t> outputGasPrice = RlpEncoder::encodeItemWithVector(trx.gasPrice);
    vector<uint8_t> outputGasLimit = RlpEncoder::encodeItemWithVector(trx.gasLimit);
    vector<uint8_t> outputTo = RlpEncoder::encodeItemWithVector(trx.to);
    vector<uint8_t> outputValue = RlpEncoder::encodeItemWithVector(trx.value);
    vector<uint8_t> outputData = RlpEncoder::encodeItemWithVector(trx.data);

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

vector<uint8_t> Transaction::RlpEncodeForRawTransaction(transaction_struct trx, uint8_t* sig, uint8_t recoveryId) {

    vector<uint8_t> signature;
    for (int i = 0; i < SIGNATURE_LENGTH; i++) {
        signature.push_back(sig[i]);
    }

    vector<uint8_t> outputNonce = RlpEncoder::encodeItemWithVector(trx.nonce);
    vector<uint8_t> outputGasPrice = RlpEncoder::encodeItemWithVector(trx.gasPrice);
    vector<uint8_t> outputGasLimit = RlpEncoder::encodeItemWithVector(trx.gasLimit);
    vector<uint8_t> outputTo = RlpEncoder::encodeItemWithVector(trx.to);
    vector<uint8_t> outputValue = RlpEncoder::encodeItemWithVector(trx.value);
    vector<uint8_t> outputData = RlpEncoder::encodeItemWithVector(trx.data);

    vector<uint8_t> R;
    R.insert(R.end(), signature.begin(), signature.begin() + (SIGNATURE_LENGTH / 2));
    vector<uint8_t> S;
    S.insert(S.end(), signature.begin() + (SIGNATURE_LENGTH / 2), signature.end());
    vector<uint8_t> V;
  
    //signature[64] += 27;
    V.push_back((uint8_t)(recoveryId + 27));
    //V.push_back(signature[64]);

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

