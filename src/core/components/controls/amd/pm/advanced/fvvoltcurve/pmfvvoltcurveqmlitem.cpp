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
#include "pmfvvoltcurveqmlitem.h"

#include "core/qmlcomponentregistry.h"
#include "pmfvvoltcurve.h"
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

class AMD::PMFVVoltCurveQMLItem::Initializer final
: public QMLItem::Initializer
, public AMD::PMFVVoltCurve::Exporter
{
 public:
  Initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine,
              AMD::PMFVVoltCurveQMLItem &qmlItem) noexcept
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
  void takePMFVVoltCurveVoltModes(std::vector<std::string> const &modes) override;
  void takePMFVVoltCurveVoltMode(std::string const &mode) override;
  void takePMFVVoltCurveVoltRange(
      std::vector<std::pair<
          std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t>,
          std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t>>> const
          &pointRanges) override;
  void takePMFVVoltCurveGPURange(units::frequency::megahertz_t min,
                                 units::frequency::megahertz_t max) override;

  void takePMFVVoltCurveMemRange(units::frequency::megahertz_t min,
                                 units::frequency::megahertz_t max) override;

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
  AMD::PMFVVoltCurveQMLItem &outer_;
};

void AMD::PMFVVoltCurveQMLItem::Initializer::takeActive(bool active)
{
  outer_.takeActive(active);
}

void AMD::PMFVVoltCurveQMLItem::Initializer::takePMFVVoltCurveVoltModes(
    std::vector<std::string> const &modes)
{
  outer_.voltModes(modes);
}

void AMD::PMFVVoltCurveQMLItem::Initializer::takePMFVVoltCurveVoltMode(
    std::string const &mode)
{
  outer_.takePMFVVoltCurveVoltMode(mode);
}

void AMD::PMFVVoltCurveQMLItem::Initializer::takePMFVVoltCurveVoltRange(
    std::vector<std::pair<
        std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t>,
        std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t>>> const
        &pointRanges)
{
  outer_.voltCurveRange(pointRanges);
}

void AMD::PMFVVoltCurveQMLItem::Initializer::takePMFVVoltCurveGPURange(
    units::frequency::megahertz_t min, units::frequency::megahertz_t max)
{
  outer_.gpuRange(min, max);
}

void AMD::PMFVVoltCurveQMLItem::Initializer::takePMFVVoltCurveMemRange(
    units::frequency::megahertz_t min, units::frequency::megahertz_t max)
{
  outer_.memRange(min, max);
}

void AMD::PMFVVoltCurveQMLItem::Initializer::takePMFVVoltCurveVoltCurve(
    std::vector<std::pair<units::frequency::megahertz_t,
                          units::voltage::millivolt_t>> const &curve)
{
  outer_.takePMFVVoltCurveVoltCurve(curve);
}

void AMD::PMFVVoltCurveQMLItem::Initializer::takePMFVVoltCurveGPUStates(
    std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const &states)
{
  outer_.takePMFVVoltCurveGPUStates(states);
}

void AMD::PMFVVoltCurveQMLItem::Initializer::takePMFVVoltCurveMemStates(
    std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const &states)
{
  outer_.takePMFVVoltCurveMemStates(states);
}

AMD::PMFVVoltCurveQMLItem::PMFVVoltCurveQMLItem() noexcept
{
  setName(tr(AMD::PMFVVoltCurve::ItemID.data()));
}

void AMD::PMFVVoltCurveQMLItem::changeVoltMode(const QString &mode)
{
  auto newMode = mode.toStdString();
  if (voltMode_ != newMode) {
    voltMode_ = newMode;

    emit voltModeChanged(mode);
    emit settingsChanged();
  }
}

void AMD::PMFVVoltCurveQMLItem::updateVoltCurvePoint(QPointF const &oldPoint,
                                                     QPointF const &newPoint)
{
  if (oldPoint != newPoint) {
    auto oPoint = std::make_pair(
        units::frequency::megahertz_t(std::round(oldPoint.x())),
        units::voltage::millivolt_t(std::round(oldPoint.y())));
    auto nPoint = std::make_pair(
        units::frequency::megahertz_t(std::round(newPoint.x())),
        units::voltage::millivolt_t(std::round(newPoint.y())));

    for (size_t i = 0; i < voltCurve_.size(); ++i) {
      if (voltCurve_[i] == oPoint) {
        voltCurve_[i] = nPoint;
        qVoltCurve_.replace(static_cast<int>(i), newPoint);

        emit voltCurveChanged(qVoltCurve_);
        emit settingsChanged();
        break;
      }
    }
  }
}

void AMD::PMFVVoltCurveQMLItem::changeGPUState(int index, int freq)
{
  if (gpuStates_.count(static_cast<unsigned int>(index))) {
    auto &stateFreq = gpuStates_.at(static_cast<unsigned int>(index));
    if (stateFreq.to<int>() != freq) {
      stateFreq = units::frequency::megahertz_t(freq);

      emit gpuStateChanged(index, freq);
      emit settingsChanged();
    }
  }
}

void AMD::PMFVVoltCurveQMLItem::changeMemState(int index, int freq)
{
  if (memStates_.count(static_cast<unsigned int>(index))) {
    auto &stateFreq = memStates_.at(static_cast<unsigned int>(index));
    if (stateFreq.to<int>() != freq) {
      stateFreq = units::frequency::megahertz_t(freq);

      emit memStateChanged(index, freq);
      emit settingsChanged();
    }
  }
}

