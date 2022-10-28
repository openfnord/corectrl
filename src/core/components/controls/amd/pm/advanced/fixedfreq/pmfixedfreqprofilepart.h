// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/profilepart.h"
#include "pmfixedfreq.h"
#include <string>
#include <vector>

namespace AMD {

class PMFixedFreqProfilePart final
: public ProfilePart
, public PMFixedFreq::Importer
{
 public:
  class Importer : public IProfilePart::Importer
  {
   public:
    virtual unsigned int providePMFixedFreqSclkIndex() const = 0;
    virtual unsigned int providePMFixedFreqMclkIndex() const = 0;
  };

  class Exporter : public IProfilePart::Exporter
  {
   public:
    virtual void takePMFixedFreqSclkIndex(unsigned int index) = 0;
    virtual void takePMFixedFreqMclkIndex(unsigned int index) = 0;
  };

  PMFixedFreqProfilePart() noexcept;

  std::unique_ptr<Exportable::Exporter>
  factory(IProfilePartProvider const &profilePartProvider) override;
  std::unique_ptr<Exportable::Exporter> initializer() override;

  std::string const &ID() const override;

  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;

  bool provideActive() const override;

  unsigned int providePMFixedFreqSclkIndex() const override;
  unsigned int providePMFixedFreqMclkIndex() const override;

 protected:
  void importProfilePart(IProfilePart::Importer &i) override;
  void exportProfilePart(IProfilePart::Exporter &e) const override;
  std::unique_ptr<IProfilePart> cloneProfilePart() const override;

 private:
  void sclkIndex(unsigned int index);
  void mclkIndex(unsigned int index);
  void clkIndex(unsigned int &targetIndex, unsigned int newIndex,
                std::vector<unsigned int> const &availableIndices) const;

  class Initializer;

  std::string const id_;

  unsigned int sclkIndex_;
  unsigned int mclkIndex_;

  std::vector<unsigned int> sclkIndices_;
  std::vector<unsigned int> mclkIndices_;

  static bool const registered_;
};
} // namespace AMD
