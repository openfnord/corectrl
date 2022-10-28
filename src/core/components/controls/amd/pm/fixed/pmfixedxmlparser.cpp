// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmfixedxmlparser.h"

#include "core/profilepartxmlparserprovider.h"
#include "pmfixed.h"
#include <memory>

class AMD::PMFixedXMLParser::Initializer final
: public AMD::PMFixedProfilePart::Exporter
{
 public:
  Initializer(AMD::PMFixedXMLParser &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takeActive(bool active) override;
  void takePMFixedMode(std::string const &mode) override;

 private:
  AMD::PMFixedXMLParser &outer_;
};

void AMD::PMFixedXMLParser::Initializer::takeActive(bool active)
{
  outer_.active_ = outer_.activeDefault_ = active;
}

void AMD::PMFixedXMLParser::Initializer::takePMFixedMode(std::string const &mode)
{
  outer_.mode_ = outer_.modeDefault_ = mode;
}

AMD::PMFixedXMLParser::PMFixedXMLParser() noexcept
: ProfilePartXMLParser(AMD::PMFixed::ItemID, *this, *this)
{
}

std::unique_ptr<Exportable::Exporter>
AMD::PMFixedXMLParser::factory(IProfilePartXMLParserProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> AMD::PMFixedXMLParser::initializer()
{
  return std::make_unique<AMD::PMFixedXMLParser::Initializer>(*this);
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
AMD::PMFixedXMLParser::provideExporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMFixedXMLParser::provideImporter(Item const &)
{
  return {};
}

void AMD::PMFixedXMLParser::takeActive(bool active)
{
  active_ = active;
}

bool AMD::PMFixedXMLParser::provideActive() const
{
  return active_;
}

void AMD::PMFixedXMLParser::takePMFixedMode(std::string const &mode)
{
  mode_ = mode;
}

std::string const &AMD::PMFixedXMLParser::providePMFixedMode() const
{
  return mode_;
}

void AMD::PMFixedXMLParser::appendTo(pugi::xml_node &parentNode)
{
  auto pmFixedNode = parentNode.append_child(ID().c_str());
  pmFixedNode.append_attribute("active") = active_;
  pmFixedNode.append_attribute("mode") = mode_.c_str();
}

void AMD::PMFixedXMLParser::resetAttributes()
{
  active_ = activeDefault_;
  mode_ = modeDefault_;
}

void AMD::PMFixedXMLParser::loadPartFrom(pugi::xml_node const &parentNode)
{
  auto pmFixedNode = parentNode.find_child(
      [&](pugi::xml_node const &node) { return node.name() == ID(); });

  active_ = pmFixedNode.attribute("active").as_bool(activeDefault_);
  mode_ = pmFixedNode.attribute("mode").as_string(modeDefault_.c_str());
}

bool const AMD::PMFixedXMLParser::registered_ =
    ProfilePartXMLParserProvider::registerProvider(AMD::PMFixed::ItemID, []() {
      return std::make_unique<AMD::PMFixedXMLParser>();
    });
