// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "icryptolayer.h"
#include <botan/pk_keys.h>
#include <memory>

class CryptoLayer final : public ICryptoLayer
{
 public:
  void init() override;

  QByteArray publicKey() override;
  void usePublicKey(QByteArray const &publicKey) override;

  bool verify(QByteArray const &data, QByteArray const &signature) override;
  QByteArray signature(QByteArray const &data) override;

 private:
  std::unique_ptr<Botan::Private_Key> privateKey_;
  std::unique_ptr<Botan::Public_Key> opPublicKey_;
};
