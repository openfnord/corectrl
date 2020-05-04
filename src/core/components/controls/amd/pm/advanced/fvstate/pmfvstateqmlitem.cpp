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
#include "pmfvstateqmlitem.h"

#include "core/qmlcomponentregistry.h"
#include "pmfvstate.h"
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QString>
#include <QtQml>
#include <algorithm>
#include <iterator>
#include <memory>

class AMD::PMFVStateQMLItem::Initializer final
: public QMLItem::Initializer
, public AMD::PMFVState::Exporter
{
 public:
  Initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine,
              AMD::PMFVStateQMLItem &qmlItem) noexcept
  : QMLItem::Initializer(qmlComponentFactory, qmlEngine)
  , outer_(qmlItem)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takeActive(bool active) override;
  void takePMFVStateVoltRange(units::voltage::millivolt_t min,
                              units::voltage::millivolt_t max) override;

  void takePMFVStateGPURange(units::frequency::megahertz_t min,
                             units::frequency::megahertz_t max) override;

  void takePMFVStateMemRange(units::frequency::megahertz_t min,
                             units::frequency::megahertz_t max) override;
  void takePMFVStateGPUVoltMode(std::string const &mode) override;
  void takePMFVStateMemVoltMode(std::string const &mode) override;
  void takePMFVStateVoltModes(std::vector<std::string> const &modes) override;
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
  AMD::PMFVStateQMLItem &outer_;
};

void AMD::PMFVStateQMLItem::Initializer::takeActive(bool active)
{
  outer_.takeActive(active);
}

void AMD::PMFVStateQMLItem::Initializer::takePMFVStateVoltRange(
    units::voltage::millivolt_t min, units::voltage::millivolt_t max)
{
  outer_.voltRange(min, max);
}

void AMD::PMFVStateQMLItem::Initializer::takePMFVStateGPURange(
    units::frequency::megahertz_t min, units::frequency::megahertz_t max)
{
  outer_.gpuRange(min, max);
}

void AMD::PMFVStateQMLItem::Initializer::takePMFVStateMemRange(
    units::frequency::megahertz_t min, units::frequency::megahertz_t max)
{
  outer_.memRange(min, max);
}

void AMD::PMFVStateQMLItem::Initializer::takePMFVStateGPUVoltMode(
    std::string const &mode)
{
  outer_.takePMFVStateGPUVoltMode(mode);
}

void AMD::PMFVStateQMLItem::Initializer::takePMFVStateMemVoltMode(
    std::string const &mode)
{
  outer_.takePMFVStateMemVoltMode(mode);
}

void AMD::PMFVStateQMLItem::Initializer::takePMFVStateVoltModes(
    std::vector<std::string> const &modes)
{
  outer_.voltModes(modes);
}

void AMD::PMFVStateQMLItem::Initializer::takePMFVStateGPUStates(
    std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                           units::voltage::millivolt_t>> const &states)
{
  outer_.takePMFVStateGPUStates(states);
}

void AMD::PMFVStateQMLItem::Initializer::takePMFVStateMemStates(
    std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                           units::voltage::millivolt_t>> const &states)
{
  outer_.takePMFVStateMemStates(states);
}

void AMD::PMFVStateQMLItem::Initializer::takePMFVStateGPUActiveStates(
    std::vector<unsigned int> const &indices)
{
  outer_.takePMFVStateGPUActiveStates(indices);
}

void AMD::PMFVStateQMLItem::Initializer::takePMFVStateMemActiveStates(
    std::vector<unsigned int> const &indices)
{
  outer_.takePMFVStateMemActiveStates(indices);
}

AMD::PMFVStateQMLItem::PMFVStateQMLItem() noexcept
{
  setName(tr(AMD::PMFVState::ItemID.data()));
}

void AMD::PMFVStateQMLItem::changeGPUVoltMode(const QString &mode)
{
  auto newMode = mode.toStdString();
  if (gpuVoltMode_ != newMode) {
    gpuVoltMode_ = newMode;

    emit gpuVoltModeChanged(mode);
    emit settingsChanged();
  }
}

void AMD::PMFVStateQMLItem::changeMemVoltMode(const QString &mode)
{
  auto newMode = mode.toStdString();
  if (memVoltMode_ != newMode) {
    memVoltMode_ = newMode;

    emit memVoltModeChanged(mode);
    emit settingsChanged();
  }
}

