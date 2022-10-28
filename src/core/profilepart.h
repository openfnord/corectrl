// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
