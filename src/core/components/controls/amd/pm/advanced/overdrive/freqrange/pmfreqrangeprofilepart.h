// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/profilepart.h"
#include "pmfreqrange.h"
#include <string>
#include <utility>
#include <vector>

namespace AMD {

class PMFreqRangeProfilePart final
: public ProfilePart
, public PMFreqRange::Importer
{
 public:
  class Importer : public IProfilePart::Importer
  {
   public:
    virtual units::frequency::megahertz_t
    providePMFreqRangeState(unsigned int index) const = 0;
  };

  class Exporter : public IProfilePart::Exporter
  {
   public:
    virtual void takePMFreqRangeControlName(std::string const &mode) = 0;
    virtual void takePMFreqRangeStates(
        std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
            &states) = 0;
  };

  PMFreqRangeProfilePart() noexcept;

  std::string const &ID() const override;
  std::string const &instanceID() const override;

  std::unique_ptr<Exportable::Exporter>
  factory(IProfilePartProvider const &profilePartProvider) override;
  std::unique_ptr<Exportable::Exporter> initializer() override;

  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;

  bool provideActive() const override;

  units::frequency::megahertz_t
  providePMFreqRangeState(unsigned int index) const override;

 protected:
  void importProfilePart(IProfilePart::Importer &i) override;
  void exportProfilePart(IProfilePart::Exporter &e) const override;
  std::unique_ptr<IProfilePart> cloneProfilePart() const override;

 private:
  void setState(unsigned int index, units::frequency::megahertz_t freq);

  class Initializer;

  std::string const id_;
  std::string controlName_;
  std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> states_;
  std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t> stateRange_;

  static bool const registered_;
};
} // namespace AMD
