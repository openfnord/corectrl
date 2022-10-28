// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "cryptolayer.h"

#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include <botan/base64.h>
#include <botan/data_src.h>
#include <botan/ed25519.h>
#include <botan/pubkey.h>
#include <botan/system_rng.h>
#include <botan/x509_key.h>
#include <cstddef>
#include <cstdint>
#include <exception>

void CryptoLayer::init()
{
  privateKey_ = std::make_unique<Botan::Ed25519_PrivateKey>(Botan::system_rng());
}

QByteArray CryptoLayer::publicKey()
{
  auto key = Botan::X509::PEM_encode(*privateKey_);
  return {key.c_str()};
}

void CryptoLayer::usePublicKey(QByteArray const &publicKey)
{
  auto keyStr = publicKey.toStdString();
  Botan::DataSource_Memory pubKey(keyStr);
  opPublicKey_ =
      std::unique_ptr<Botan::Public_Key>(Botan::X509::load_key(pubKey));
}

bool CryptoLayer::verify(QByteArray const &data, QByteArray const &signature)
{
  try {
    Botan::PK_Verifier verifier(*opPublicKey_, "SHA-512");
    auto decodedSignature = Botan::base64_decode(signature.toStdString());
    return verifier.verify_message(
        reinterpret_cast<uint8_t const *>(data.data()),
        static_cast<size_t>(data.size()), decodedSignature.data(),
        decodedSignature.size());
  }
  catch (std::exception const &e) {
    LOG(ERROR) << e.what();
  }
  return false;
}

QByteArray CryptoLayer::signature(QByteArray const &data)
{
  try {
    Botan::PK_Signer signer(*privateKey_, Botan::system_rng(), "SHA-512");
    auto signature = signer.sign_message(
        reinterpret_cast<uint8_t const *>(data.data()),
        static_cast<size_t>(data.size()), Botan::system_rng());
    auto encodedSignature = Botan::base64_encode(signature);
    return {encodedSignature.c_str()};
  }
  catch (std::exception const &e) {
    LOG(ERROR) << e.what();
  }
  return {};
}
