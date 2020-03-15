//
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// Distributed under the GPL version 3 or any later version.
//
#pragma once

#include "core/components/controls/control.h"
#include <string>
#include <string_view>
#include <vector>

namespace AMD {

class PMFixed : public Control
{
 public:
  static constexpr std::string_view ItemID{"AMD_PM_FIXED"};

  class Importer : public IControl::Importer
  {
   public:
    virtual std::string const &providePMFixedMode() const = 0;
  };

  class Exporter : public IControl::Exporter
  {
   public:
    virtual void takePMFixedMode(std::string const &mode) = 0;
    virtual void takePMFixedModes(std::vector<std::string> const &modes) = 0;
  };

  PMFixed(std::string_view mode) noexcept;

  void preInit(ICommandQueue &ctlCmds) final override;
  void postInit(ICommandQueue &ctlCmds) final override;
  void init() final override;

  std::string const &ID() const final override;

 protected:
  void importControl(IControl::Importer &i) final override;
  void exportControl(IControl::Exporter &e) const final override;

  std::string const &mode() const;
  void mode(std::string const &mode);

  virtual std::vector<std::string> const &modes() const = 0;

 private:
  std::string const id_;
  std::string mode_;
};

} // namespace AMD
