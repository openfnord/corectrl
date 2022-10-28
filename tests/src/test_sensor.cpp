// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "catch.hpp"
#include "trompeloeil.hpp"

#include "core/components/sensors/sensor.h"

extern template struct trompeloeil::reporter<trompeloeil::specialized>;

namespace Tests {
namespace Sensor {

class SensorTestAdapter
: public ::Sensor<units::frequency::megahertz_t, unsigned int>
{
 public:
  using ::Sensor<units::frequency::megahertz_t, unsigned int>::Sensor;
  using ::Sensor<units::frequency::megahertz_t, unsigned int>::value;
};

class SensorDataSourceStub : public IDataSource<unsigned int>
{
 public:
  SensorDataSourceStub(unsigned int data)
  : data_(data)
  {
  }

  std::string source() const override
  {
    return "";
  }

  bool read(unsigned int &data) override
  {
    data = data_;
    return true;
  }

  unsigned int const data_;
};

class SensorExporterMock
: public ::Sensor<units::frequency::megahertz_t, unsigned int>::Exporter
{
 public:
  MAKE_MOCK1(takeValue, void(units::frequency::megahertz_t));
  MAKE_MOCK1(takeRange,
             void(std::optional<std::pair<units::frequency::megahertz_t,
                                          units::frequency::megahertz_t>> const &));
  MAKE_MOCK1(
      provideExporter,
      std::optional<std::reference_wrapper<Exportable::Exporter>>(Item const &));
};

TEST_CASE("Sensor tests", "[GPU][Sensor]")
{
  std::vector<std::unique_ptr<IDataSource<unsigned int>>> dataSources;
  dataSources.emplace_back(std::make_unique<SensorDataSourceStub>(1));
  dataSources.emplace_back(std::make_unique<SensorDataSourceStub>(2));
  SensorTestAdapter ts("_test_sensor_", std::move(dataSources),
                       std::make_pair(units::frequency::megahertz_t(100),
                                      units::frequency::megahertz_t(20000)),
                       [](std::vector<unsigned int> const &input) {
                         return input[0] + input[1];
                       });

  auto testValue = units::frequency::megahertz_t(3);
  std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t> range(
      100, 20000);

  SECTION("Has 0 as initial value")
  {
    REQUIRE(ts.value() == units::frequency::megahertz_t(0));

    SECTION("Update its value from DataSource")
    {
      ts.update();

      REQUIRE(ts.value() == testValue);

      SECTION("Exports its value")
      {
        SensorExporterMock e;
        ALLOW_CALL(e, provideExporter(trompeloeil::_)).LR_RETURN(e);
        REQUIRE_CALL(e, takeValue(trompeloeil::eq(testValue)));
        REQUIRE_CALL(e, takeRange(trompeloeil::_)).LR_WITH(_1.value() == range);
        ts.exportWith(e);
      }
    }
  }
}
} // namespace Sensor
} // namespace Tests
