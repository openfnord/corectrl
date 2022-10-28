// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "isysmodel.h"
#include <memory>
#include <string>
#include <vector>

class IProfilePart;
class IProfilePartView;

class IProfileView : public ISysModel::Importer
{
 public:
  class View
  {
   public:
    virtual std::string const &name() const = 0;
    virtual std::vector<std::shared_ptr<IProfilePart>> const &parts() const = 0;

    virtual ~View() = default;
  };

  virtual std::string const &name() const = 0;
  virtual std::vector<std::unique_ptr<IProfilePartView>> const &parts() const = 0;

  virtual ~IProfileView() = default;
};
