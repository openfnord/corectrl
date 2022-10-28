// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/components/controls/controlmodeprofilepart.h"

namespace AMD {

class FanModeProfilePart final : public ControlModeProfilePart
{
 public:
  FanModeProfilePart() noexcept;

 protected:
  std::unique_ptr<ControlModeProfilePart> instance() const override;

 private:
  static bool const registered_;
};

} // namespace AMD
