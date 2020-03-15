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

#include "controlmode.h"
#include "core/profilepart.h"
#include <memory>
#include <string>
#include <string_view>
#include <vector>

class ControlModeProfilePart
: public ProfilePart
, public ControlMode::Importer
{
 public:
  class Importer : public IProfilePart::Importer
  {
   public:
    virtual std::string const &provideMode() const = 0;
  };

  class Exporter : public IProfilePart::Exporter
  {
   public:
    virtual void takeMode(std::string const &mode) = 0;
  };

  ControlModeProfilePart(std::string_view id) noexcept;

  std::unique_ptr<Exportable::Exporter>
  factory(IProfilePartProvider const &profilePartProvider) final override;
  std::unique_ptr<Exportable::Exporter> initializer() final override;

  std::string const &ID() const final override;

  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) final override;

  bool provideActive() const final override;
  std::string const &provideMode() const final override;

 protected:
  void importProfilePart(IProfilePart::Importer &i) final override;
  void exportProfilePart(IProfilePart::Exporter &e) const final override;
  std::unique_ptr<IProfilePart> cloneProfilePart() const final override;

  virtual std::unique_ptr<ControlModeProfilePart> instance() const = 0;

 private:
  void mode(std::string const &mode);

  class Factory;
  class Initializer;

  std::vector<std::unique_ptr<IProfilePart>> parts_;
  std::string const id_;
  std::string mode_;
};
