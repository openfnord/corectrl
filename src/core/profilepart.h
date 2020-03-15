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
#include "iprofilepart.h"
#include "iprofilepartprovider.h"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

class ProfilePart : public IProfilePart
{
 public:
  ProfilePart() = default;
  ProfilePart(ProfilePart const &) = delete;
  ProfilePart &operator=(ProfilePart const &) = delete;

  bool active() const final override;
  void activate(bool active) final override;

  std::unique_ptr<IProfilePart> clone() const final override;

  void importWith(Importable::Importer &i) final override;
  void exportWith(Exportable::Exporter &e) const final override;

  class Factory
  {
   public:
    Factory(IProfilePartProvider const &profilePartProvider) noexcept;

    std::optional<std::reference_wrapper<Exportable::Exporter>>
    factory(std::string const &componentID);

    virtual void takeProfilePart(std::unique_ptr<IProfilePart> &&part) = 0;
    virtual ~Factory() = default;

   protected:
    std::unique_ptr<IProfilePart> createPart(std::string const &componentID) const;

   private:
    IProfilePartProvider const &profilePartProvider_;
    std::vector<std::unique_ptr<Exportable::Exporter>> factories_;
  };

 protected:
  virtual void importProfilePart(IProfilePart::Importer &i) = 0;
  virtual void exportProfilePart(IProfilePart::Exporter &e) const = 0;
  virtual std::unique_ptr<IProfilePart> cloneProfilePart() const = 0;

 private:
  bool active_{true};
};
