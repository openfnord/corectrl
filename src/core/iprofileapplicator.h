// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

class IProfileView;

class IProfileApplicator
{
 public:
  virtual void apply(IProfileView &profileView) = 0;

  virtual ~IProfileApplicator() = default;
};
