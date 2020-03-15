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

#include "exportable.h"
#include "iprofile.h"
#include "iprofilepartview.h"
#include "iprofileview.h"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

class ProfileView final : public IProfileView
{
 public:
  std::string const &name() const override;
  std::vector<std::unique_ptr<IProfilePartView>> const &parts() const override;

  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;

  class Initializer : public IProfile::Exporter
  {
   public:
    Initializer(ProfileView &profileView,
                std::optional<std::reference_wrapper<IProfileView>> base =
                    std::nullopt) noexcept;

    std::optional<std::reference_wrapper<Exportable::Exporter>>
    provideExporter(Item const &i) override;

    void takeActive(bool active) override;
    void takeInfo(IProfile::Info const &info) override;

   private:
    ProfileView &profileView_;
    std::optional<std::reference_wrapper<IProfileView>> base_;
  };

 private:
  std::string name_;
  std::vector<std::unique_ptr<IProfilePartView>> parts_;
};
