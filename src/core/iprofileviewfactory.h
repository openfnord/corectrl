// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include <functional>
#include <memory>
#include <optional>

class Exportable;
class IProfileView;

class IProfileViewFactory
{
 public:
  virtual std::unique_ptr<IProfileView>
  build(Exportable const &profile,
        std::optional<std::reference_wrapper<IProfileView>> base =
            std::nullopt) const = 0;

  virtual ~IProfileViewFactory() = default;
};
