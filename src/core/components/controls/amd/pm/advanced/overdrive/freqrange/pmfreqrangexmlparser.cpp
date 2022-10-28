// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmfreqrangexmlparser.h"

#include "../pmoverdrive.h"
#include "core/profilepartxmlparserprovider.h"
#include "pmfreqrange.h"
#include <algorithm>
#include <memory>
#include <set>

class AMD::PMFreqRangeXMLParser::Initializer final
: public AMD::PMFreqRangeProfilePart::Exporter
{
 public:
  Initializer(AMD::PMFreqRangeXMLParser &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takeActive(bool active) override;
  void takePMFreqRangeControlName(std::string const &name) override;
  void takePMFreqRangeStates(
      std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
          &states) override;

 private:
  AMD::PMFreqRangeXMLParser &outer_;
};

void AMD::PMFreqRangeXMLParser::Initializer::takeActive(bool active)
{
  outer_.active_ = outer_.activeDefault_ = active;
}

void AMD::PMFreqRangeXMLParser::Initializer::takePMFreqRangeControlName(
    std::string const &name)
{
  outer_.controlName_ = name;
  outer_.nodeID_ = name;
  std::transform(outer_.nodeID_.cbegin(), outer_.nodeID_.cend(),
                 outer_.nodeID_.begin(), ::tolower);
}

void AMD::PMFreqRangeXMLParser::Initializer::takePMFreqRangeStates(
    std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const &states)
{
  outer_.states_ = outer_.statesDefault_ = states;
}

AMD::PMFreqRangeXMLParser::PMFreqRangeXMLParser() noexcept
: ProfilePartXMLParser(AMD::PMFreqRange::ItemID, *this, *this)
{
}

std::string const &AMD::PMFreqRangeXMLParser::instanceID() const
{
  return controlName_;
}

std::unique_ptr<Exportable::Exporter>
AMD::PMFreqRangeXMLParser::factory(IProfilePartXMLParserProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> AMD::PMFreqRangeXMLParser::initializer()
{
  return std::make_unique<AMD::PMFreqRangeXMLParser::Initializer>(*this);
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
AMD::PMFreqRangeXMLParser::provideExporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMFreqRangeXMLParser::provideImporter(Item const &)
{
  return {};
}

void AMD::PMFreqRangeXMLParser::takeActive(bool active)
{
  active_ = active;
}

bool AMD::PMFreqRangeXMLParser::provideActive() const
{
  return active_;
}

void AMD::PMFreqRangeXMLParser::takePMFreqRangeControlName(std::string const &)
{
}

void AMD::PMFreqRangeXMLParser::takePMFreqRangeStates(
    std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const &states)
{
  states_ = states;
}

units::frequency::megahertz_t
AMD::PMFreqRangeXMLParser::providePMFreqRangeState(unsigned int index) const
{
  auto stateIt = std::find_if(states_.cbegin(), states_.cend(),
                              [=](auto &state) { return state.first == index; });
  if (stateIt != states_.cend())
    return stateIt->second;
  else
    return units::frequency::megahertz_t(0);
}

void AMD::PMFreqRangeXMLParser::appendTo(pugi::xml_node &parentNode)
{
  auto node = parentNode.append_child(ID().c_str());

  node.append_attribute("active") = active_;
  node.append_attribute("id") = nodeID_.c_str();
  saveStates(node);
}

void AMD::PMFreqRangeXMLParser::resetAttributes()
{
  active_ = activeDefault_;
  states_ = statesDefault_;
}

void AMD::PMFreqRangeXMLParser::loadPartFrom(pugi::xml_node const &parentNode)
{
  auto legacyNode = parentNode.find_child([&](pugi::xml_node const &node) {
    return node.name() == AMD::PMOverdrive::LegacyFVVoltCurveItemID;
  });
  if (!legacyNode) {
    auto node = parentNode.find_child([&](pugi::xml_node const &node) {
      if (node.name() != ID())
        return false;

      return node.attribute("id").as_string() == nodeID_;
    });

    active_ = node.attribute("active").as_bool(activeDefault_);
    loadStates(node);
  }
  else {
    active_ = legacyNode.attribute("active").as_bool(activeDefault_);
    loadStatesFromLegacyNode(legacyNode);
  }
}

void AMD::PMFreqRangeXMLParser::saveStates(pugi::xml_node &node) const
{
  for (auto [index, freq] : states_) {
    auto stateNode = node.append_child(StateNodeName.data());
    stateNode.append_attribute("index") = index;
    stateNode.append_attribute("freq") = freq.to<unsigned int>();
  }
}

void AMD::PMFreqRangeXMLParser::loadStates(pugi::xml_node const &node)
{
  if (!node) {
    states_ = statesDefault_;
  }
  else {
    states_.clear();

    for (auto stateNode : node.children(StateNodeName.data())) {
      auto indexAttr = stateNode.attribute("index");
      auto freqAttr = stateNode.attribute("freq");

      if (indexAttr && freqAttr) {
        auto index = indexAttr.as_uint();
        auto indexIt = std::find_if(
            statesDefault_.cbegin(), statesDefault_.cend(),
            [=](auto &state) { return state.first == index; });
        if (indexIt == statesDefault_.cend())
          continue; // skip unknown index

        auto freq = freqAttr.as_uint();
        states_.emplace_back(index, units::frequency::megahertz_t(freq));
      }
      else
        continue; // skip malformed state data
    }

    if (states_.size() != statesDefault_.size()) {
      // mix loaded states with default states
      std::set<std::pair<unsigned int, units::frequency::megahertz_t>> states(
          statesDefault_.cbegin(), statesDefault_.cend());

      for (auto &state : states_) {
        states.erase(state);
        states.insert(state);
      }

      states_ =
          std::vector<std::pair<unsigned int, units::frequency::megahertz_t>>(
              states.cbegin(), states.cend());
    }
  }
}

void AMD::PMFreqRangeXMLParser::loadStatesFromLegacyNode(pugi::xml_node const &node)
{
  auto statesNode = node.find_child([&](pugi::xml_node const &node) {
    // match states node
    if (node.name() != LegacyStatesNodeName)
      return false;

    // match state
    return node.attribute("id").as_string() == nodeID_;
  });
  loadStates(statesNode);
}

bool const AMD::PMFreqRangeXMLParser::registered_ =
    ProfilePartXMLParserProvider::registerProvider(
        AMD::PMFreqRange::ItemID,
        []() { return std::make_unique<AMD::PMFreqRangeXMLParser>(); });
