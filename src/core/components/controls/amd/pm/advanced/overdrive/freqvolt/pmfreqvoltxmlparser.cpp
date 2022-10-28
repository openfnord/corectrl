// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmfreqvoltxmlparser.h"

#include "../pmoverdrive.h"
#include "core/profilepartxmlparserprovider.h"
#include "pmfreqvolt.h"
#include <algorithm>
#include <memory>

class AMD::PMFreqVoltXMLParser::Initializer final
: public AMD::PMFreqVoltProfilePart::Exporter
{
 public:
  Initializer(AMD::PMFreqVoltXMLParser &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takeActive(bool active) override;
  void takePMFreqVoltControlName(std::string const &name) override;
  void takePMFreqVoltVoltMode(std::string const &mode) override;
  void takePMFreqVoltStates(
      std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                             units::voltage::millivolt_t>> const &states) override;
  void
  takePMFreqVoltActiveStates(std::vector<unsigned int> const &states) override;

 private:
  AMD::PMFreqVoltXMLParser &outer_;
};

void AMD::PMFreqVoltXMLParser::Initializer::takeActive(bool active)
{
  outer_.active_ = outer_.activeDefault_ = active;
}

void AMD::PMFreqVoltXMLParser::Initializer::takePMFreqVoltControlName(
    std::string const &name)
{
  outer_.controlName_ = name;
  outer_.nodeID_ = name;
  std::transform(outer_.nodeID_.cbegin(), outer_.nodeID_.cend(),
                 outer_.nodeID_.begin(), ::tolower);
}

void AMD::PMFreqVoltXMLParser::Initializer::takePMFreqVoltVoltMode(
    std::string const &mode)
{
  outer_.voltMode_ = outer_.voltModeDefault_ = mode;
}

void AMD::PMFreqVoltXMLParser::Initializer::takePMFreqVoltStates(
    std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                           units::voltage::millivolt_t>> const &states)
{
  outer_.states_ = outer_.statesDefault_ = states;
}

void AMD::PMFreqVoltXMLParser::Initializer::takePMFreqVoltActiveStates(
    std::vector<unsigned int> const &states)
{
  outer_.activeStates_ = outer_.activeStatesDefault_ = states;
}

AMD::PMFreqVoltXMLParser::PMFreqVoltXMLParser() noexcept
: ProfilePartXMLParser(AMD::PMFreqVolt::ItemID, *this, *this)
{
}

std::string const &AMD::PMFreqVoltXMLParser::instanceID() const
{
  return controlName_;
}

std::unique_ptr<Exportable::Exporter>
AMD::PMFreqVoltXMLParser::factory(IProfilePartXMLParserProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> AMD::PMFreqVoltXMLParser::initializer()
{
  return std::make_unique<AMD::PMFreqVoltXMLParser::Initializer>(*this);
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
AMD::PMFreqVoltXMLParser::provideExporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMFreqVoltXMLParser::provideImporter(Item const &)
{
  return {};
}

void AMD::PMFreqVoltXMLParser::takeActive(bool active)
{
  active_ = active;
}

bool AMD::PMFreqVoltXMLParser::provideActive() const
{
  return active_;
}

void AMD::PMFreqVoltXMLParser::takePMFreqVoltControlName(std::string const &)
{
}

void AMD::PMFreqVoltXMLParser::takePMFreqVoltVoltMode(std::string const &mode)
{
  voltMode_ = mode;
}

void AMD::PMFreqVoltXMLParser::takePMFreqVoltStates(
    std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                           units::voltage::millivolt_t>> const &states)
{
  states_ = states;
}

void AMD::PMFreqVoltXMLParser::takePMFreqVoltActiveStates(
    std::vector<unsigned int> const &states)
{
  activeStates_ = states;
}

std::string const &AMD::PMFreqVoltXMLParser::providePMFreqVoltVoltMode() const
{
  return voltMode_;
}

std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
AMD::PMFreqVoltXMLParser::providePMFreqVoltState(unsigned int index) const
{
  auto stateIt = std::find_if(states_.cbegin(), states_.cend(), [=](auto &state) {
    return std::get<0>(state) == index;
  });
  if (stateIt != states_.cend())
    return std::make_pair(std::get<1>(*stateIt), std::get<2>(*stateIt));
  else
    return std::make_pair(units::frequency::megahertz_t(0),
                          units::voltage::millivolt_t(0));
}

std::vector<unsigned int>
AMD::PMFreqVoltXMLParser::providePMFreqVoltActiveStates() const
{
  return activeStates_;
}

void AMD::PMFreqVoltXMLParser::appendTo(pugi::xml_node &parentNode)
{
  auto node = parentNode.append_child(ID().c_str());

  node.append_attribute("active") = active_;
  node.append_attribute("id") = nodeID_.c_str();
  node.append_attribute("voltMode") = voltMode_.c_str();
  saveStates(node);
}

void AMD::PMFreqVoltXMLParser::resetAttributes()
{
  active_ = activeDefault_;
  voltMode_ = voltModeDefault_;
  states_ = statesDefault_;
  activeStates_ = activeStatesDefault_;
}

void AMD::PMFreqVoltXMLParser::loadPartFrom(pugi::xml_node const &parentNode)
{
  auto legacyNode = parentNode.find_child([&](pugi::xml_node const &node) {
    return node.name() == AMD::PMOverdrive::LegacyFVStateItemID;
  });
  if (!legacyNode) {
    auto node = parentNode.find_child([&](pugi::xml_node const &node) {
      if (node.name() != ID())
        return false;

      return node.attribute("id").as_string() == nodeID_;
    });

    active_ = node.attribute("active").as_bool(activeDefault_);
    voltMode_ = node.attribute("voltMode").as_string(voltModeDefault_.c_str());
    loadStates(node);
  }
  else {
    active_ = legacyNode.attribute("active").as_bool(activeDefault_);
    loadStatesFromLegacyNode(legacyNode);
  }
}

void AMD::PMFreqVoltXMLParser::saveStates(pugi::xml_node &node) const
{
  for (auto [index, freq, volt] : states_) {
    auto stateNode = node.append_child(StateNodeName.data());
    auto activeIt = std::find(activeStates_.cbegin(), activeStates_.cend(),
                              index);
    stateNode.append_attribute("active") = activeIt != activeStates_.cend();
    stateNode.append_attribute("index") = index;
    stateNode.append_attribute("freq") = freq.to<unsigned int>();
    stateNode.append_attribute("volt") = volt.to<unsigned int>();
  }
}

void AMD::PMFreqVoltXMLParser::loadStates(pugi::xml_node const &node)
{
  if (!node) {
    states_ = statesDefault_;
    activeStates_ = activeStatesDefault_;
  }
  else {
    states_.clear();
    activeStates_.clear();

    for (auto stateNode : node.children(StateNodeName.data())) {
      auto activeAttr = stateNode.attribute("active");
      auto indexAttr = stateNode.attribute("index");
      auto freqAttr = stateNode.attribute("freq");
      auto voltAttr = stateNode.attribute("volt");

      if (activeAttr && indexAttr && freqAttr && voltAttr) {
        auto index = indexAttr.as_uint();
        auto indexIt = std::find_if(
            statesDefault_.cbegin(), statesDefault_.cend(),
            [=](auto &state) { return std::get<0>(state) == index; });
        if (indexIt == statesDefault_.cend()) // unknown index
          break;

        auto freq = freqAttr.as_uint();
        auto volt = voltAttr.as_uint();
        states_.emplace_back(index, units::frequency::megahertz_t(freq),
                             units::voltage::millivolt_t(volt));

        if (activeAttr.as_bool())
          activeStates_.emplace_back(index);
      }
      else // malformed data
        break;
    }

    if (states_.size() != statesDefault_.size()) {
      states_ = statesDefault_;
      activeStates_ = activeStatesDefault_;
    }
    else if (activeStates_.size() > statesDefault_.size())
      activeStates_ = activeStatesDefault_;
  }
}

void AMD::PMFreqVoltXMLParser::loadStatesFromLegacyNode(pugi::xml_node const &node)
{
  auto statesNode = node.find_child([&](pugi::xml_node const &node) {
    // match states node
    if (node.name() != LegacyStatesNodeName)
      return false;

    // match state
    return node.attribute("id").as_string() == nodeID_;
  });
  voltMode_ =
      statesNode.attribute("voltMode").as_string(voltModeDefault_.c_str());
  loadStates(statesNode);
}

bool const AMD::PMFreqVoltXMLParser::registered_ =
    ProfilePartXMLParserProvider::registerProvider(AMD::PMFreqVolt::ItemID, []() {
      return std::make_unique<AMD::PMFreqVoltXMLParser>();
    });
