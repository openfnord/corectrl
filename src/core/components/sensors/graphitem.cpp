// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "graphitem.h"

#include <QTimer>
#include <QtCharts/QValueAxis>
#include <algorithm>
#include <limits>

unsigned short const GraphItem::PointsCount = 120;

GraphItem::GraphItem(std::string_view name, std::string_view unit)
: QQuickItem()
, name_(name.data())
, unit_(unit.data())
, yMin_(std::numeric_limits<qreal>::max())
, yMax_(std::numeric_limits<qreal>::min())
{
  setObjectName(name_);
  points_.reserve(GraphItem::PointsCount);

  connect(this, &GraphItem::visibleChanged, this, &GraphItem::refreshSeriePoints);
}

QString const &GraphItem::name() const
{
  return name_;
}

QString const &GraphItem::unit() const
{
  return unit_;
}

QString GraphItem::color() const
{
  return QString::fromStdString(color_);
}

qreal GraphItem::value() const
{
  return value_;
}

void GraphItem::initialRange(qreal min, qreal max)
{
  if (max > min) {
    yMin_ = min;
    yMax_ = max;
  }
}

void GraphItem::updateGraph(qreal value)
{
  if (series_ != nullptr && !ignored()) {

    if (points_.size() == PointsCount)
      points_.removeFirst();

    qreal newX;
    auto lastX = points_.empty() ? PointsCount : points_.last().x();
    if (static_cast<unsigned short>(lastX) ==
        std::numeric_limits<unsigned short>::max()) {
      // 65535 * 0.5 / 60 / 60 ~> 9h 10min @ 0.5s refresh rate between restarts

      restartXPoints();
      newX = PointsCount;
    }
    else {
      newX = lastX + 1;
    }

    points_.append(QPointF(newX, value));
    QTimer::singleShot(0, this, &GraphItem::refreshSeriePoints);

    // update axes
    xAxis_->setRange(newX - PointsCount + 1, newX);
    updateYAxis(value);

    value_ = value;
    emit valueChanged(value);
  }
}

std::optional<std::reference_wrapper<Importable::Importer>>
GraphItem::provideImporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
GraphItem::provideExporter(Item const &)
{
  return {};
}

bool GraphItem::provideActive() const
{
  return active_;
}

std::string const &GraphItem::provideColor() const
{
  return color_;
}

void GraphItem::takeActive(bool active)
{
  if (active_ != active) {
    active_ = active;

    if (series_ != nullptr)
      series_->setVisible(active);

    emit activeChanged(active);
  }
}

void GraphItem::takeColor(std::string const &color)
{
  if (!color.empty() && color_ != color) {
    color_ = color;

    if (series_ != nullptr)
      series_->setColor(color_.c_str());

    emit colorChanged(QString::fromStdString(color_));
  }
}

void GraphItem::configure(QAbstractSeries *series, QAbstractAxis *xAxis,
                          QAbstractAxis *yAxis)
{
  series_ = dynamic_cast<QLineSeries *>(series);
  series_->setColor(color_.c_str());
  series_->setVisible(active_);

  xAxis_ = xAxis;
  yAxis_ = yAxis;

  xAxis_->setRange(1, PointsCount);
  yAxis_->setRange(yMin_, yMax_);
}

void GraphItem::updateYAxisRange(qreal min, qreal max)
{
  yMin_ = std::min(yMin_, min);
  yMax_ = std::max(yMax_, (max > yMin_) ? max : yMin_ + 1);

  yAxis_->setRange(yMin_, yMax_);
}

void GraphItem::refreshSeriePoints()
{
  if (isVisible() && series_ != nullptr)
    series_->replace(points_);
}

bool GraphItem::active() const
{
  return provideActive();
}

void GraphItem::active(bool active)
{
  if (active_ != active) {
    active_ = active;

    if (series_ != nullptr)
      series_->setVisible(active);

    emit settingsChanged();
  }
}

bool GraphItem::ignored() const
{
  return ignored_;
}

void GraphItem::ignored(bool ignored)
{
  if (ignored_ != ignored) {
    ignored_ = ignored;

    if (series_ != nullptr)
      series_->setVisible(active_ && !ignored_);

    emit ignoredChanged(ignored);
  }
}

void GraphItem::updateYAxis(qreal value)
{
  if (value < yMin_ || value > yMax_) {
    updateYAxisRange(value, value);
    emit yAxisRangeChanged(yMin_, yMax_);
  }
}

void GraphItem::restartXPoints()
{
  int pointX = 1;
  for (auto &point : points_)
    point.rx() = pointX++;
}
