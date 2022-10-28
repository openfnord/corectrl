// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmfixedfreqxmlparser.h"

#include "core/profilepartxmlparserprovider.h"
#include "pmfixedfreq.h"
#include <memory>

class AMD::PMFixedFreqXMLParser::Initializer final
: public AMD::PMFixedFreqProfilePart::Exporter
{
 public:
  Initializer(AMD::PMFixedFreqXMLParser &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takeActive(bool active) override;
  void takePMFixedFreqSclkIndex(unsigned int index) override;
  void takePMFixedFreqMclkIndex(unsigned int index) override;

 private:
  AMD::PMFixedFreqXMLParser &outer_;
};

void AMD::PMFixedFreqXMLParser::Initializer::takeActive(bool active)
{
  outer_.active_ = outer_.activeDefault_ = active;
}

void AMD::PMFixedFreqXMLParser::Initializer::takePMFixedFreqSclkIndex(
    unsigned int index)
{
  outer_.sclkIndex_ = outer_.sclkIndexDefault_ = index;
}

void AMD::PMFixedFreqXMLParser::Initializer::takePMFixedFreqMclkIndex(
    unsigned int index)
{
  outer_.mclkIndex_ = outer_.mclkIndexDefault_ = index;
}

AMD::PMFixedFreqXMLParser::PMFixedFreqXMLParser() noexcept
: ProfilePartXMLParser(AMD::PMFixedFreq::ItemID, *this, *this)
{
}

std::unique_ptr<Exportable::Exporter>
AMD::PMFixedFreqXMLParser::factory(IProfilePartXMLParserProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> AMD::PMFixedFreqXMLParser::initializer()
{
  return std::make_unique<AMD::PMFixedFreqXMLParser::Initializer>(*this);
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
AMD::PMFixedFreqXMLParser::provideExporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMFixedFreqXMLParser::provideImporter(Item const &)
{
  return {};
}

void AMD::PMFixedFreqXMLParser::takeActive(bool active)
{
  active_ = active;
}

bool AMD::PMFixedFreqXMLParser::provideActive() const
{
  return active_;
}

void AMD::PMFixedFreqXMLParser::takePMFixedFreqSclkIndex(unsigned int index)
{
  sclkIndex_ = index;
}

unsigned int AMD::PMFixedFreqXMLParser::providePMFixedFreqSclkIndex() const
{
  return sclkIndex_;
}

void AMD::PMFixedFreqXMLParser::takePMFixedFreqMclkIndex(unsigned int index)
{
  mclkIndex_ = index;
}

unsigned int AMD::PMFixedFreqXMLParser::providePMFixedFreqMclkIndex() const
{
  return mclkIndex_;
}

void AMD::PMFixedFreqXMLParser::appendTo(pugi::xml_node &parentNode)
{
  auto pmFrequencyNode = parentNode.append_child(ID().c_str());
  pmFrequencyNode.append_attribute("active") = active_;
  pmFrequencyNode.append_attribute("sclkState") = sclkIndex_;
  pmFrequencyNode.append_attribute("mclkState") = mclkIndex_;
}

void AMD::PMFixedFreqXMLParser::resetAttributes()
{
  active_ = activeDefault_;
  sclkIndex_ = sclkIndexDefault_;
  mclkIndex_ = mclkIndexDefault_;
}

void AMD::PMFixedFreqXMLParser::loadPartFrom(pugi::xml_node const &parentNode)
{
  auto pmFrequencyNode = parentNode.find_child(
      [&](pugi::xml_node const &node) { return node.name() == ID(); });

  active_ = pmFrequencyNode.attribute("active").as_bool(activeDefault_);
  sclkIndex_ = pmFrequencyNode.attribute("sclkState").as_uint(sclkIndexDefault_);
  mclkIndex_ = pmFrequencyNode.attribute("mclkState").as_uint(mclkIndexDefault_);
}

bool const AMD::PMFixedFreqXMLParser::registered_ =
    ProfilePartXMLParserProvider::registerProvider(
        AMD::PMFixedFreq::ItemID,
        []() { return std::make_unique<AMD::PMFixedFreqXMLParser>(); });
