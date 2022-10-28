// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/components/controls/controlgroupprofilepart.h"

namespace AMD {

class PMOverclockProfilePart final : public ControlGroupProfilePart
{
 public:
  PMOverclockProfilePart() noexcept;

 protected:
  std::unique_ptr<ControlGroupProfilePart> instance() const override;

 private:
  static bool const registered_;
};

} // namespace AMD
