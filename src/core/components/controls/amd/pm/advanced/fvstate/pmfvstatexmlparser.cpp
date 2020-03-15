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
#include "pmfvstatexmlparser.h"

#include "core/profilepartxmlparserprovider.h"
#include "pmfvstate.h"
#include <algorithm>
#include <memory>

class AMD::PMFVStateXMLParser::Initializer final
: public AMD::PMFVStateProfilePart::Exporter
{
 public:
  Initializer(AMD::PMFVStateXMLParser &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takeActive(bool active) override;
  void takePMFVStateGPUVoltMode(std::string const &mode) override;
  void takePMFVStateMemVoltMode(std::string const &mode) override;
  void takePMFVStateGPUStates(
      std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                             units::voltage::millivolt_t>> const &states) override;
  void takePMFVStateMemStates(
      std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                             units::voltage::millivolt_t>> const &states) override;
  void
  takePMFVStateGPUActiveStates(std::vector<unsigned int> const &indices) override;
  void
  takePMFVStateMemActiveStates(std::vector<unsigned int> const &indices) override;

 private:
  AMD::PMFVStateXMLParser &outer_;
};

void AMD::PMFVStateXMLParser::Initializer::takeActive(bool active)
{
  outer_.active_ = outer_.activeDefault_ = active;
}

void AMD::PMFVStateXMLParser::Initializer::takePMFVStateGPUVoltMode(
    std::string const &mode)
{
  outer_.gpuVoltMode_ = outer_.gpuVoltModeDefault_ = mode;
}

void AMD::PMFVStateXMLParser::Initializer::takePMFVStateMemVoltMode(
    std::string const &mode)
{
  outer_.memVoltMode_ = outer_.memVoltModeDefault_ = mode;
}

void AMD::PMFVStateXMLParser::Initializer::takePMFVStateGPUStates(
    std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                           units::voltage::millivolt_t>> const &states)
{
  outer_.gpuStates_ = outer_.gpuStatesDefault_ = states;
}

void AMD::PMFVStateXMLParser::Initializer::takePMFVStateMemStates(
    std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                           units::voltage::millivolt_t>> const &states)
{
  outer_.memStates_ = outer_.memStatesDefault_ = states;
}

void AMD::PMFVStateXMLParser::Initializer::takePMFVStateGPUActiveStates(
    std::vector<unsigned int> const &indices)
{
  outer_.gpuActiveStates_ = outer_.gpuActiveStatesDefault_ = indices;
}

void AMD::PMFVStateXMLParser::Initializer::takePMFVStateMemActiveStates(
    std::vector<unsigned int> const &indices)
{
  outer_.memActiveStates_ = outer_.memActiveStatesDefault_ = indices;
}

AMD::PMFVStateXMLParser::PMFVStateXMLParser() noexcept
: ProfilePartXMLParser(*this, *this)
, id_(AMD::PMFVState::ItemID)
{
}

std::unique_ptr<Exportable::Exporter>
AMD::PMFVStateXMLParser::factory(IProfilePartXMLParserProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> AMD::PMFVStateXMLParser::initializer()
{
  return std::make_unique<AMD::PMFVStateXMLParser::Initializer>(*this);
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
AMD::PMFVStateXMLParser::provideExporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMFVStateXMLParser::provideImporter(Item const &)
{
  return {};
}

void AMD::PMFVStateXMLParser::takeActive(bool active)
{
  active_ = active;
}

bool AMD::PMFVStateXMLParser::provideActive() const
{
  return active_;
}

void AMD::PMFVStateXMLParser::takePMFVStateGPUVoltMode(std::string const &mode)
{
  gpuVoltMode_ = mode;
}

std::string const &AMD::PMFVStateXMLParser::providePMFVStateGPUVoltMode() const
{
  return gpuVoltMode_;
}

void AMD::PMFVStateXMLParser::takePMFVStateMemVoltMode(std::string const &mode)
{
  memVoltMode_ = mode;
}

std::string const &AMD::PMFVStateXMLParser::providePMFVStateMemVoltMode() const
{
  return memVoltMode_;
}

void AMD::PMFVStateXMLParser::takePMFVStateGPUStates(
    std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                           units::voltage::millivolt_t>> const &states)
{
  gpuStates_ = states;
}

std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
AMD::PMFVStateXMLParser::providePMFVStateGPUState(unsigned int index) const
{
  auto stateIt = std::find_if(
      gpuStates_.cbegin(), gpuStates_.cend(),
      [=](auto &state) { return std::get<0>(state) == index; });
  if (stateIt != gpuStates_.cend())
    return std::make_pair(std::get<1>(*stateIt), std::get<2>(*stateIt));
  else
    return std::make_pair(units::frequency::megahertz_t(0),
                          units::voltage::millivolt_t(0));
}

void AMD::PMFVStateXMLParser::takePMFVStateMemStates(
    std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                           units::voltage::millivolt_t>> const &states)
{
  memStates_ = states;
}

std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
AMD::PMFVStateXMLParser::providePMFVStateMemState(unsigned int index) const
{
  auto stateIt = std::find_if(
      memStates_.cbegin(), memStates_.cend(),
      [=](auto &state) { return std::get<0>(state) == index; });
  if (stateIt != memStates_.cend())
    return std::make_pair(std::get<1>(*stateIt), std::get<2>(*stateIt));
  else
    return std::make_pair(units::frequency::megahertz_t(0),
                          units::voltage::millivolt_t(0));
}

void AMD::PMFVStateXMLParser::takePMFVStateGPUActiveStates(
    std::vector<unsigned int> const &indices)
{
  gpuActiveStates_ = indices;
}

std::vector<unsigned int>
AMD::PMFVStateXMLParser::providePMFVStateGPUActiveStates() const
{
  return gpuActiveStates_;
}

void AMD::PMFVStateXMLParser::takePMFVStateMemActiveStates(
    std::vector<unsigned int> const &indices)
{
  memActiveStates_ = indices;
}

std::vector<unsigned int>
AMD::PMFVStateXMLParser::providePMFVStateMemActiveStates() const
{
  return memActiveStates_;
}

void AMD::PMFVStateXMLParser::appendTo(pugi::xml_node &parentNode)
{
  auto pmFVStateNode = parentNode.append_child(id_.c_str());

  pmFVStateNode.append_attribute("active") = active_;
  saveStates(pmFVStateNode, GPUStateNodeId, gpuVoltMode_, gpuStates_,
             gpuActiveStates_);
  saveStates(pmFVStateNode, MemStateNodeId, memVoltMode_, memStates_,
             memActiveStates_);
}

void AMD::PMFVStateXMLParser::resetAttributes()
{
  active_ = activeDefault_;
  gpuVoltMode_ = gpuVoltModeDefault_;
  memVoltMode_ = memVoltModeDefault_;
  gpuStates_ = gpuStatesDefault_;
  memStates_ = memStatesDefault_;
  gpuActiveStates_ = gpuActiveStatesDefault_;
  memActiveStates_ = memActiveStatesDefault_;
}

void AMD::PMFVStateXMLParser::loadPartFrom(pugi::xml_node const &parentNode)
{
  auto pmFVStateNode = parentNode.find_child(
      [&](pugi::xml_node const &node) { return node.name() == id_; });

  active_ = pmFVStateNode.attribute("active").as_bool(activeDefault_);
  loadStates(pmFVStateNode, GPUStateNodeId, gpuVoltMode_, gpuVoltModeDefault_,
             gpuStates_, gpuStatesDefault_, gpuActiveStates_,
             gpuActiveStatesDefault_);
  loadStates(pmFVStateNode, MemStateNodeId, memVoltMode_, memVoltModeDefault_,
             memStates_, memStatesDefault_, memActiveStates_,
             memActiveStatesDefault_);
}

void AMD::PMFVStateXMLParser::saveStates(
    pugi::xml_node &fvStateNode, std::string_view stateNodeId,
    std::string const &voltMode,
    std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                           units::voltage::millivolt_t>> &states,
    std::vector<unsigned int> &activeStates) const
{
  auto statesNode = fvStateNode.append_child(StatesNodeName.data());
  statesNode.append_attribute("id") = stateNodeId.data();
  statesNode.append_attribute("voltMode") = voltMode.data();

  for (auto [index, freq, volt] : states) {
    auto stateNode = statesNode.append_child(StateNodeName.data());
    auto activeIt = std::find(activeStates.cbegin(), activeStates.cend(), index);
    stateNode.append_attribute("active") = activeIt != activeStates.cend();
    stateNode.append_attribute("index") = index;
    stateNode.append_attribute("freq") = freq.to<unsigned int>();
    stateNode.append_attribute("volt") = volt.to<unsigned int>();
  }
}

void AMD::PMFVStateXMLParser::loadStates(
    pugi::xml_node const &fvStateNode, std::string_view stateNodeId,
    std::string &voltMode, std::string const &voltModeDefault,
    std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                           units::voltage::millivolt_t>> &states,
    std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                           units::voltage::millivolt_t>> const &statesDefault,
    std::vector<unsigned int> &activeStates,
    std::vector<unsigned int> const &activeStatesDefault) const
{
  auto statesNode = fvStateNode.find_child([&](pugi::xml_node const &node) {
    // match states node
    if (node.name() != StatesNodeName)
      return false;

    // match state
    return node.attribute("id").as_string() == stateNodeId;
  });

  if (!statesNode) {
    states = statesDefault;
    activeStates = activeStatesDefault;
  }
  else {
    voltMode = statesNode.attribute("voltMode").as_string(voltModeDefault.data());

    states.clear();
    activeStates.clear();

    for (auto stateNode : statesNode.children(StateNodeName.data())) {
      auto activeAttr = stateNode.attribute("active");
      auto indexAttr = stateNode.attribute("index");
      auto freqAttr = stateNode.attribute("freq");
      auto voltAttr = stateNode.attribute("volt");

      if (activeAttr && indexAttr && freqAttr && voltAttr) {
        auto index = indexAttr.as_uint();
        auto indexIt = std::find_if(
            statesDefault.cbegin(), statesDefault.cend(),
            [=](auto &state) { return std::get<0>(state) == index; });
        if (indexIt == statesDefault.cend()) // unknown index
          break;

        auto freq = freqAttr.as_uint();
        auto volt = voltAttr.as_uint();
        states.emplace_back(index, units::frequency::megahertz_t(freq),
                            units::voltage::millivolt_t(volt));
        if (activeAttr.as_bool())
          activeStates.emplace_back(index);
      }
      else // malformed data
        break;
    }

    if (states.size() != statesDefault.size()) {
      states = statesDefault;
      activeStates = activeStatesDefault;
    }
    else if (activeStates.size() > statesDefault.size())
      activeStates = activeStatesDefault;
  }
}

bool const AMD::PMFVStateXMLParser::registered_ =
    ProfilePartXMLParserProvider::registerProvider(AMD::PMFVState::ItemID, []() {
      return std::make_unique<AMD::PMFVStateXMLParser>();
    });
