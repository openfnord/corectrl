// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "iprofilepartview.h"
#include <memory>
#include <string>

class IProfilePart;

class ProfilePartView final : public IProfilePartView
{
 public:
  ProfilePartView(std::string const &profile,
                  std::shared_ptr<IProfilePart> part) noexcept;

  std::string const &profile() const override;
  std::shared_ptr<IProfilePart> const &part() const override;

 private:
  std::string profile_;
  std::shared_ptr<IProfilePart> part_;
};
