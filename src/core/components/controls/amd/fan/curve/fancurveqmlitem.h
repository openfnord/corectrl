// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/qmlitem.h"
#include "fancurveprofilepart.h"
#include <QObject>
#include <QPointF>
#include <QVariantList>
#include <QtGlobal>
#include <vector>

namespace AMD {

class FanCurveQMLItem
: public QMLItem
, public AMD::FanCurveProfilePart::Importer
, public AMD::FanCurveProfilePart::Exporter
{
  Q_OBJECT
  Q_PROPERTY(bool fanStop READ fanStop WRITE enableFanStop NOTIFY fanStopChanged)
  Q_PROPERTY(qreal fanStartValue READ fanStartValue WRITE changeFanStartValue
                 NOTIFY fanStartValueChanged)
  Q_PROPERTY(qreal minTemp READ minTemp)
  Q_PROPERTY(qreal maxTemp READ maxTemp)

 public:
  explicit FanCurveQMLItem() noexcept;

 signals:
  void curveChanged(QVariantList const &points);
  void fanStopChanged(bool enabled);
  void fanStartValueChanged(qreal value);
  void temperatureRangeChanged(qreal min, qreal max);

 public slots:
  void enableFanStop(bool enabled);
  void changeFanStartValue(qreal value);
  void updateCurvePoint(QPointF const &oldPoint, QPointF const &newPoint);

 public:
  void activate(bool active) override;
  QVariantList const &curve() const;
  bool fanStop() const;
  qreal fanStartValue() const;
  qreal minTemp() const;
  qreal maxTemp() const;

  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;
  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;

  bool provideActive() const override;
  std::vector<FanCurve::Point> const &provideFanCurvePoints() const override;
  bool provideFanCurveFanStop() const override;
  units::concentration::percent_t provideFanCurveFanStartValue() const override;

  void takeActive(bool active) override;
  void takeFanCurvePoints(std::vector<FanCurve::Point> const &points) override;
  void takeFanCurveFanStop(bool enabled) override;
  void takeFanCurveFanStartValue(units::concentration::percent_t value) override;

  std::unique_ptr<Exportable::Exporter>
  initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine) override;

 private:
  void temperatureRange(units::temperature::celsius_t min,
                        units::temperature::celsius_t max);
  class Initializer;

  bool active_;

  std::vector<FanCurve::Point> points_;
  QVariantList qPoints_;
  bool fanStop_;
  unsigned int fanStartValue_;

  qreal minTemp_;
  qreal maxTemp_;

  static bool register_();
  static bool const registered_;

  static char const *const trStrings[];
};

} // namespace AMD
