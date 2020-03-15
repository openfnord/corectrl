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
