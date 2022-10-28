// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/components/controls/control.h"
#include "units/units.h"
#include <memory>
#include <string>
#include <string_view>

template<typename...>
class IDataSource;

namespace AMD {

class FanFixed : public Control
{
 public:
  static constexpr std::string_view ItemID{"AMD_FAN_FIXED"};

  class Importer : public IControl::Importer
  {
   public:
    virtual units::concentration::percent_t provideFanFixedValue() const = 0;
    virtual bool provideFanFixedFanStop() const = 0;
    virtual units::concentration::percent_t
    provideFanFixedFanStartValue() const = 0;
  };

  class Exporter : public IControl::Exporter
  {
   public:
    virtual void takeFanFixedValue(units::concentration::percent_t value) = 0;
    virtual void takeFanFixedFanStop(bool enabled) = 0;
    virtual void
    takeFanFixedFanStartValue(units::concentration::percent_t value) = 0;
  };

  FanFixed(std::unique_ptr<IDataSource<unsigned int>> &&pwmEnableDataSource,
           std::unique_ptr<IDataSource<unsigned int>> &&pwmDataSource) noexcept;

  void preInit(ICommandQueue &ctlCmds) final override;
  void postInit(ICommandQueue &ctlCmds) final override;
  void init() final override;

  std::string const &ID() const final override;

 protected:
  void importControl(IControl::Importer &i) final override;
  void exportControl(IControl::Exporter &e) const final override;

  void cleanControl(ICommandQueue &ctlCmds) final override;
  void syncControl(ICommandQueue &ctlCmds) final override;

  unsigned int value() const;
  void value(unsigned int value);

  bool fanStop() const;
  void fanStop(bool active);

  unsigned int fanStartValue() const;
  void fanStartValue(unsigned int value);

 private:
  std::string const id_;

  std::unique_ptr<IDataSource<unsigned int>> const pwmEnableDataSource_;
  std::unique_ptr<IDataSource<unsigned int>> const pwmDataSource_;

  unsigned int pwmEnable_;
  unsigned int pwm_;

  unsigned int value_;
  unsigned int lastValue_;

  bool fanStop_;
  unsigned int fanStartValue_;
};

} // namespace AMD
