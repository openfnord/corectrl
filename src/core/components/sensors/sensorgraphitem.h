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
#pragma once

#include "core/exportable.h"
#include "graphitem.h"
#include "sensor.h"
#include "units/units.h"
#include <functional>
#include <optional>
#include <string_view>
#include <utility>

template<typename Unit, typename T>
class SensorReader final : public Sensor<Unit, T>::Exporter
{
 public:
  SensorReader(
      std::string_view id, std::function<void(Unit)> &&onValue,
      std::function<void(std::optional<std::pair<Unit, Unit>> const &)> &&onRange)
  : id_(id)
  , onValue_(std::move(onValue))
  , onRange_(std::move(onRange))
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override
  {
    if (i.ID() == id_)
      return *this;

    return {};
  }

  void takeValue(Unit value) override
  {
    onValue_(value);
  }

  void takeRange(std::optional<std::pair<Unit, Unit>> const &range) override
  {
    onRange_(range);
  }

 private:
  std::string_view const id_;
  std::function<void(Unit)> const onValue_;
  std::function<void(std::optional<std::pair<Unit, Unit>> const &)> const onRange_;
};

template<typename Unit, typename T>
class SensorGraphItem final : public GraphItem
{
 public:
  SensorGraphItem(std::string_view id, std::string_view unit) noexcept
  : GraphItem(id, unit)
  , valueReader_(
        id, [&](Unit value) { this->updateGraph(value.template to<double>()); },
        [](std::optional<std::pair<Unit, Unit>> const &) {})
  , rangeReader_(
        id, [&](Unit) {},
        [&](std::optional<std::pair<Unit, Unit>> const &range) {
          if (range.has_value()) {
            this->initialRange(range.value().first.template to<double>(),
                               range.value().second.template to<double>());
          }
        })
  {
  }

  void init(Exportable const *sensor) override
  {
    sensor_ = sensor;
    sensor_->exportWith(rangeReader_);
  }

  void update() override
  {
    sensor_->exportWith(valueReader_);
  }

 private:
  SensorReader<Unit, T> valueReader_;
  SensorReader<Unit, T> rangeReader_;

  Exportable const *sensor_;
};
