// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmfreqvoltqmlitem.h"

#include "core/qmlcomponentregistry.h"
#include "pmfreqvolt.h"
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QString>
#include <QtGlobal>
#include <QtQml>
#include <algorithm>
#include <iterator>
#include <memory>

char const *const AMD::PMFreqVoltQMLItem::trStrings[] = {
    QT_TRANSLATE_NOOP("AMD::PMFreqVoltQMLItem", "SCLK"),
    QT_TRANSLATE_NOOP("AMD::PMFreqVoltQMLItem", "MCLK"),
};

class AMD::PMFreqVoltQMLItem::Initializer final
: public QMLItem::Initializer
, public AMD::PMFreqVolt::Exporter
{
 public:
  Initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine,
              AMD::PMFreqVoltQMLItem &qmlItem) noexcept
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
  void takePMFreqVoltControlName(std::string const &name) override;
  void takePMFreqVoltVoltRange(units::voltage::millivolt_t min,
                               units::voltage::millivolt_t max) override;
  void takePMFreqVoltFreqRange(units::frequency::megahertz_t min,
                               units::frequency::megahertz_t max) override;
  void takePMFreqVoltVoltMode(std::string const &mode) override;
  void takePMFreqVoltVoltModes(std::vector<std::string> const &modes) override;
  void takePMFreqVoltStates(
      std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                             units::voltage::millivolt_t>> const &states) override;
  void
  takePMFreqVoltActiveStates(std::vector<unsigned int> const &indices) override;

 private:
  AMD::PMFreqVoltQMLItem &outer_;
};

void AMD::PMFreqVoltQMLItem::Initializer::takeActive(bool active)
{
  outer_.takeActive(active);
}

void AMD::PMFreqVoltQMLItem::Initializer::takePMFreqVoltControlName(
    std::string const &name)
{
  outer_.takePMFreqVoltControlName(name);
}

void AMD::PMFreqVoltQMLItem::Initializer::takePMFreqVoltVoltRange(
    units::voltage::millivolt_t min, units::voltage::millivolt_t max)
{
  outer_.voltRange(min, max);
}

void AMD::PMFreqVoltQMLItem::Initializer::takePMFreqVoltFreqRange(
    units::frequency::megahertz_t min, units::frequency::megahertz_t max)
{
  outer_.freqRange(min, max);
}

void AMD::PMFreqVoltQMLItem::Initializer::takePMFreqVoltVoltMode(
    std::string const &mode)
{
  outer_.takePMFreqVoltVoltMode(mode);
}

void AMD::PMFreqVoltQMLItem::Initializer::takePMFreqVoltVoltModes(
    std::vector<std::string> const &modes)
{
  outer_.voltModes(modes);
}

void AMD::PMFreqVoltQMLItem::Initializer::takePMFreqVoltStates(
    std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                           units::voltage::millivolt_t>> const &states)
{
  outer_.takePMFreqVoltStates(states);
}

void AMD::PMFreqVoltQMLItem::Initializer::takePMFreqVoltActiveStates(
    std::vector<unsigned int> const &indices)
{
  outer_.takePMFreqVoltActiveStates(indices);
}

AMD::PMFreqVoltQMLItem::PMFreqVoltQMLItem() noexcept
{
  setName(tr(AMD::PMFreqVolt::ItemID.data()));
}

void AMD::PMFreqVoltQMLItem::changeVoltMode(const QString &mode)
{
  auto newMode = mode.toStdString();
  if (voltMode_ != newMode) {
    voltMode_ = newMode;

    emit voltModeChanged(mode);
    emit settingsChanged();
  }
}

void AMD::PMFreqVoltQMLItem::changeState(int index, int freq, int volt)
{
  if (states_.count(static_cast<unsigned int>(index))) {
    auto &[stateFreq, stateVolt] = states_.at(static_cast<unsigned int>(index));
    if (stateFreq.to<int>() != freq || stateVolt.to<int>() != volt) {
      stateFreq = units::frequency::megahertz_t(freq);
      stateVolt = units::voltage::millivolt_t(volt);

      emit stateChanged(index, freq, volt);
      emit settingsChanged();
    }
  }
}

void AMD::PMFreqVoltQMLItem::changeActiveState(int index, bool activate)
{
  if (states_.count(static_cast<unsigned int>(index))) {
    if (activate) {
      auto activeIt = std::find(activeStates_.cbegin(), activeStates_.cend(),
                                index);
      if (activeIt == activeStates_.cend()) {
        activeStates_.push_back(static_cast<unsigned int>(index));

        emit activeStatesChanged(activeStatesIndices(activeStates_));
        emit settingsChanged();
      }
    }
    else {
      activeStates_.erase(std::remove(activeStates_.begin(), activeStates_.end(),
                                      static_cast<unsigned int>(index)),
                          activeStates_.end());

      emit activeStatesChanged(activeStatesIndices(activeStates_));
      emit settingsChanged();
    }
  }
}

