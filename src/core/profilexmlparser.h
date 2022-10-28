// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "iprofile.h"
#include "iprofileparser.h"
#include "profilepartxmlparser.h"
#include <memory>
#include <string>
#include <unordered_map>

class IProfilePartXMLParser;
class IProfilePartXMLParserProvider;

class ProfileXMLParser final
: public IProfileParser
, public IProfile::Importer
, public IProfile::Exporter
{
 public:
  ProfileXMLParser() noexcept;

  std::string const &format() override;
  std::unique_ptr<Exportable::Exporter> initializer() override;
  bool load(std::vector<char> const &data, IProfile &profile) override;
  bool save(std::vector<char> &data, IProfile const &profile) override;

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;
  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;

  void takeActive(bool active) override;
  void takeInfo(IProfile::Info const &info) override;

  bool provideActive() const override;
  IProfile::Info const &provideInfo() const override;

  class Factory final
  : public ProfilePartXMLParser::Factory
  , public IProfile::Exporter
  {
   public:
    Factory(IProfilePartXMLParserProvider const &profilePartParserProvider,
            ProfileXMLParser &outer) noexcept;

    void takePartParser(Item const &i,
                        std::unique_ptr<IProfilePartXMLParser> &&part) override;

    std::optional<std::reference_wrapper<Exportable::Exporter>>
    provideExporter(Item const &i) override;

    void takeActive(bool) override
    {
    }

    void takeInfo(IProfile::Info const &) override
    {
    }

   private:
    ProfileXMLParser &outer_;
  };

 private:
  class Initializer;

  std::string const format_;
  std::string profileNodeName_;
  std::unordered_map<std::string, std::unique_ptr<IProfilePartXMLParser>> parsers_;

  IProfile::Info info_;
  IProfile::Info infoDefault_;

  bool active_;
  bool activeDefault_;
};
