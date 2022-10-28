// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "graphitemprofilepart.h"
#include <QList>
#include <QObject>
#include <QPointF>
#include <QQuickItem>
#include <QString>
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <string>
#include <string_view>

QT_CHARTS_USE_NAMESPACE

class GraphItem
: public QQuickItem
, public GraphItemProfilePart::Importer
, public GraphItemProfilePart::Exporter
{
  Q_OBJECT
  Q_PROPERTY(QString name READ name NOTIFY nameChanged)
  Q_PROPERTY(qreal value READ value NOTIFY valueChanged)
  Q_PROPERTY(bool active READ active WRITE active NOTIFY activeChanged)
  Q_PROPERTY(bool ignored READ ignored WRITE ignored NOTIFY ignoredChanged)
  Q_PROPERTY(QString unit READ unit)
  Q_PROPERTY(QString color READ color)

 public:
  GraphItem(std::string_view name, std::string_view unit);

  QString const &name() const;
  QString const &unit() const;
  QString color() const;
  qreal value() const;

  void initialRange(qreal min, qreal max);
  void updateGraph(qreal value);

  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) final override;
  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) final override;

  bool provideActive() const final override;
  std::string const &provideColor() const final override;

  void takeActive(bool active) override;

  void takeColor(std::string const &color) override;

  virtual void init(Exportable const *e) = 0;

 public slots:
  void configure(QAbstractSeries *series, QAbstractAxis *xAxis,
                 QAbstractAxis *yAxis);
  void updateYAxisRange(qreal min, qreal max);
  void refreshSeriePoints();

  virtual void update() = 0;

 signals:
  void settingsChanged();
  void nameChanged();
  void valueChanged(qreal value);
  void activeChanged(bool active);
  void colorChanged(QString const &color);
  void ignoredChanged(bool ignored);
  void yAxisRangeChanged(qreal min, qreal max);

 private:
  bool active() const;
  void active(bool active);
  bool ignored() const;
  void ignored(bool ignored);
  void updateYAxis(qreal value);
  void restartXPoints();

  static unsigned short const PointsCount;

  QString const name_;
  QString const unit_;

  std::string color_{"white"};
  bool active_{true};
  bool ignored_{false};

  qreal value_{0};

  QList<QPointF> points_;

  QLineSeries *series_{nullptr};
  QAbstractAxis *xAxis_;
  QAbstractAxis *yAxis_;

  qreal yMin_;
  qreal yMax_;
};
