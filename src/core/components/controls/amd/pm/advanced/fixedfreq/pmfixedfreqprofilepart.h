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
