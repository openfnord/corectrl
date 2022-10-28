// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmfreqrangeqmlitem.h"

#include "core/qmlcomponentregistry.h"
#include "pmfreqrange.h"
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QString>
#include <QtGlobal>
#include <QtQml>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <memory>

char const *const AMD::PMFreqRangeQMLItem::trStrings[] = {
    QT_TRANSLATE_NOOP("AMD::PMFreqRangeQMLItem", "SCLK"),
    QT_TRANSLATE_NOOP("AMD::PMFreqRangeQMLItem", "MCLK"),
};

class AMD::PMFreqRangeQMLItem::Initializer final
: public QMLItem::Initializer
, public AMD::PMFreqRange::Exporter
{
 public:
  Initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine,
              AMD::PMFreqRangeQMLItem &qmlItem) noexcept
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
  void takePMFreqRangeControlName(std::string const &name) override;
  void takePMFreqRangeStateRange(units::frequency::megahertz_t min,
                                 units::frequency::megahertz_t max) override;
  void takePMFreqRangeStates(
      std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
          &states) override;

 private:
  AMD::PMFreqRangeQMLItem &outer_;
};

void AMD::PMFreqRangeQMLItem::Initializer::takeActive(bool active)
{
  outer_.takeActive(active);
}

void AMD::PMFreqRangeQMLItem::Initializer::takePMFreqRangeControlName(
    std::string const &name)
{
  outer_.takePMFreqRangeControlName(name);
}

void AMD::PMFreqRangeQMLItem::Initializer::takePMFreqRangeStateRange(
    units::frequency::megahertz_t min, units::frequency::megahertz_t max)
{
  outer_.stateRange(min, max);
}

void AMD::PMFreqRangeQMLItem::Initializer::takePMFreqRangeStates(
    std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const &states)
{
  outer_.takePMFreqRangeStates(states);
}

AMD::PMFreqRangeQMLItem::PMFreqRangeQMLItem() noexcept
{
  setName(tr(AMD::PMFreqRange::ItemID.data()));
}

void AMD::PMFreqRangeQMLItem::changeState(int index, int freq)
{
  if (states_.count(static_cast<unsigned int>(index)) > 0) {
    auto &stateFreq = states_.at(static_cast<unsigned int>(index));
    if (stateFreq.to<int>() != freq) {
      stateFreq = units::frequency::megahertz_t(freq);

      emit stateChanged(index, freq);
      emit settingsChanged();
    }
  }
}

QString const &AMD::PMFreqRangeQMLItem::instanceID() const
{
  return instanceID_;
}

void AMD::PMFreqRangeQMLItem::activate(bool active)
{
  takeActive(active);
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
AMD::PMFreqRangeQMLItem::provideExporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMFreqRangeQMLItem::provideImporter(Item const &)
{
  return {};
}

void AMD::PMFreqRangeQMLItem::takeActive(bool active)
{
  active_ = active;
  setVisible(active);
}

void AMD::PMFreqRangeQMLItem::takePMFreqRangeControlName(std::string const &name)
{
  controlName(name);
}

void AMD::PMFreqRangeQMLItem::takePMFreqRangeStates(
    std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const &states)
{
  QVariantList statesList;
  std::map<unsigned int, units::frequency::megahertz_t> newStates;

  for (auto &[index, freq] : states) {
    newStates.emplace(index, freq);

    statesList.push_back(index);
    statesList.push_back(freq.to<int>());
  }

  if (newStates != states_) {
    std::swap(states_, newStates);
    emit statesChanged(statesList);
  }
}

bool AMD::PMFreqRangeQMLItem::provideActive() const
{
  return active_;
}

units::frequency::megahertz_t
AMD::PMFreqRangeQMLItem::providePMFreqRangeState(unsigned int index) const
{
  if (states_.count(index) > 0)
    return states_.at(index);
  else
    return units::frequency::megahertz_t(0);
}

std::unique_ptr<Exportable::Exporter> AMD::PMFreqRangeQMLItem::initializer(
    IQMLComponentFactory const &qmlComponentFactory,
    QQmlApplicationEngine &qmlEngine)
{
  return std::make_unique<AMD::PMFreqRangeQMLItem::Initializer>(
      qmlComponentFactory, qmlEngine, *this);
}

void AMD::PMFreqRangeQMLItem::controlName(std::string const &name)
{
  instanceID_ = QString::fromStdString(name);
  emit controlLabelChanged(tr(name.c_str()));
}

void AMD::PMFreqRangeQMLItem::stateRange(units::frequency::megahertz_t min,
                                         units::frequency::megahertz_t max)
{
  emit stateRangeChanged(min.to<int>(), max.to<int>());
}

bool AMD::PMFreqRangeQMLItem::register_()
{
  QMLComponentRegistry::addQMLTypeRegisterer([]() {
    qmlRegisterType<AMD::PMFreqRangeQMLItem>("CoreCtrl.UIComponents", 1, 0,
                                             AMD::PMFreqRange::ItemID.data());
  });

  QMLComponentRegistry::addQMLItemProvider(
      AMD::PMFreqRange::ItemID, [](QQmlApplicationEngine &engine) {
        QQmlComponent component(
            &engine, QStringLiteral("qrc:/qml/AMDPMFreqRangeForm.qml"));
        return qobject_cast<QMLItem *>(component.create());
      });

  return true;
}

bool const AMD::PMFreqRangeQMLItem::registered_ =
    AMD::PMFreqRangeQMLItem::register_();
