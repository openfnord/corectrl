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
#include "pmfvvoltcurvexmlparser.h"

#include "core/profilepartxmlparserprovider.h"
#include "pmfvvoltcurve.h"
#include <algorithm>
#include <memory>

class AMD::PMFVVoltCurveXMLParser::Initializer final
: public AMD::PMFVVoltCurveProfilePart::Exporter
{
 public:
  Initializer(AMD::PMFVVoltCurveXMLParser &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takeActive(bool active) override;
  void takePMFVVoltCurveVoltMode(std::string const &mode) override;
  void takePMFVVoltCurveVoltCurve(
      std::vector<std::pair<units::frequency::megahertz_t,
                            units::voltage::millivolt_t>> const &curve) override;
  void takePMFVVoltCurveGPUStates(
      std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
          &states) override;
  void takePMFVVoltCurveMemStates(
      std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
          &states) override;

 private:
  AMD::PMFVVoltCurveXMLParser &outer_;
};

void AMD::PMFVVoltCurveXMLParser::Initializer::takeActive(bool active)
{
  outer_.active_ = outer_.activeDefault_ = active;
}

void AMD::PMFVVoltCurveXMLParser::Initializer::takePMFVVoltCurveVoltMode(
    std::string const &mode)
{
  outer_.voltMode_ = outer_.voltModeDefault_ = mode;
}

void AMD::PMFVVoltCurveXMLParser::Initializer::takePMFVVoltCurveVoltCurve(
    std::vector<std::pair<units::frequency::megahertz_t,
                          units::voltage::millivolt_t>> const &curve)
{
  outer_.voltCurve_ = outer_.voltCurveDefault_ = curve;
}

void AMD::PMFVVoltCurveXMLParser::Initializer::takePMFVVoltCurveGPUStates(
    std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const &states)
{
  outer_.gpuStates_ = outer_.gpuStatesDefault_ = states;
}

void AMD::PMFVVoltCurveXMLParser::Initializer::takePMFVVoltCurveMemStates(
    std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const &states)
{
  outer_.memStates_ = outer_.memStatesDefault_ = states;
}

AMD::PMFVVoltCurveXMLParser::PMFVVoltCurveXMLParser() noexcept
: ProfilePartXMLParser(*this, *this)
, id_(AMD::PMFVVoltCurve::ItemID)
{
}

std::unique_ptr<Exportable::Exporter>
AMD::PMFVVoltCurveXMLParser::factory(IProfilePartXMLParserProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> AMD::PMFVVoltCurveXMLParser::initializer()
{
  return std::make_unique<AMD::PMFVVoltCurveXMLParser::Initializer>(*this);
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
AMD::PMFVVoltCurveXMLParser::provideExporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMFVVoltCurveXMLParser::provideImporter(Item const &)
{
  return {};
}

void AMD::PMFVVoltCurveXMLParser::takeActive(bool active)
{
  active_ = active;
}

bool AMD::PMFVVoltCurveXMLParser::provideActive() const
{
  return active_;
}

void AMD::PMFVVoltCurveXMLParser::takePMFVVoltCurveVoltMode(std::string const &mode)
{
  voltMode_ = mode;
}

std::string const &AMD::PMFVVoltCurveXMLParser::providePMFVVoltCurveVoltMode() const
{
  return voltMode_;
}

void AMD::PMFVVoltCurveXMLParser::takePMFVVoltCurveVoltCurve(
    std::vector<std::pair<units::frequency::megahertz_t,
                          units::voltage::millivolt_t>> const &curve)
{
  voltCurve_ = curve;
}

std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
AMD::PMFVVoltCurveXMLParser::providePMFVVoltCurveVoltCurvePoint(
    unsigned int index) const
{
  if (index < voltCurve_.size())
    return voltCurve_.at(index);
  else
    return std::make_pair(units::frequency::megahertz_t(0),
                          units::voltage::millivolt_t(0));
}

void AMD::PMFVVoltCurveXMLParser::takePMFVVoltCurveGPUStates(
    std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const &states)
{
  gpuStates_ = states;
}

units::frequency::megahertz_t
AMD::PMFVVoltCurveXMLParser::providePMFVVoltCurveGPUState(unsigned int index) const
{
  auto stateIt = std::find_if(gpuStates_.cbegin(), gpuStates_.cend(),
                              [=](auto &state) { return state.first == index; });
  if (stateIt != gpuStates_.cend())
    return stateIt->second;
  else
    return units::frequency::megahertz_t(0);
}

void AMD::PMFVVoltCurveXMLParser::takePMFVVoltCurveMemStates(
    std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const &states)
{
  memStates_ = states;
}

units::frequency::megahertz_t
AMD::PMFVVoltCurveXMLParser::providePMFVVoltCurveMemState(unsigned int index) const
{
  auto stateIt = std::find_if(memStates_.cbegin(), memStates_.cend(),
                              [=](auto &state) { return state.first == index; });
  if (stateIt != memStates_.cend())
    return stateIt->second;
  else
    return units::frequency::megahertz_t(0);
}

void AMD::PMFVVoltCurveXMLParser::appendTo(pugi::xml_node &parentNode)
{
  auto pmFVVoltCurveNode = parentNode.append_child(id_.c_str());

  pmFVVoltCurveNode.append_attribute("active") = active_;
  pmFVVoltCurveNode.append_attribute("voltMode") = voltMode_.data();

  saveVoltCurve(pmFVVoltCurveNode);
  saveStates(pmFVVoltCurveNode, GPUStateNodeId, gpuStates_);
  saveStates(pmFVVoltCurveNode, MemStateNodeId, memStates_);
}

void AMD::PMFVVoltCurveXMLParser::resetAttributes()
{
  active_ = activeDefault_;
  voltMode_ = voltModeDefault_;
  voltCurve_ = voltCurveDefault_;
  gpuStates_ = gpuStatesDefault_;
  memStates_ = memStatesDefault_;
}

void AMD::PMFVVoltCurveXMLParser::loadPartFrom(pugi::xml_node const &parentNode)
{
  auto pmFVVoltCurveNode = parentNode.find_child(
      [&](pugi::xml_node const &node) { return node.name() == id_; });

  active_ = pmFVVoltCurveNode.attribute("active").as_bool(activeDefault_);
  voltMode_ =
      pmFVVoltCurveNode.attribute("voltMode").as_string(voltModeDefault_.data());

  loadVoltCurve(pmFVVoltCurveNode);
  loadStates(pmFVVoltCurveNode, GPUStateNodeId, gpuStates_, gpuStatesDefault_);
  loadStates(pmFVVoltCurveNode, MemStateNodeId, memStates_, memStatesDefault_);
}

void AMD::PMFVVoltCurveXMLParser::saveVoltCurve(pugi::xml_node &fvCurveVoltNode) const
{
  auto voltCurveNode = fvCurveVoltNode.append_child(VoltCurveNodeName.data());
  for (auto [freq, volt] : voltCurve_) {
    auto pointNode = voltCurveNode.append_child(VoltCurvePointNodeName.data());
    pointNode.append_attribute("freq") = freq.to<unsigned int>();
    pointNode.append_attribute("volt") = volt.to<unsigned int>();
  }
}

void AMD::PMFVVoltCurveXMLParser::loadVoltCurve(pugi::xml_node &fvCurveVoltNode)
{
  auto curveVoltNode = fvCurveVoltNode.find_child([&](pugi::xml_node const &node) {
    return node.name() == VoltCurveNodeName;
  });

  if (!curveVoltNode)
    voltCurve_ = voltCurveDefault_;
  else {
    voltCurve_.clear();

    for (auto pointNode : curveVoltNode.children(VoltCurvePointNodeName.data())) {
      auto freqAttr = pointNode.attribute("freq");
      auto voltAttr = pointNode.attribute("volt");

      if (freqAttr && voltAttr) {
        auto freq = freqAttr.as_uint();
        auto volt = voltAttr.as_uint();
        voltCurve_.emplace_back(units::frequency::megahertz_t(freq),
                                units::voltage::millivolt_t(volt));
      }
      else // malformed data
        break;
    }

    if (voltCurve_.size() != voltCurveDefault_.size())
      voltCurve_ = voltCurveDefault_;
  }
}

void AMD::PMFVVoltCurveXMLParser::saveStates(
    pugi::xml_node &fvCurveVoltNode, std::string_view stateNodeId,
    std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> &states) const
{
  auto statesNode = fvCurveVoltNode.append_child(StatesNodeName.data());
  statesNode.append_attribute("id") = stateNodeId.data();

  for (auto [index, freq] : states) {
    auto stateNode = statesNode.append_child(StateNodeName.data());
    stateNode.append_attribute("index") = index;
    stateNode.append_attribute("freq") = freq.to<unsigned int>();
  }
}

void AMD::PMFVVoltCurveXMLParser::loadStates(
    pugi::xml_node const &fvCurveVoltNode, std::string_view stateNodeId,
    std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> &states,
    std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
        &statesDefault) const
{
  auto statesNode = fvCurveVoltNode.find_child([&](pugi::xml_node const &node) {
    // match states node
    if (node.name() != StatesNodeName)
      return false;

    // match state
    return node.attribute("id").as_string() == stateNodeId;
  });

  if (!statesNode) {
    states = statesDefault;
  }
  else {
    states.clear();

    for (auto stateNode : statesNode.children(StateNodeName.data())) {
      auto indexAttr = stateNode.attribute("index");
      auto freqAttr = stateNode.attribute("freq");

      if (indexAttr && freqAttr) {
        auto index = indexAttr.as_uint();
        auto indexIt = std::find_if(
            statesDefault.cbegin(), statesDefault.cend(),
            [=](auto &state) { return state.first == index; });
        if (indexIt == statesDefault.cend()) // unknown index
          break;

        auto freq = freqAttr.as_uint();
        states.emplace_back(index, units::frequency::megahertz_t(freq));
      }
      else // malformed data
        break;
    }

    if (states.size() != statesDefault.size()) {
      states = statesDefault;
    }
  }
}

bool const AMD::PMFVVoltCurveXMLParser::registered_ =
    ProfilePartXMLParserProvider::registerProvider(
        AMD::PMFVVoltCurve::ItemID,
        []() { return std::make_unique<AMD::PMFVVoltCurveXMLParser>(); });
