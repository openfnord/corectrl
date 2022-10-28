// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmpowerstatexmlparser.h"

#include "core/profilepartxmlparserprovider.h"
#include "pmpowerstate.h"
#include <memory>

class AMD::PMPowerStateXMLParser::Initializer final
: public AMD::PMPowerStateProfilePart::Exporter
{
 public:
  Initializer(AMD::PMPowerStateXMLParser &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takeActive(bool active) override;
  void takePMPowerStateMode(std::string const &mode) override;

 private:
  AMD::PMPowerStateXMLParser &outer_;
};

void AMD::PMPowerStateXMLParser::Initializer::takeActive(bool active)
{
  outer_.active_ = outer_.activeDefault_ = active;
}

void AMD::PMPowerStateXMLParser::Initializer::takePMPowerStateMode(
    std::string const &mode)
{
  outer_.mode_ = outer_.modeDefault_ = mode;
}

AMD::PMPowerStateXMLParser::PMPowerStateXMLParser() noexcept
: ProfilePartXMLParser(AMD::PMPowerState::ItemID, *this, *this)
{
}

std::unique_ptr<Exportable::Exporter>
AMD::PMPowerStateXMLParser::factory(IProfilePartXMLParserProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> AMD::PMPowerStateXMLParser::initializer()
{
  return std::make_unique<AMD::PMPowerStateXMLParser::Initializer>(*this);
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
AMD::PMPowerStateXMLParser::provideExporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMPowerStateXMLParser::provideImporter(Item const &)
{
  return {};
}

void AMD::PMPowerStateXMLParser::takeActive(bool active)
{
  active_ = active;
}

bool AMD::PMPowerStateXMLParser::provideActive() const
{
  return active_;
}

void AMD::PMPowerStateXMLParser::takePMPowerStateMode(std::string const &mode)
{
  mode_ = mode;
}

std::string const &AMD::PMPowerStateXMLParser::providePMPowerStateMode() const
{
  return mode_;
}

void AMD::PMPowerStateXMLParser::appendTo(pugi::xml_node &parentNode)
{
  auto pmFixedNode = parentNode.append_child(ID().c_str());
  pmFixedNode.append_attribute("active") = active_;
  pmFixedNode.append_attribute("mode") = mode_.c_str();
}

void AMD::PMPowerStateXMLParser::resetAttributes()
{
  active_ = activeDefault_;
  mode_ = modeDefault_;
}

void AMD::PMPowerStateXMLParser::loadPartFrom(pugi::xml_node const &parentNode)
{
  auto pmFixedNode = parentNode.find_child(
      [&](pugi::xml_node const &node) { return node.name() == ID(); });

  active_ = pmFixedNode.attribute("active").as_bool(activeDefault_);
  mode_ = pmFixedNode.attribute("mode").as_string(modeDefault_.c_str());
}

bool const AMD::PMPowerStateXMLParser::registered_ =
    ProfilePartXMLParserProvider::registerProvider(
        AMD::PMPowerState::ItemID,
        []() { return std::make_unique<AMD::PMPowerStateXMLParser>(); });
