// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/info/ihwidtranslator.h"

class HWIDTranslatorStub : public IHWIDTranslator
{
 public:
  std::string vendor(std::string const &) const
  {
    return "";
  }

  std::string device(std::string const &, std::string const &) const
  {
    return "";
  }

  std::string subdevice(std::string const &, std::string const &,
                        std::string const &, std::string const &) const
  {
    return "";
  }
};
