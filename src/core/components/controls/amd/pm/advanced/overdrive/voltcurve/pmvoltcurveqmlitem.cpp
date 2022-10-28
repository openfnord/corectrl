// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmvoltcurveqmlitem.h"

#include "core/qmlcomponentregistry.h"
#include "pmvoltcurve.h"
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

class AMD::PMVoltCurveQMLItem::Initializer final
: public QMLItem::Initializer
, public AMD::PMVoltCurve::Exporter
{
 public:
  Initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine,
              AMD::PMVoltCurveQMLItem &qmlItem) noexcept
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
  void takePMVoltCurveModes(std::vector<std::string> const &modes) override;
  void takePMVoltCurveMode(std::string const &mode) override;
  void takePMVoltCurvePointsRange(
      std::vector<std::pair<
          std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t>,
          std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t>>> const
          &ranges) override;
  void takePMVoltCurvePoints(
      std::vector<std::pair<units::frequency::megahertz_t,
                            units::voltage::millivolt_t>> const &points) override;

 private:
  AMD::PMVoltCurveQMLItem &outer_;
};

void AMD::PMVoltCurveQMLItem::Initializer::takeActive(bool active)
{
  outer_.takeActive(active);
}

void AMD::PMVoltCurveQMLItem::Initializer::takePMVoltCurveModes(
    std::vector<std::string> const &modes)
{
  outer_.modes(modes);
}

void AMD::PMVoltCurveQMLItem::Initializer::takePMVoltCurveMode(
    std::string const &mode)
{
  outer_.takePMVoltCurveMode(mode);
}

void AMD::PMVoltCurveQMLItem::Initializer::takePMVoltCurvePointsRange(
    std::vector<std::pair<
        std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t>,
        std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t>>> const
        &ranges)
{
  outer_.pointsRange(ranges);
}

void AMD::PMVoltCurveQMLItem::Initializer::takePMVoltCurvePoints(
    std::vector<std::pair<units::frequency::megahertz_t,
                          units::voltage::millivolt_t>> const &points)
{
  outer_.takePMVoltCurvePoints(points);
}

AMD::PMVoltCurveQMLItem::PMVoltCurveQMLItem() noexcept
{
  setName(tr(AMD::PMVoltCurve::ItemID.data()));
}

void AMD::PMVoltCurveQMLItem::changeMode(const QString &mode)
{
  auto newMode = mode.toStdString();
  if (mode_ != newMode) {
    mode_ = newMode;

    emit modeChanged(mode);
    emit settingsChanged();
  }
}

void AMD::PMVoltCurveQMLItem::updatePoint(QPointF const &oldPoint,
                                          QPointF const &newPoint)
{
  if (oldPoint != newPoint) {
    auto oPoint = std::make_pair(
        units::frequency::megahertz_t(std::round(oldPoint.x())),
        units::voltage::millivolt_t(std::round(oldPoint.y())));
    auto nPoint = std::make_pair(
        units::frequency::megahertz_t(std::round(newPoint.x())),
        units::voltage::millivolt_t(std::round(newPoint.y())));

    for (size_t i = 0; i < points_.size(); ++i) {
      if (points_[i] == oPoint) {
        points_[i] = nPoint;
        qPoints_.replace(static_cast<int>(i), newPoint);

        emit pointsChanged(qPoints_);
        emit settingsChanged();
        break;
      }
    }
  }
}

void AMD::PMVoltCurveQMLItem::activate(bool active)
{
  takeActive(active);
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
AMD::PMVoltCurveQMLItem::provideExporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMVoltCurveQMLItem::provideImporter(Item const &)
{
  return {};
}

void AMD::PMVoltCurveQMLItem::takeActive(bool active)
{
  active_ = active;
  setVisible(active);
}

bool AMD::PMVoltCurveQMLItem::provideActive() const
{
  return active_;
}

void AMD::PMVoltCurveQMLItem::takePMVoltCurveMode(std::string const &mode)
{
  if (mode_ != mode) {
    mode_ = mode;
    emit modeChanged(QString::fromStdString(mode_));
  }
}

std::string const &AMD::PMVoltCurveQMLItem::providePMVoltCurveMode() const
{
  return mode_;
}

void AMD::PMVoltCurveQMLItem::takePMVoltCurvePoints(
    std::vector<std::pair<units::frequency::megahertz_t,
                          units::voltage::millivolt_t>> const &points)
{
  if (points_ != points) {
    points_ = points;

    qPoints_.clear();
    for (auto [freq, volt] : points)
      qPoints_.push_back(QPointF(freq.to<qreal>(), volt.to<qreal>()));

    emit pointsChanged(qPoints_);
  }
}

std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
AMD::PMVoltCurveQMLItem::providePMVoltCurvePoint(unsigned int index) const
{
  if (index < points_.size())
    return points_.at(index);
  else
    return std::make_pair(units::frequency::megahertz_t(0),
                          units::voltage::millivolt_t(0));
}

std::unique_ptr<Exportable::Exporter> AMD::PMVoltCurveQMLItem::initializer(
    IQMLComponentFactory const &qmlComponentFactory,
    QQmlApplicationEngine &qmlEngine)
{
  return std::make_unique<AMD::PMVoltCurveQMLItem::Initializer>(
      qmlComponentFactory, qmlEngine, *this);
}

void AMD::PMVoltCurveQMLItem::modes(std::vector<std::string> const &)
{
}

void AMD::PMVoltCurveQMLItem::pointsRange(
    std::vector<std::pair<
        std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t>,
        std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t>>> const
        &ranges)
{
  // NOTE Assuming that all points has the same range.
  emit pointsRangeChanged(ranges.back().first.first.to<int>(),
                          ranges.back().first.second.to<int>(),
                          ranges.back().second.first.to<int>(),
                          ranges.back().second.second.to<int>());
}

bool AMD::PMVoltCurveQMLItem::register_()
{
  QMLComponentRegistry::addQMLTypeRegisterer([]() {
    qmlRegisterType<AMD::PMVoltCurveQMLItem>("CoreCtrl.UIComponents", 1, 0,
                                             AMD::PMVoltCurve::ItemID.data());
  });

  QMLComponentRegistry::addQMLItemProvider(
      AMD::PMVoltCurve::ItemID, [](QQmlApplicationEngine &engine) {
        QQmlComponent component(
            &engine, QStringLiteral("qrc:/qml/AMDPMVoltCurveForm.qml"));
        return qobject_cast<QMLItem *>(component.create());
      });

  return true;
}

bool const AMD::PMVoltCurveQMLItem::registered_ =
    AMD::PMVoltCurveQMLItem::register_();