QString const &AMD::PMFreqVoltQMLItem::instanceID() const
{
  return instanceID_;
}

void AMD::PMFreqVoltQMLItem::activate(bool active)
{
  takeActive(active);
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
AMD::PMFreqVoltQMLItem::provideExporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMFreqVoltQMLItem::provideImporter(Item const &)
{
  return {};
}

void AMD::PMFreqVoltQMLItem::takeActive(bool active)
{
  active_ = active;
  setVisible(active);
}

void AMD::PMFreqVoltQMLItem::takePMFreqVoltControlName(std::string const &name)
{
  controlName(name);
}

void AMD::PMFreqVoltQMLItem::takePMFreqVoltVoltMode(std::string const &mode)
{
  if (voltMode_ != mode) {
    voltMode_ = mode;
    emit voltModeChanged(QString::fromStdString(voltMode_));
  }
}

void AMD::PMFreqVoltQMLItem::takePMFreqVoltStates(
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

  if (newStates != states_) {
    std::swap(states_, newStates);
    emit statesChanged(statesList);
  }
}

void AMD::PMFreqVoltQMLItem::takePMFreqVoltActiveStates(
    std::vector<unsigned int> const &indices)
{
  if (indices != activeStates_) {
    activeStates_ = indices;
    emit activeStatesChanged(activeStatesIndices(indices));
  }
}

bool AMD::PMFreqVoltQMLItem::provideActive() const
{
  return active_;
}

std::string const &AMD::PMFreqVoltQMLItem::providePMFreqVoltVoltMode() const
{
  return voltMode_;
}

std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
AMD::PMFreqVoltQMLItem::providePMFreqVoltState(unsigned int index) const
{
  if (states_.count(index) > 0)
    return states_.at(index);
  else
    return std::make_pair(units::frequency::megahertz_t(0),
                          units::voltage::millivolt_t(0));
}

std::vector<unsigned int>
AMD::PMFreqVoltQMLItem::providePMFreqVoltActiveStates() const
{
  return activeStates_;
}

std::unique_ptr<Exportable::Exporter> AMD::PMFreqVoltQMLItem::initializer(
    IQMLComponentFactory const &qmlComponentFactory,
    QQmlApplicationEngine &qmlEngine)
{
  return std::make_unique<AMD::PMFreqVoltQMLItem::Initializer>(
      qmlComponentFactory, qmlEngine, *this);
}

void AMD::PMFreqVoltQMLItem::controlName(std::string const &name)
{
  instanceID_ = QString::fromStdString(name);
  emit controlLabelChanged(tr(name.c_str()));
}

void AMD::PMFreqVoltQMLItem::voltModes(std::vector<std::string> const &)
{
}

void AMD::PMFreqVoltQMLItem::voltRange(units::voltage::millivolt_t min,
                                       units::voltage::millivolt_t max)
{
  emit voltRangeChanged(min.to<int>(), max.to<int>());
}

void AMD::PMFreqVoltQMLItem::freqRange(units::frequency::megahertz_t min,
                                       units::frequency::megahertz_t max)
{
  emit freqRangeChanged(min.to<int>(), max.to<int>());
}

QVector<int> AMD::PMFreqVoltQMLItem::activeStatesIndices(
    std::vector<unsigned int> const &indices) const
{
  QVector<int> states;
  states.reserve(indices.size());
  std::transform(indices.cbegin(), indices.cend(), std::back_inserter(states),
                 [&](unsigned int index) { return static_cast<int>(index); });

  return states;
}

bool AMD::PMFreqVoltQMLItem::register_()
{
  QMLComponentRegistry::addQMLTypeRegisterer([]() {
    qmlRegisterType<AMD::PMFreqVoltQMLItem>("CoreCtrl.UIComponents", 1, 0,
                                            AMD::PMFreqVolt::ItemID.data());
  });

  QMLComponentRegistry::addQMLItemProvider(
      AMD::PMFreqVolt::ItemID, [](QQmlApplicationEngine &engine) {
        QQmlComponent component(
            &engine, QStringLiteral("qrc:/qml/AMDPMFreqVoltForm.qml"));
        return qobject_cast<QMLItem *>(component.create());
      });

  return true;
}

bool const AMD::PMFreqVoltQMLItem::registered_ =
    AMD::PMFreqVoltQMLItem::register_();
