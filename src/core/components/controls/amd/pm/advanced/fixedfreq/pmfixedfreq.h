// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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

  PMFixedFreq(std::unique_ptr<IPpDpmHandler> &&ppDpmSclkHandler,
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

  std::unique_ptr<IPpDpmHandler> ppDpmSclkHandler_;
  std::unique_ptr<IPpDpmHandler> ppDpmMclkHandler_;

  std::vector<std::string> sclkSourceLines_;
  std::vector<std::string> mclkSourceLines_;
};

} // namespace AMD
