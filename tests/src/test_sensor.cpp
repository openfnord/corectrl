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
