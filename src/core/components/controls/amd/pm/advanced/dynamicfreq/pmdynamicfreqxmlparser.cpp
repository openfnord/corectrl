// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmdynamicfreqxmlparser.h"

#include "core/profilepartxmlparserprovider.h"
#include "pmdynamicfreq.h"
#include <memory>

class AMD::PMDynamicFreqXMLParser::Initializer final
: public AMD::PMDynamicFreqProfilePart::Exporter
{
 public:
  Initializer(AMD::PMDynamicFreqXMLParser &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takeActive(bool active) override;

 private:
  AMD::PMDynamicFreqXMLParser &outer_;
};

void AMD::PMDynamicFreqXMLParser::Initializer::takeActive(bool active)
{
  outer_.active_ = outer_.activeDefault_ = active;
}

AMD::PMDynamicFreqXMLParser::PMDynamicFreqXMLParser() noexcept
: ProfilePartXMLParser(AMD::PMDynamicFreq::ItemID, *this, *this)
{
}

std::unique_ptr<Exportable::Exporter>
AMD::PMDynamicFreqXMLParser::factory(IProfilePartXMLParserProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> AMD::PMDynamicFreqXMLParser::initializer()
{
  return std::make_unique<AMD::PMDynamicFreqXMLParser::Initializer>(*this);
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
AMD::PMDynamicFreqXMLParser::provideExporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMDynamicFreqXMLParser::provideImporter(Item const &)
{
  return {};
}

void AMD::PMDynamicFreqXMLParser::takeActive(bool active)
{
  active_ = active;
}

bool AMD::PMDynamicFreqXMLParser::provideActive() const
{
  return active_;
}

void AMD::PMDynamicFreqXMLParser::appendTo(pugi::xml_node &parentNode)
{
  auto pmFrequencyNode = parentNode.append_child(ID().c_str());
  pmFrequencyNode.append_attribute("active") = active_;
}

void AMD::PMDynamicFreqXMLParser::resetAttributes()
{
  active_ = activeDefault_;
}

void AMD::PMDynamicFreqXMLParser::loadPartFrom(pugi::xml_node const &parentNode)
{
  auto pmFrequencyNode = parentNode.find_child(
      [&](pugi::xml_node const &node) { return node.name() == ID(); });

  active_ = pmFrequencyNode.attribute("active").as_bool(activeDefault_);
}

bool const AMD::PMDynamicFreqXMLParser::registered_ =
    ProfilePartXMLParserProvider::registerProvider(
        AMD::PMDynamicFreq::ItemID,
        []() { return std::make_unique<AMD::PMDynamicFreqXMLParser>(); });
