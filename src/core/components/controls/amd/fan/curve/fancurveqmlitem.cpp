// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "fancurveqmlitem.h"

#include "core/qmlcomponentregistry.h"
#include "fancurve.h"
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QString>
#include <QtQml>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <memory>
#include <utility>

char const *const AMD::FanCurveQMLItem::trStrings[] = {
    QT_TRANSLATE_NOOP("ControlModeQMLItem", "AMD_FAN_CURVE"),
};

class AMD::FanCurveQMLItem::Initializer final
: public QMLItem::Initializer
, public AMD::FanCurve::Exporter
{
 public:
  Initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine,
              AMD::FanCurveQMLItem &qmlItem) noexcept
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
  void
  takeFanCurvePoints(std::vector<AMD::FanCurve::Point> const &points) override;
  void takeFanCurveFanStop(bool enabled) override;
  void takeFanCurveFanStartValue(units::concentration::percent_t value) override;
  void takeFanCurveTemperatureRange(units::temperature::celsius_t min,
                                    units::temperature::celsius_t max) override;

 private:
  AMD::FanCurveQMLItem &outer_;
};

void AMD::FanCurveQMLItem::Initializer::takeActive(bool active)
{
  outer_.takeActive(active);
}

void AMD::FanCurveQMLItem::Initializer::takeFanCurvePoints(
    std::vector<AMD::FanCurve::Point> const &points)
{
  outer_.takeFanCurvePoints(points);
}

void AMD::FanCurveQMLItem::Initializer::takeFanCurveFanStop(bool enabled)
{
  outer_.takeFanCurveFanStop(enabled);
}

void AMD::FanCurveQMLItem::Initializer::takeFanCurveFanStartValue(
    units::concentration::percent_t value)
{
  outer_.takeFanCurveFanStartValue(value);
}

void AMD::FanCurveQMLItem::Initializer::takeFanCurveTemperatureRange(
    units::temperature::celsius_t min, units::temperature::celsius_t max)
{
  outer_.temperatureRange(min, max);
}

AMD::FanCurveQMLItem::FanCurveQMLItem() noexcept
{
  setName(tr(AMD::FanCurve::ItemID.data()));
}

void AMD::FanCurveQMLItem::enableFanStop(bool enabled)
{
  if (fanStop_ != enabled) {
    fanStop_ = enabled;
    emit fanStopChanged(fanStop_);
    emit settingsChanged();
  }
}

void AMD::FanCurveQMLItem::changeFanStartValue(qreal value)
{
  auto newValue = static_cast<unsigned int>(std::round(value));
  if (fanStartValue_ != newValue) {
    fanStartValue_ = newValue;
    emit fanStartValueChanged(fanStartValue_);
    emit settingsChanged();
  }
}

void AMD::FanCurveQMLItem::updateCurvePoint(QPointF const &oldPoint,
                                            QPointF const &newPoint)
{
  if (oldPoint != newPoint) {
    auto oPoint = std::make_pair(
        units::temperature::celsius_t(std::round(oldPoint.x())),
        units::concentration::percent_t(std::round(oldPoint.y())));
    auto nPoint = std::make_pair(
        units::temperature::celsius_t(std::round(newPoint.x())),
        units::concentration::percent_t(std::round(newPoint.y())));

    for (size_t i = 0; i < points_.size(); ++i) {
      if (points_[i] == oPoint) {
        points_[i] = nPoint;
        qPoints_.replace(static_cast<int>(i), newPoint);

        emit curveChanged(qPoints_);
        emit settingsChanged();
        break;
      }
    }
  }
}

void AMD::FanCurveQMLItem::activate(bool active)
{
  takeActive(active);
}

QVariantList const &AMD::FanCurveQMLItem::curve() const
{
  return qPoints_;
}

bool AMD::FanCurveQMLItem::fanStop() const
{
  return fanStop_;
}

qreal AMD::FanCurveQMLItem::fanStartValue() const
{
  return static_cast<qreal>(fanStartValue_);
}

qreal AMD::FanCurveQMLItem::minTemp() const
{
  return minTemp_;
}

qreal AMD::FanCurveQMLItem::maxTemp() const
{
  return maxTemp_;
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::FanCurveQMLItem::provideImporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
AMD::FanCurveQMLItem::provideExporter(Item const &)
{
  return {};
}

bool AMD::FanCurveQMLItem::provideActive() const
{
  return active_;
}

std::vector<AMD::FanCurve::Point> const &
AMD::FanCurveQMLItem::provideFanCurvePoints() const
{
  return points_;
}

bool AMD::FanCurveQMLItem::provideFanCurveFanStop() const
{
  return fanStop_;
}

units::concentration::percent_t
AMD::FanCurveQMLItem::provideFanCurveFanStartValue() const
{
  return fanStartValue_;
}

void AMD::FanCurveQMLItem::takeActive(bool active)
{
  active_ = active;
  setVisible(active);
}

void AMD::FanCurveQMLItem::takeFanCurvePoints(
    std::vector<AMD::FanCurve::Point> const &points)
{
  if (points_ != points) {
    points_ = points;

    qPoints_.clear();
    for (auto &[temp, pwm] : points_)
      qPoints_.push_back(QPointF(temp.to<qreal>(), pwm.to<qreal>() * 100));

    emit curveChanged(qPoints_);
  }
}

void AMD::FanCurveQMLItem::takeFanCurveFanStop(bool active)
{
  if (fanStop_ != active) {
    fanStop_ = active;
    emit fanStopChanged(fanStop_);
  }
}

void AMD::FanCurveQMLItem::takeFanCurveFanStartValue(
    units::concentration::percent_t value)
{
  auto newValue = static_cast<unsigned int>(value * 100);
  if (fanStartValue_ != newValue) {
    fanStartValue_ = newValue;
    emit fanStartValueChanged(fanStartValue_);
  }
}

std::unique_ptr<Exportable::Exporter>
AMD::FanCurveQMLItem::initializer(IQMLComponentFactory const &qmlComponentFactory,
                                  QQmlApplicationEngine &qmlEngine)
{
  return std::make_unique<AMD::FanCurveQMLItem::Initializer>(
      qmlComponentFactory, qmlEngine, *this);
}

void AMD::FanCurveQMLItem::temperatureRange(units::temperature::celsius_t min,
                                            units::temperature::celsius_t max)
{
  minTemp_ = min.to<qreal>();
  maxTemp_ = max.to<qreal>();

  emit temperatureRangeChanged(minTemp_, maxTemp_);
}

bool AMD::FanCurveQMLItem::register_()
{
  QMLComponentRegistry::addQMLTypeRegisterer([]() {
    qmlRegisterType<AMD::FanCurveQMLItem>("CoreCtrl.UIComponents", 1, 0,
                                          AMD::FanCurve::ItemID.data());
  });

  QMLComponentRegistry::addQMLItemProvider(
      AMD::FanCurve::ItemID, [](QQmlApplicationEngine &engine) {
        QQmlComponent component(&engine,
                                QStringLiteral("qrc:/qml/AMDFanCurveForm.qml"));
        return qobject_cast<QMLItem *>(component.create());
      });

  return true;
}

bool const AMD::FanCurveQMLItem::registered_ = AMD::FanCurveQMLItem::register_();
