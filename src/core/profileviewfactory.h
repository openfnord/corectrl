// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "iprofileviewfactory.h"
#include <functional>
#include <memory>
#include <optional>

class Exportable;
class IProfileView;

class ProfileViewFactory final : public IProfileViewFactory
{
 public:
  std::unique_ptr<IProfileView>
  build(Exportable const &profile,
        std::optional<std::reference_wrapper<IProfileView>> base) const;
};
