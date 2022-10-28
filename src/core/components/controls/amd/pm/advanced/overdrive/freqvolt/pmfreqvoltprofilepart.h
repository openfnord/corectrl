// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/profilepart.h"
#include "pmfreqvolt.h"
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace AMD {

class PMFreqVoltProfilePart final
: public ProfilePart
, public PMFreqVolt::Importer
{
 public:
  class Importer : public IProfilePart::Importer
  {
   public:
    virtual std::string const &providePMFreqVoltVoltMode() const = 0;
    virtual std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
    providePMFreqVoltState(unsigned int index) const = 0;
    virtual std::vector<unsigned int> providePMFreqVoltActiveStates() const = 0;
  };

  class Exporter : public IProfilePart::Exporter
  {
   public:
    virtual void takePMFreqVoltControlName(std::string const &mode) = 0;
    virtual void takePMFreqVoltVoltMode(std::string const &mode) = 0;
    virtual void takePMFreqVoltStates(
        std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                               units::voltage::millivolt_t>> const &states) = 0;
    virtual void
    takePMFreqVoltActiveStates(std::vector<unsigned int> const &states) = 0;
  };

  PMFreqVoltProfilePart() noexcept;

  std::unique_ptr<Exportable::Exporter>
  factory(IProfilePartProvider const &profilePartProvider) override;
  std::unique_ptr<Exportable::Exporter> initializer() override;

  std::string const &ID() const override;
  std::string const &instanceID() const override;

  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;

  bool provideActive() const override;

  std::string const &providePMFreqVoltVoltMode() const override;
  std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
  providePMFreqVoltState(unsigned int index) const override;
  std::vector<unsigned int> providePMFreqVoltActiveStates() const override;

 protected:
  void importProfilePart(IProfilePart::Importer &i) override;
  void exportProfilePart(IProfilePart::Exporter &e) const override;
  std::unique_ptr<IProfilePart> cloneProfilePart() const override;

 private:
  void voltMode(std::string const &mode);
  void state(unsigned int index,
             std::pair<units::frequency::megahertz_t,
                       units::voltage::millivolt_t> const &value);
  void activateStates(std::vector<unsigned int> const &states);

  class Initializer;

  std::string const id_;
  std::string controlName_;

  std::string voltMode_;
  std::vector<std::string> voltModes_;

  std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t> voltRange_;
  std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t> freqRange_;

  std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                         units::voltage::millivolt_t>>
      states_;

  std::vector<unsigned int> activeStates_;

  static bool const registered_;
};
} // namespace AMD
