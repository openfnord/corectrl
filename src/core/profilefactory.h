// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "iprofilepartprovider.h"
#include <memory>

class ISysModel;
class IProfile;

class ProfileFactory final
{
 public:
  ProfileFactory(
      std::unique_ptr<IProfilePartProvider> &&profilePartProvider) noexcept;

  std::unique_ptr<IProfile> build(ISysModel const &sysModel) const;

 private:
  std::unique_ptr<IProfilePartProvider> profilePartProvider_;
};
