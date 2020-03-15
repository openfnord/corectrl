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
