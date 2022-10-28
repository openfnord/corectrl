// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "cpuxmlparser.h"

#include "core/info/icpuinfo.h"
#include "core/profilepartxmlparserprovider.h"
#include "icpu.h"
#include <utility>

class CPUXMLParser::Factory final
: public ProfilePartXMLParser::Factory
, public ICPUProfilePart::Exporter
{
 public:
  Factory(IProfilePartXMLParserProvider const &profilePartParserProvider,
          CPUXMLParser &parser) noexcept
  : ProfilePartXMLParser::Factory(profilePartParserProvider)
  , outer_(parser)
  {
  }

  void takePartParser(Item const &i,
                      std::unique_ptr<IProfilePartXMLParser> &&part) override;

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;

  void takeActive(bool) override
  {
  }

  void takeSocketId(int) override
  {
  }

 private:
  CPUXMLParser &outer_;
};

void CPUXMLParser::Factory::takePartParser(
    Item const &i, std::unique_ptr<IProfilePartXMLParser> &&part)
{
  outer_.parsers_.emplace(i.ID(), std::move(part));
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
CPUXMLParser::Factory::provideExporter(Item const &i)
{
  if (i.ID() == ICPU::ItemID)
    return *this;
  else
    return factory(i);
}

class CPUXMLParser::Initializer final : public ICPUProfilePart::Exporter
{
 public:
  Initializer(CPUXMLParser &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;

  void takeActive(bool active) override;
  void takeSocketId(int id) override;

 private:
  CPUXMLParser &outer_;
  std::unordered_map<std::string, std::unique_ptr<Exportable::Exporter>> initializers_;
};

std::optional<std::reference_wrapper<Exportable::Exporter>>
CPUXMLParser::Initializer::provideExporter(Item const &i)
{
  auto &id = i.ID();
  if (outer_.parsers_.count(id) > 0) {
    if (initializers_.count(id) > 0)
      return *initializers_.at(id);
    else {
      auto initializer = outer_.parsers_.at(id)->initializer();
      if (initializer != nullptr) {
        initializers_.emplace(id, std::move(initializer));
        return *initializers_.at(id);
      }
    }
  }

  return {};
}

void CPUXMLParser::Initializer::takeActive(bool active)
{
  outer_.active_ = outer_.activeDefault_ = active;
}

void CPUXMLParser::Initializer::takeSocketId(int id)
{
  outer_.socketId_ = outer_.socketIdDefault_ = id;
}

CPUXMLParser::CPUXMLParser() noexcept
: ProfilePartXMLParser(ICPU::ItemID, *this, *this)
{
}

std::unique_ptr<Exportable::Exporter> CPUXMLParser::factory(
    IProfilePartXMLParserProvider const &profilePartParserProvider)
{
  return std::make_unique<CPUXMLParser::Factory>(profilePartParserProvider,
                                                 *this);
}

std::unique_ptr<Exportable::Exporter> CPUXMLParser::initializer()
{
  return std::make_unique<CPUXMLParser::Initializer>(*this);
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
CPUXMLParser::provideExporter(Item const &i)
{
  auto parserIt = parsers_.find(i.ID());
  if (parserIt != parsers_.cend())
    return parserIt->second->profilePartExporter();

  return {};
}

std::optional<std::reference_wrapper<Importable::Importer>>
CPUXMLParser::provideImporter(Item const &i)
{
  auto parserIt = parsers_.find(i.ID());
  if (parserIt != parsers_.cend())
    return parserIt->second->profilePartImporter();

  return {};
}

void CPUXMLParser::takeActive(bool active)
{
  active_ = active;
}

bool CPUXMLParser::provideActive() const
{
  return active_;
}

void CPUXMLParser::takeSocketId(int id)
{
  socketId_ = id;
}

int CPUXMLParser::provideSocketId() const
{
  return socketId_;
}

void CPUXMLParser::appendTo(pugi::xml_node &parentNode)
{
  auto cpuNode = parentNode.append_child(ID().c_str());
  cpuNode.append_attribute("active") = active_;
  cpuNode.append_attribute("socketId") = socketId_;

  for (auto &[key, component] : parsers_)
    component->appendTo(cpuNode);
}

void CPUXMLParser::resetAttributes()
{
  active_ = activeDefault_;
  socketId_ = socketIdDefault_;
}

void CPUXMLParser::loadPartFrom(pugi::xml_node const &parentNode)
{
  auto cpuNode = parentNode.find_child([&](pugi::xml_node const &node) {
    // match cpu node
    if (node.name() != ID())
      return false;

    // match specific cpu
    return node.attribute("socketId").as_int(-1) == socketId_;
  });

  active_ = cpuNode.attribute("active").as_bool(activeDefault_);

  for (auto &[key, component] : parsers_)
    component->loadFrom(cpuNode);
}

bool const CPUXMLParser::registered_ =
    ProfilePartXMLParserProvider::registerProvider(
        ICPU::ItemID, []() { return std::make_unique<CPUXMLParser>(); });
