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
#include "gpuxmlparser.h"

#include "core/info/igpuinfo.h"
#include "core/profilepartxmlparserprovider.h"
#include "igpu.h"
#include <utility>

class GPUXMLParser::Factory final
: public ProfilePartXMLParser::Factory
, public IGPUProfilePart::Exporter
{
 public:
  Factory(IProfilePartXMLParserProvider const &profilePartParserProvider,
          GPUXMLParser &parser) noexcept
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

  void takeIndex(int) override
  {
  }

  void takeDeviceID(std::string const &) override
  {
  }

  void takeRevision(std::string const &) override
  {
  }

 private:
  GPUXMLParser &outer_;
};

void GPUXMLParser::Factory::takePartParser(
    Item const &i, std::unique_ptr<IProfilePartXMLParser> &&part)
{
  outer_.parsers_.emplace(i.ID(), std::move(part));
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
GPUXMLParser::Factory::provideExporter(Item const &i)
{
  if (i.ID() == IGPU::ItemID)
    return *this;
  else
    return factory(i);
}

class GPUXMLParser::Initializer final : public IGPUProfilePart::Exporter
{
 public:
  Initializer(GPUXMLParser &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;

  void takeActive(bool active) override;
  void takeIndex(int index) override;
  void takeDeviceID(std::string const &deviceID) override;
  void takeRevision(std::string const &revision) override;

 private:
  GPUXMLParser &outer_;
  std::unordered_map<std::string, std::unique_ptr<Exportable::Exporter>> initializers_;
};

std::optional<std::reference_wrapper<Exportable::Exporter>>
GPUXMLParser::Initializer::provideExporter(Item const &i)
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

void GPUXMLParser::Initializer::takeActive(bool active)
{
  outer_.active_ = outer_.activeDefault_ = active;
}

void GPUXMLParser::Initializer::takeIndex(int index)
{
  outer_.index_ = outer_.indexDefault_ = index;
}

void GPUXMLParser::Initializer::takeDeviceID(std::string const &deviceID)
{
  outer_.deviceID_ = outer_.deviceIDDefault_ = deviceID;
}

void GPUXMLParser::Initializer::takeRevision(std::string const &revision)
{
  outer_.revision_ = outer_.revisionDefault_ = revision;
}

GPUXMLParser::GPUXMLParser() noexcept
: ProfilePartXMLParser(*this, *this)
, id_(IGPU::ItemID)
{
}

std::unique_ptr<Exportable::Exporter> GPUXMLParser::factory(
    IProfilePartXMLParserProvider const &profilePartParserProvider)
{
  return std::make_unique<GPUXMLParser::Factory>(profilePartParserProvider,
                                                 *this);
}

std::unique_ptr<Exportable::Exporter> GPUXMLParser::initializer()
{
  return std::make_unique<GPUXMLParser::Initializer>(*this);
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
GPUXMLParser::provideExporter(Item const &i)
{
  auto parserIt = parsers_.find(i.ID());
  if (parserIt != parsers_.cend())
    return parserIt->second->profilePartExporter();

  return {};
}

std::optional<std::reference_wrapper<Importable::Importer>>
GPUXMLParser::provideImporter(Item const &i)
{
  auto parserIt = parsers_.find(i.ID());
  if (parserIt != parsers_.cend())
    return parserIt->second->profilePartImporter();

  return {};
}

void GPUXMLParser::takeActive(bool active)
{
  active_ = active;
}

bool GPUXMLParser::provideActive() const
{
  return active_;
}

void GPUXMLParser::takeIndex(int index)
{
  index_ = index;
}

int GPUXMLParser::provideIndex() const
{
  return index_;
}

void GPUXMLParser::takeDeviceID(std::string const &deviceID)
{
  deviceID_ = deviceID;
}

std::string const &GPUXMLParser::provideDeviceID() const
{
  return deviceID_;
}

void GPUXMLParser::takeRevision(std::string const &revision)
{
  revision_ = revision;
}

std::string const &GPUXMLParser::provideRevision() const
{
  return revision_;
}

void GPUXMLParser::appendTo(pugi::xml_node &parentNode)
{
  auto gpuNode = parentNode.append_child(id_.c_str());
  gpuNode.append_attribute("active") = active_;
  gpuNode.append_attribute("index") = index_;
  gpuNode.append_attribute("deviceid") = deviceID_.c_str();
  gpuNode.append_attribute("revision") = revision_.c_str();

  for (auto &[key, component] : parsers_)
    component->appendTo(gpuNode);
}

void GPUXMLParser::resetAttributes()
{
  active_ = activeDefault_;
  index_ = indexDefault_;
  deviceID_ = deviceIDDefault_;
  revision_ = revisionDefault_;
}

void GPUXMLParser::loadPartFrom(pugi::xml_node const &parentNode)
{
  auto gpuNode = parentNode.find_child([&](pugi::xml_node const &node) {
    // match gpu node
    if (node.name() != id_)
      return false;

    // match specific gpu
    return node.attribute("index").as_int(-1) == index_ &&
           node.attribute(IGPUInfo::Keys::deviceID.data()).as_string() ==
               deviceID_ &&
           node.attribute(IGPUInfo::Keys::revision.data()).as_string() ==
               revision_;
  });

  active_ = gpuNode.attribute("active").as_bool(activeDefault_);

  for (auto &[key, component] : parsers_)
    component->loadFrom(gpuNode);
}

bool const GPUXMLParser::registered_ =
    ProfilePartXMLParserProvider::registerProvider(
        IGPU::ItemID, []() { return std::make_unique<GPUXMLParser>(); });
