// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/profilepart.h"
#include <string>
#include <string_view>

class GraphItemProfilePart : public ProfilePart
{
 public:
  class Importer : public IProfilePart::Importer
  {
   public:
    virtual std::string const &provideColor() const = 0;
  };

  class Exporter : public IProfilePart::Exporter
  {
   public:
    virtual void takeColor(std::string const &color) = 0;
  };

  GraphItemProfilePart(std::string_view id, std::string_view color) noexcept;

  std::unique_ptr<Exportable::Exporter>
  factory(IProfilePartProvider const &profilePartProvider) override;
  std::unique_ptr<Exportable::Exporter> initializer() override;

  std::string const &ID() const override;

 protected:
  void importProfilePart(IProfilePart::Importer &i) override;
  void exportProfilePart(IProfilePart::Exporter &e) const override;
  std::unique_ptr<IProfilePart> cloneProfilePart() const override;

 private:
  std::string id_;
  std::string color_;
};