void AMD::PMFVVoltCurveQMLItem::activate(bool active)
{
  takeActive(active);
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
AMD::PMFVVoltCurveQMLItem::provideExporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMFVVoltCurveQMLItem::provideImporter(Item const &)
{
  return {};
}

void AMD::PMFVVoltCurveQMLItem::takeActive(bool active)
{
  active_ = active;
  setVisible(active);
}

bool AMD::PMFVVoltCurveQMLItem::provideActive() const
{
  return active_;
}

void AMD::PMFVVoltCurveQMLItem::takePMFVVoltCurveVoltMode(std::string const &mode)
{
  if (voltMode_ != mode) {
    voltMode_ = mode;
    emit voltModeChanged(QString::fromStdString(voltMode_));
  }
}

std::string const &AMD::PMFVVoltCurveQMLItem::providePMFVVoltCurveVoltMode() const
{
  return voltMode_;
}

void AMD::PMFVVoltCurveQMLItem::takePMFVVoltCurveVoltCurve(
    std::vector<std::pair<units::frequency::megahertz_t,
                          units::voltage::millivolt_t>> const &curve)
{
  if (voltCurve_ != curve) {
    voltCurve_ = curve;

    qVoltCurve_.clear();
    for (auto [freq, volt] : curve)
      qVoltCurve_.push_back(QPointF(freq.to<qreal>(), volt.to<qreal>()));

    emit voltCurveChanged(qVoltCurve_);
  }
}

std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
AMD::PMFVVoltCurveQMLItem::providePMFVVoltCurveVoltCurvePoint(unsigned int index) const
{
  if (index < voltCurve_.size())
    return voltCurve_.at(index);
  else
    return std::make_pair(units::frequency::megahertz_t(0),
                          units::voltage::millivolt_t(0));
}

void AMD::PMFVVoltCurveQMLItem::takePMFVVoltCurveGPUStates(
    std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const &states)
{
  QVariantList statesList;
  std::map<unsigned int, units::frequency::megahertz_t> newStates;

  for (auto &[index, freq] : states) {
    newStates.emplace(index, freq);

    statesList.push_back(index);
    statesList.push_back(freq.to<int>());
  }

  if (newStates != gpuStates_) {
    std::swap(gpuStates_, newStates);
    emit gpuStatesChanged(statesList);
  }
}

units::frequency::megahertz_t
AMD::PMFVVoltCurveQMLItem::providePMFVVoltCurveGPUState(unsigned int index) const
{
  if (gpuStates_.count(index) > 0)
    return gpuStates_.at(index);
  else
    return units::frequency::megahertz_t(0);
}

void AMD::PMFVVoltCurveQMLItem::takePMFVVoltCurveMemStates(
    std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const &states)
{
  QVariantList statesList;
  std::map<unsigned int, units::frequency::megahertz_t> newStates;

  for (auto &[index, freq] : states) {
    newStates.emplace(index, freq);

    statesList.push_back(index);
    statesList.push_back(freq.to<int>());
  }

  if (newStates != memStates_) {
    std::swap(memStates_, newStates);
    emit memStatesChanged(statesList);
  }
}

units::frequency::megahertz_t
AMD::PMFVVoltCurveQMLItem::providePMFVVoltCurveMemState(unsigned int index) const
{
  if (memStates_.count(index) > 0)
    return memStates_.at(index);
  else
    return units::frequency::megahertz_t(0);
}

std::unique_ptr<Exportable::Exporter> AMD::PMFVVoltCurveQMLItem::initializer(
    IQMLComponentFactory const &qmlComponentFactory,
    QQmlApplicationEngine &qmlEngine)
{
  return std::make_unique<AMD::PMFVVoltCurveQMLItem::Initializer>(
      qmlComponentFactory, qmlEngine, *this);
}

void AMD::PMFVVoltCurveQMLItem::voltModes(std::vector<std::string> const &)
{
  // XXX If more modes are added, emit a signal with the modes.
  // Add trStrings for the modes and handle the signal on the QML part.
}

void AMD::PMFVVoltCurveQMLItem::voltCurveRange(
    std::vector<std::pair<
        std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t>,
        std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t>>> const
        &pointRanges)
{
  // XXX Asume that all points share the same range. Only voltage data will be used.
  emit voltRangeChanged(pointRanges.back().second.first.to<int>(),
                        pointRanges.back().second.second.to<int>());
}

void AMD::PMFVVoltCurveQMLItem::voltRange(units::voltage::millivolt_t min,
                                          units::voltage::millivolt_t max)
{
  emit voltRangeChanged(min.to<int>(), max.to<int>());
}

void AMD::PMFVVoltCurveQMLItem::gpuRange(units::frequency::megahertz_t min,
                                         units::frequency::megahertz_t max)
{
  emit gpuFreqRangeChanged(min.to<int>(), max.to<int>());
}

void AMD::PMFVVoltCurveQMLItem::memRange(units::frequency::megahertz_t min,
                                         units::frequency::megahertz_t max)
{
  emit memFreqRangeChanged(min.to<int>(), max.to<int>());
}

bool AMD::PMFVVoltCurveQMLItem::register_()
{
  QMLComponentRegistry::addQMLTypeRegisterer([]() {
    qmlRegisterType<AMD::PMFVVoltCurveQMLItem>(
        "Radman.UIComponents", 1, 0, AMD::PMFVVoltCurve::ItemID.data());
  });

  QMLComponentRegistry::addQMLItemProvider(
      AMD::PMFVVoltCurve::ItemID, [](QQmlApplicationEngine &engine) {
        QQmlComponent component(
            &engine, QStringLiteral("qrc:/qml/AMDPMFVVoltCurveForm.qml"));
        return qobject_cast<QMLItem *>(component.create());
      });

  return true;
}

bool const AMD::PMFVVoltCurveQMLItem::registered_ =
    AMD::PMFVVoltCurveQMLItem::register_();
