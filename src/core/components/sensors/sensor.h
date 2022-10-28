// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/components/sensors/isensor.h"
#include "core/idatasource.h"
#include "units/units.h"
#include <atomic>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

template<typename Unit, typename T>
class Sensor : public ISensor
{
 public:
  class Exporter : public ISensor::Exporter
  {
   public:
    virtual void takeValue(Unit value) = 0;
    virtual void takeRange(std::optional<std::pair<Unit, Unit>> const &range) = 0;
  };

  Sensor(
      std::string_view id,
      std::vector<std::unique_ptr<IDataSource<T>>> &&dataSources,
      std::optional<std::pair<Unit, Unit>> &&range = std::nullopt,
      std::function<T(std::vector<T> const &)> &&transform =
          [](std::vector<T> const &input) { return input[0]; }) noexcept
  : id_(id)
  , dataSources_(std::move(dataSources))
  , range_(std::move(range))
  , transform_(std::move(transform))
  , value_(units::make_unit<Unit>(0))
  {
    transformValues_.resize(dataSources_.size(), 0);
  }

  std::string const &ID() const override
  {
    return id_;
  }

  void exportWith(ISensor::Exporter &e) const override
  {
    auto exporter = e.provideExporter(*this);
    if (exporter.has_value()) {
      auto &sensorExporter =
          dynamic_cast<Sensor<Unit, T>::Exporter &>(exporter->get());
      sensorExporter.takeValue(value());
      sensorExporter.takeRange(range_);
    }
  }

  void update() override
  {
    if (!dataSources_.empty()) {
      for (size_t i = 0; i < dataSources_.size(); ++i)
        dataSources_[i]->read(transformValues_[i]);

      value(transform_(transformValues_));
    }
  }

 protected:
  Unit value() const
  {
    return value_.load(std::memory_order_relaxed);
  }

  void value(T value)
  {
    value_.store(units::make_unit<Unit>(value), std::memory_order_relaxed);
  }

 private:
  std::string const id_;
  std::vector<std::unique_ptr<IDataSource<T>>> const dataSources_;
  std::optional<std::pair<Unit, Unit>> const range_;
  std::function<T(std::vector<T> const &)> const transform_;

  std::vector<T> transformValues_;
  std::atomic<Unit> value_;
};
