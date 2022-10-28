// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/profilepart.h"
#include "pmfreqod.h"
#include <string>

namespace AMD {

class PMFreqOdProfilePart final
: public ProfilePart
, public PMFreqOd::Importer
{
 public:
  class Importer : public IProfilePart::Importer
  {
   public:
    virtual unsigned int providePMFreqOdSclkOd() const = 0;
    virtual unsigned int providePMFreqOdMclkOd() const = 0;
  };

  class Exporter : public IProfilePart::Exporter
  {
   public:
    virtual void takePMFreqOdSclkOd(unsigned int value) = 0;
    virtual void takePMFreqOdMclkOd(unsigned int value) = 0;
  };

  PMFreqOdProfilePart() noexcept;

  std::unique_ptr<Exportable::Exporter>
  factory(IProfilePartProvider const &profilePartProvider) override;
  std::unique_ptr<Exportable::Exporter> initializer() override;

  std::string const &ID() const override;

  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;

  bool provideActive() const override;

  unsigned int providePMFreqOdSclkOd() const override;
  unsigned int providePMFreqOdMclkOd() const override;

 protected:
  void importProfilePart(IProfilePart::Importer &i) override;
  void exportProfilePart(IProfilePart::Exporter &e) const override;
  std::unique_ptr<IProfilePart> cloneProfilePart() const override;

 private:
  void sclkOd(unsigned int value);
  void mclkOd(unsigned int value);

  class Initializer;

  std::string const id_;

  unsigned int sclkOd_;
  unsigned int mclkOd_;

  static bool const registered_;
};
} // namespace AMD