void AMD::PMFVStateQMLItem::changeGPUState(int index, int freq, int volt)
{
  if (gpuStates_.count(static_cast<unsigned int>(index))) {
    auto &[stateFreq,
           stateVolt] = gpuStates_.at(static_cast<unsigned int>(index));
    if (stateFreq.to<int>() != freq || stateVolt.to<int>() != volt) {
      stateFreq = units::frequency::megahertz_t(freq);
      stateVolt = units::voltage::millivolt_t(volt);

      emit gpuStateChanged(index, freq, volt);
      emit settingsChanged();
    }
  }
}

void AMD::PMFVStateQMLItem::changeMemState(int index, int freq, int volt)
{
  if (memStates_.count(static_cast<unsigned int>(index))) {
    auto &[stateFreq,
           stateVolt] = memStates_.at(static_cast<unsigned int>(index));
    if (stateFreq.to<int>() != freq || stateVolt.to<int>() != volt) {
      stateFreq = units::frequency::megahertz_t(freq);
      stateVolt = units::voltage::millivolt_t(volt);

      emit memStateChanged(index, freq, volt);
      emit settingsChanged();
    }
  }
}

void AMD::PMFVStateQMLItem::changeGPUActiveState(int index, bool activate)
{
  if (gpuStates_.count(static_cast<unsigned int>(index))) {
    if (activate) {
      auto activeIt = std::find(gpuActiveStates_.cbegin(),
                                gpuActiveStates_.cend(), index);
      if (activeIt == gpuActiveStates_.cend()) {
        gpuActiveStates_.push_back(static_cast<unsigned int>(index));

        emit gpuActiveStatesChanged(activeStatesIndices(gpuActiveStates_));
        emit settingsChanged();
      }
    }
    else {
      gpuActiveStates_.erase(std::remove(gpuActiveStates_.begin(),
                                         gpuActiveStates_.end(),
                                         static_cast<unsigned int>(index)),
                             gpuActiveStates_.end());

      emit gpuActiveStatesChanged(activeStatesIndices(gpuActiveStates_));
      emit settingsChanged();
    }
  }
}

void AMD::PMFVStateQMLItem::changeMemActiveState(int index, bool activate)
{
  if (memStates_.count(static_cast<unsigned int>(index))) {
    if (activate) {
      auto activeIt = std::find(memActiveStates_.cbegin(),
                                memActiveStates_.cend(), index);
      if (activeIt == memActiveStates_.cend()) {
        memActiveStates_.push_back(static_cast<unsigned int>(index));

        emit memActiveStatesChanged(activeStatesIndices(memActiveStates_));
        emit settingsChanged();
      }
    }
    else {
      memActiveStates_.erase(std::remove(memActiveStates_.begin(),
                                         memActiveStates_.end(),
                                         static_cast<unsigned int>(index)),
                             memActiveStates_.end());

      emit memActiveStatesChanged(activeStatesIndices(memActiveStates_));
      emit settingsChanged();
    }
  }
}

