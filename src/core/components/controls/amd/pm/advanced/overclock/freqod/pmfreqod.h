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

class PMFreqOd : public Control
{
 public:
  static constexpr std::string_view ItemID{"AMD_PM_FREQ_OD"};

  class Importer : public IControl::Importer
  {
   public:
    virtual unsigned int providePMFreqOdSclkOd() const = 0;
    virtual unsigned int providePMFreqOdMclkOd() const = 0;
  };

  class Exporter : public IControl::Exporter
  {
   public:
    virtual void takePMFreqOdSclkOd(unsigned int value) = 0;
    virtual void takePMFreqOdMclkOd(unsigned int value) = 0;
    virtual void takePMFreqOdBaseSclk(units::frequency::megahertz_t value) = 0;
    virtual void takePMFreqOdBaseMclk(units::frequency::megahertz_t value) = 0;
  };

  PMFreqOd(std::unique_ptr<IDataSource<unsigned int>> &&sclkOdDataSource,
           std::unique_ptr<IDataSource<unsigned int>> &&mclkOdDataSource,
           std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
               &sclkStates,
           std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
               &mclkStates) noexcept;

  void preInit(ICommandQueue &ctlCmds) final override;
  void postInit(ICommandQueue &ctlCmds) final override;
  void init() final override;
  std::string const &ID() const final override;

 protected:
  void importControl(IControl::Importer &i) final override;
  void exportControl(IControl::Exporter &e) const final override;

  void cleanControl(ICommandQueue &ctlCmds) final override;
  void syncControl(ICommandQueue &ctlCmds) final override;

  unsigned int sclkOd() const;
  void sclkOd(unsigned int value);

  unsigned int mclkOd() const;
  void mclkOd(unsigned int value);

  units::frequency::megahertz_t baseSclk() const;
  units::frequency::megahertz_t baseMclk() const;

 private:
  std::string const id_;

  std::unique_ptr<IDataSource<unsigned int>> const sclkOdDataSource_;
  std::unique_ptr<IDataSource<unsigned int>> const mclkOdDataSource_;

  units::frequency::megahertz_t baseSclk_;
  units::frequency::megahertz_t baseMclk_;

  unsigned int sclkOd_;
  unsigned int mclkOd_;

  unsigned int sclkOdDataSourceEntry_;
  unsigned int mclkOdDataSourceEntry_;
};

} // namespace AMD
