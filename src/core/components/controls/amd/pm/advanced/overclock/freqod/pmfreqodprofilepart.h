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
