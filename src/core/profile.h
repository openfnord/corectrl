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

#include "iprofile.h"
#include "iprofileview.h"
#include "isysmodel.h"
#include "profilepart.h"
#include <memory>
#include <string>
#include <vector>

class IProfilePartProvider;

class Profile final
: public IProfile
, public IProfileView::View
{
 public:
  Profile() noexcept;

  Profile(Profile const &) = delete;
  Profile &operator=(Profile const &) = delete;

  std::unique_ptr<Exportable::Exporter> initializer() override;

  bool active() const override;
  void activate(bool active) override;

  IProfile::Info const &info() const override;
  void info(IProfile::Info const &info) override;

  std::unique_ptr<IProfile> clone() const override;

  std::string const &ID() const override;
  void importWith(Importable::Importer &i) override;
  void exportWith(Exportable::Exporter &e) const override;

  std::string const &name() const override;
  std::vector<std::shared_ptr<IProfilePart>> const &parts() const override;

  class Factory final
  : public ProfilePart::Factory
  , public ISysModel::Exporter
  {
   public:
    Factory(IProfilePartProvider const &profilePartProvider,
            Profile &outer) noexcept;

    void takeProfilePart(std::unique_ptr<IProfilePart> &&part) override;

    std::optional<std::reference_wrapper<Exportable::Exporter>>
    provideExporter(Item const &i) override;

   private:
    Profile &outer_;
  };

 private:
  class Initializer;

  std::string const id_;
  std::vector<std::shared_ptr<IProfilePart>> parts_;

  IProfile::Info info_;
  bool active_{true};
};
