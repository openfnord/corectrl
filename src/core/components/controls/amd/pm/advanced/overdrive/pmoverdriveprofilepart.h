// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/components/controls/controlgroupprofilepart.h"

namespace AMD {

class PMOverdriveProfilePart final : public ControlGroupProfilePart
{
 public:
  PMOverdriveProfilePart() noexcept;

 protected:
  std::unique_ptr<ControlGroupProfilePart> instance() const override;

 private:
  static bool const registered_;
};

} // namespace AMD
