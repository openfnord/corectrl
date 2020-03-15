//
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// Distributed under the GPL version 3 or any later version.
//
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
