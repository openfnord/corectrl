// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