void AMD::PMFVStateQMLItem::activate(bool active)
{
  takeActive(active);
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
AMD::PMFVStateQMLItem::provideExporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMFVStateQMLItem::provideImporter(Item const &)
{
  return {};
}

void AMD::PMFVStateQMLItem::takeActive(bool active)
{
  active_ = active;
  setVisible(active);
}

bool AMD::PMFVStateQMLItem::provideActive() const
{
  return active_;
}

void AMD::PMFVStateQMLItem::takePMFVStateGPUVoltMode(std::string const &mode)
{
  if (gpuVoltMode_ != mode) {
    gpuVoltMode_ = mode;
    emit gpuVoltModeChanged(QString::fromStdString(gpuVoltMode_));
  }
}

std::string const &AMD::PMFVStateQMLItem::providePMFVStateGPUVoltMode() const
{
  return gpuVoltMode_;
}

void AMD::PMFVStateQMLItem::takePMFVStateMemVoltMode(std::string const &mode)
{
  if (memVoltMode_ != mode) {
    memVoltMode_ = mode;
    emit memVoltModeChanged(QString::fromStdString(memVoltMode_));
  }
}

std::string const &AMD::PMFVStateQMLItem::providePMFVStateMemVoltMode() const
{
  return memVoltMode_;
}

void AMD::PMFVStateQMLItem::takePMFVStateGPUStates(
    std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                           units::voltage::millivolt_t>> const &states)
{
  std::map<unsigned int,
           std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>>
      newStates;

  QVariantList statesList;
  statesList.reserve(states.size() * 3);
  for (auto &[index, freq, volt] : states) {
    newStates.emplace(index, std::make_pair(freq, volt));

    statesList.push_back(index);
    statesList.push_back(freq.to<int>());
    statesList.push_back(volt.to<int>());
  }

  if (newStates != gpuStates_) {
    std::swap(gpuStates_, newStates);
    emit gpuStatesChanged(statesList);
  }
}

std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
AMD::PMFVStateQMLItem::providePMFVStateGPUState(unsigned int index) const
{
  if (gpuStates_.count(index) > 0)
    return gpuStates_.at(index);
  else
    return std::make_pair(units::frequency::megahertz_t(0),
                          units::voltage::millivolt_t(0));
}

void AMD::PMFVStateQMLItem::takePMFVStateMemStates(
    std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                           units::voltage::millivolt_t>> const &states)
{
  std::map<unsigned int,
           std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>>
      newStates;

  QVariantList statesList;
  statesList.reserve(states.size() * 3);
  for (auto &[index, freq, volt] : states) {
    newStates.emplace(index, std::make_pair(freq, volt));

    statesList.push_back(index);
    statesList.push_back(freq.to<int>());
    statesList.push_back(volt.to<int>());
  }

  if (newStates != memStates_) {
    std::swap(memStates_, newStates);
    emit memStatesChanged(statesList);
  }
}

std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
AMD::PMFVStateQMLItem::providePMFVStateMemState(unsigned int index) const
{
  if (memStates_.count(index) > 0)
    return memStates_.at(index);
  else
    return std::make_pair(units::frequency::megahertz_t(0),
                          units::voltage::millivolt_t(0));
}

void AMD::PMFVStateQMLItem::takePMFVStateGPUActiveStates(
    std::vector<unsigned int> const &indices)
{
  if (indices != gpuActiveStates_) {
    gpuActiveStates_ = indices;
    emit gpuActiveStatesChanged(activeStatesIndices(indices));
  }
}

std::vector<unsigned int>
AMD::PMFVStateQMLItem::providePMFVStateGPUActiveStates() const
{
  return gpuActiveStates_;
}

void AMD::PMFVStateQMLItem::takePMFVStateMemActiveStates(
    std::vector<unsigned int> const &indices)
{
  if (indices != memActiveStates_) {
    memActiveStates_ = indices;
    emit memActiveStatesChanged(activeStatesIndices(indices));
  }
}

std::vector<unsigned int>
AMD::PMFVStateQMLItem::providePMFVStateMemActiveStates() const
{
  return memActiveStates_;
}

std::unique_ptr<Exportable::Exporter> AMD::PMFVStateQMLItem::initializer(
    IQMLComponentFactory const &qmlComponentFactory,
    QQmlApplicationEngine &qmlEngine)
{
  return std::make_unique<AMD::PMFVStateQMLItem::Initializer>(
      qmlComponentFactory, qmlEngine, *this);
}

void AMD::PMFVStateQMLItem::voltModes(std::vector<std::string> const &)
{
  // XXX emit a signal with the modes if other voltage modes are added
}

void AMD::PMFVStateQMLItem::voltRange(units::voltage::millivolt_t min,
                                      units::voltage::millivolt_t max)
{
  emit voltRangeChanged(min.to<int>(), max.to<int>());
}

void AMD::PMFVStateQMLItem::gpuRange(units::frequency::megahertz_t min,
                                     units::frequency::megahertz_t max)
{
  emit gpuFreqRangeChanged(min.to<int>(), max.to<int>());
}

void AMD::PMFVStateQMLItem::memRange(units::frequency::megahertz_t min,
                                     units::frequency::megahertz_t max)
{
  emit memFreqRangeChanged(min.to<int>(), max.to<int>());
}

QVector<int> AMD::PMFVStateQMLItem::activeStatesIndices(
    std::vector<unsigned int> const &indices) const
{
  QVector<int> states;
  states.reserve(indices.size());
  std::transform(indices.cbegin(), indices.cend(), std::back_inserter(states),
                 [&](unsigned int index) { return static_cast<int>(index); });

  return states;
}

bool AMD::PMFVStateQMLItem::register_()
{
  QMLComponentRegistry::addQMLTypeRegisterer([]() {
    qmlRegisterType<AMD::PMFVStateQMLItem>("Radman.UIComponents", 1, 0,
                                           AMD::PMFVState::ItemID.data());
  });

  QMLComponentRegistry::addQMLItemProvider(
      AMD::PMFVState::ItemID, [](QQmlApplicationEngine &engine) {
        QQmlComponent component(
            &engine, QStringLiteral("qrc:/qml/AMDPMFVStateForm.qml"));
        return qobject_cast<QMLItem *>(component.create());
      });

  return true;
}

bool const AMD::PMFVStateQMLItem::registered_ =
    AMD::PMFVStateQMLItem::register_();
