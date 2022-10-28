// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmfreqodxmlparser.h"

#include "core/profilepartxmlparserprovider.h"
#include "pmfreqod.h"
#include <memory>

class AMD::PMFreqOdXMLParser::Initializer final
: public AMD::PMFreqOdProfilePart::Exporter
{
 public:
  Initializer(AMD::PMFreqOdXMLParser &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takeActive(bool active) override;
  void takePMFreqOdSclkOd(unsigned int value) override;
  void takePMFreqOdMclkOd(unsigned int value) override;

 private:
  AMD::PMFreqOdXMLParser &outer_;
};

void AMD::PMFreqOdXMLParser::Initializer::takeActive(bool active)
{
  outer_.active_ = outer_.activeDefault_ = active;
}

void AMD::PMFreqOdXMLParser::Initializer::takePMFreqOdSclkOd(unsigned int value)
{
  outer_.sclkOd_ = outer_.sclkOdDefault_ = value;
}

void AMD::PMFreqOdXMLParser::Initializer::takePMFreqOdMclkOd(unsigned int value)
{
  outer_.mclkOd_ = outer_.mclkOdDefault_ = value;
}

AMD::PMFreqOdXMLParser::PMFreqOdXMLParser() noexcept
: ProfilePartXMLParser(AMD::PMFreqOd::ItemID, *this, *this)
{
}

std::unique_ptr<Exportable::Exporter>
AMD::PMFreqOdXMLParser::factory(IProfilePartXMLParserProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> AMD::PMFreqOdXMLParser::initializer()
{
  return std::make_unique<AMD::PMFreqOdXMLParser::Initializer>(*this);
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
AMD::PMFreqOdXMLParser::provideExporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMFreqOdXMLParser::provideImporter(Item const &)
{
  return {};
}

void AMD::PMFreqOdXMLParser::takeActive(bool active)
{
  active_ = active;
}

bool AMD::PMFreqOdXMLParser::provideActive() const
{
  return active_;
}

unsigned int AMD::PMFreqOdXMLParser::providePMFreqOdSclkOd() const
{
  return sclkOd_;
}

void AMD::PMFreqOdXMLParser::takePMFreqOdSclkOd(unsigned int value)
{
  sclkOd_ = value;
}

unsigned int AMD::PMFreqOdXMLParser::providePMFreqOdMclkOd() const
{
  return mclkOd_;
}

void AMD::PMFreqOdXMLParser::takePMFreqOdMclkOd(unsigned int value)
{
  mclkOd_ = value;
}

void AMD::PMFreqOdXMLParser::appendTo(pugi::xml_node &parentNode)
{
  auto pmFrequencyNode = parentNode.append_child(ID().c_str());
  pmFrequencyNode.append_attribute("active") = active_;
  pmFrequencyNode.append_attribute("sclkOd") = sclkOd_;
  pmFrequencyNode.append_attribute("mclkOd") = mclkOd_;
}

void AMD::PMFreqOdXMLParser::resetAttributes()
{
  active_ = activeDefault_;
  sclkOd_ = sclkOdDefault_;
  mclkOd_ = mclkOdDefault_;
}

void AMD::PMFreqOdXMLParser::loadPartFrom(pugi::xml_node const &parentNode)
{
  auto pmFrequencyNode = parentNode.find_child(
      [&](pugi::xml_node const &node) { return node.name() == ID(); });

  active_ = pmFrequencyNode.attribute("active").as_bool(activeDefault_);
  sclkOd_ = pmFrequencyNode.attribute("sclkOd").as_uint(sclkOdDefault_);
  mclkOd_ = pmFrequencyNode.attribute("mclkOd").as_uint(mclkOdDefault_);
}

bool const AMD::PMFreqOdXMLParser::registered_ =
    ProfilePartXMLParserProvider::registerProvider(AMD::PMFreqOd::ItemID, []() {
      return std::make_unique<AMD::PMFreqOdXMLParser>();
    });
