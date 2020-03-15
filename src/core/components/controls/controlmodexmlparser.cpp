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
#include "controlmodexmlparser.h"

#include "core/item.h"
#include <utility>

class ControlModeXMLParser::Factory final
: public ProfilePartXMLParser::Factory
, public ControlModeProfilePart::Exporter
{
 public:
  Factory(IProfilePartXMLParserProvider const &profilePartParserProvider,
          ControlModeXMLParser &outer) noexcept
  : ProfilePartXMLParser::Factory(profilePartParserProvider)
  , outer_(outer)
  {
  }

  void takePartParser(Item const &i,
                      std::unique_ptr<IProfilePartXMLParser> &&part) override;

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;

  void takeActive(bool) override
  {
  }

  void takeMode(std::string const &) override
  {
  }

 private:
  ControlModeXMLParser &outer_;
};

void ControlModeXMLParser::Factory::takePartParser(
    Item const &i, std::unique_ptr<IProfilePartXMLParser> &&part)
{
  outer_.parsers_.emplace(i.ID(), std::move(part));
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
ControlModeXMLParser::Factory::provideExporter(Item const &i)
{
  if (i.ID() == outer_.id_)
    return *this;
  else
    return factory(i);
}

class ControlModeXMLParser::Initializer final
: public ControlModeProfilePart::Exporter
{
 public:
  Initializer(ControlModeXMLParser &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;

  void takeActive(bool active) override;
  void takeMode(std::string const &mode) override;

 private:
  ControlModeXMLParser &outer_;
  std::unordered_map<std::string, std::unique_ptr<Exportable::Exporter>> initializers_;
};

std::optional<std::reference_wrapper<Exportable::Exporter>>
ControlModeXMLParser::Initializer::provideExporter(Item const &i)
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

void ControlModeXMLParser::Initializer::takeActive(bool active)
{
  outer_.active_ = outer_.activeDefault_ = active;
}

void ControlModeXMLParser::Initializer::takeMode(std::string const &mode)
{
  outer_.mode_ = outer_.modeDefault_ = mode;
}

ControlModeXMLParser::ControlModeXMLParser(std::string_view id) noexcept
: ProfilePartXMLParser(*this, *this)
, id_(id)
{
}

std::unique_ptr<Exportable::Exporter> ControlModeXMLParser::factory(
    IProfilePartXMLParserProvider const &profilePartXMLParserProvider)
{
  return std::make_unique<ControlModeXMLParser::Factory>(
      profilePartXMLParserProvider, *this);
}

std::unique_ptr<Exportable::Exporter> ControlModeXMLParser::initializer()
{
  return std::make_unique<ControlModeXMLParser::Initializer>(*this);
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
ControlModeXMLParser::provideExporter(Item const &i)
{
  auto parserIt = parsers_.find(i.ID());
  if (parserIt != parsers_.cend())
    return parserIt->second->profilePartExporter();

  return {};
}

std::optional<std::reference_wrapper<Importable::Importer>>
ControlModeXMLParser::provideImporter(Item const &i)
{
  auto parserIt = parsers_.find(i.ID());
  if (parserIt != parsers_.cend())
    return parserIt->second->profilePartImporter();

  return {};
}

void ControlModeXMLParser::takeActive(bool active)
{
  active_ = active;
}

bool ControlModeXMLParser::provideActive() const
{
  return active_;
}

void ControlModeXMLParser::takeMode(std::string const &mode)
{
  mode_ = mode;
}

std::string const &ControlModeXMLParser::provideMode() const
{
  return mode_;
}

void ControlModeXMLParser::appendTo(pugi::xml_node &parentNode)
{
  auto node = parentNode.append_child(id_.c_str());
  node.append_attribute("active") = active_;
  node.append_attribute("mode") = mode_.c_str();

  for (auto &[key, component] : parsers_)
    component->appendTo(node);
}

void ControlModeXMLParser::resetAttributes()
{
  active_ = activeDefault_;
  mode_ = modeDefault_;
}

void ControlModeXMLParser::loadPartFrom(pugi::xml_node const &parentNode)
{
  auto node = parentNode.find_child(
      [&](pugi::xml_node const &node) { return node.name() == id_; });

  active_ = node.attribute("active").as_bool(activeDefault_);
  mode_ = node.attribute("mode").as_string(modeDefault_.c_str());

  for (auto &[key, component] : parsers_)
    component->loadFrom(node);
}
