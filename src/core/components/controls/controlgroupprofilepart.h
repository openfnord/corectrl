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

#include "controlgroup.h"
#include "core/profilepart.h"
#include <memory>
#include <string>
#include <string_view>
#include <vector>

class ControlGroupProfilePart
: public ProfilePart
, public ControlGroup::Importer
{
 public:
  class Importer : public IProfilePart::Importer
  {
  };

  class Exporter : public IProfilePart::Exporter
  {
  };

  ControlGroupProfilePart(std::string_view id) noexcept;

  std::unique_ptr<Exportable::Exporter>
  factory(IProfilePartProvider const &profilePartProvider) final override;
  std::unique_ptr<Exportable::Exporter> initializer() final override;

  std::string const &ID() const final override;

  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) final override;

  bool provideActive() const final override;

 protected:
  void importProfilePart(IProfilePart::Importer &i) final override;
  void exportProfilePart(IProfilePart::Exporter &e) const final override;
  std::unique_ptr<IProfilePart> cloneProfilePart() const final override;

  virtual std::unique_ptr<ControlGroupProfilePart> instance() const = 0;

 private:
  class Factory;
  class Initializer;

  std::vector<std::unique_ptr<IProfilePart>> parts_;
  std::string const id_;
};
