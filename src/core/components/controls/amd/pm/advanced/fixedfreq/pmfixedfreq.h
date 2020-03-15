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
#include "units/units.h"
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

template<typename...>
class IDataSource;

namespace AMD {

class IPpDpmHandler;

class PMFixedFreq : public Control
{
 public:
  static constexpr std::string_view ItemID{"AMD_PM_FIXED_FREQ"};

  class Importer : public IControl::Importer
  {
   public:
    virtual unsigned int providePMFixedFreqSclkIndex() const = 0;
    virtual unsigned int providePMFixedFreqMclkIndex() const = 0;
  };

  class Exporter : public IControl::Exporter
  {
   public:
    virtual void takePMFixedFreqSclkIndex(unsigned int index) = 0;
    virtual void takePMFixedFreqMclkIndex(unsigned int index) = 0;
    virtual void takePMFixedFreqSclkStates(
        std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
            &states) = 0;
    virtual void takePMFixedFreqMclkStates(
        std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
            &states) = 0;
  };

  PMFixedFreq(std::unique_ptr<IDataSource<std::string>> &&perfLevelDataSource,
              std::unique_ptr<IPpDpmHandler> &&ppDpmSclkHandler,
              std::unique_ptr<IPpDpmHandler> &&ppDpmMclkHandler) noexcept;

  void preInit(ICommandQueue &ctlCmds) final override;
  void postInit(ICommandQueue &ctlCmds) final override;
  void init() final override;

  std::string const &ID() const final override;

 protected:
  void importControl(IControl::Importer &i) final override;
  void exportControl(IControl::Exporter &e) const final override;

  void cleanControl(ICommandQueue &ctlCmds) final override;
  void syncControl(ICommandQueue &ctlCmds) final override;

 private:
  std::string const id_;

  std::unique_ptr<IDataSource<std::string>> const perfLevelDataSource_;
  std::unique_ptr<IPpDpmHandler> ppDpmSclkHandler_;
  std::unique_ptr<IPpDpmHandler> ppDpmMclkHandler_;

  std::string dataSourceEntry_;
  std::vector<std::string> sclkSourceLines_;
  std::vector<std::string> mclkSourceLines_;
};

} // namespace AMD
