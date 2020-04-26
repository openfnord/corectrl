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

#include "units/units.h"
#include <cstdint>
#include <libdrm/amdgpu_drm.h>
#include <libdrm/radeon_drm.h>
#include <optional>
#include <string>
#include <string_view>
#include <sys/ioctl.h>
#include <tuple>
#include <utility>
#include <vector>

namespace Utils {
namespace AMD {

template<typename Data>
bool readAMDGPUInfoSensor(int deviceFD, Data *value, std::uint32_t sensor)
{
#if defined(AMDGPU_INFO_SENSOR) && defined(DRM_IOCTL_AMDGPU_INFO)
  struct drm_amdgpu_info buffer = {};

  buffer.query = AMDGPU_INFO_SENSOR;
  buffer.return_pointer = reinterpret_cast<std::uint64_t>(value);
  buffer.return_size = sizeof(*value);
  buffer.sensor_info.type = sensor;

  return ioctl(deviceFD, DRM_IOCTL_AMDGPU_INFO, &buffer) >= 0;
#else
  return false;
#endif
}

template<typename Data>
bool readAMDGPUInfo(int deviceFD, Data *value, std::uint32_t query)
{
#if defined(DRM_IOCTL_AMDGPU_INFO)
  struct drm_amdgpu_info buffer = {};

  buffer.query = query;
  buffer.return_pointer = reinterpret_cast<std::uint64_t>(value);
  buffer.return_size = sizeof(*value);

  return ioctl(deviceFD, DRM_IOCTL_AMDGPU_INFO, &buffer) >= 0;
#else
  return false;
#endif
}

template<typename Data>
bool readRadeonInfoSensor(int deviceFD, Data *value, std::uint32_t sensor)
{
#if defined(DRM_IOCTL_RADEON_INFO)
  struct drm_radeon_info buffer = {};

  buffer.request = sensor;
  buffer.value = reinterpret_cast<std::uint64_t>(value);

  return ioctl(deviceFD, DRM_IOCTL_RADEON_INFO, &buffer) >= 0;
#else
  return false;
#endif
}

/// Returns DPM states clocks. (4.6+)
/// @param ppDpmLines pp_dpm_sclk / pp_dpm_mclk data source contents
std::optional<std::vector<std::pair<unsigned int, units::frequency::megahertz_t>>>
parseDPMStates(std::vector<std::string> const &ppDpmLines);

/// Returns the current state index from pp_dpm_sclk / pp_dpm_mclk. (4.6+)
/// @param ppDpmLines pp_dpm_sclk / pp_dpm_mclk data source contents
std::optional<unsigned int>
parseDPMCurrentStateIndex(std::vector<std::string> const &ppDpmLines);

/// Returns power profile modes. (4.17+)
/// @param  ppPowerProfileModeLines pp_power_profile_mode data source contents
std::optional<std::vector<std::pair<std::string, int>>>
parsePowerProfileModeModes(std::vector<std::string> const &ppPowerProfileModeLines);

/// Returns current power profile mode index. (4.17+)
/// @param  ppPowerProfileModeLines pp_power_profile_mode data source contents
std::optional<int> parsePowerProfileModeCurrentModeIndex(
    std::vector<std::string> const &ppPowerProfileModeLines);

/// Returns states clock / voltage target data. (4.17+)
/// @param targetLbl SCLK or MCLK
/// @param ppOdClkVoltageLines pp_od_clk_voltage data source contents
std::optional<std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                                     units::voltage::millivolt_t>>>
parseOdClkVoltStateStates(std::string_view targetLbl,
                          std::vector<std::string> const &ppOdClkVoltageLines);

/// Returns clock range from OD_RANGE data. (4.18+)
/// @param targetLbl SCLK or MCLK
/// @param ppOdClkVoltageLines pp_od_clk_voltage data source contents
std::optional<std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t>>
parseOdClkVoltStateClkRange(std::string_view targetLbl,
                            std::vector<std::string> const &ppOdClkVoltageLines);

/// Returns state voltage range from OD_RANGE data. (4.18+)
/// @param ppOdClkVoltageLines pp_od_clk_voltage data source contents
std::optional<std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t>>
parseOdClkVoltStateVoltRange(std::vector<std::string> const &ppOdClkVoltageLines);

/// Returns states clock target data for curve voltage ASICs. (4.20+, vega20+)
/// @param targetLbl SCLK or MCLK
/// @param ppOdClkVoltageLines pp_od_clk_voltage data source contents
std::optional<std::vector<std::pair<unsigned int, units::frequency::megahertz_t>>>
parseOdClkVoltCurveStates(std::string_view targetLbl,
                          std::vector<std::string> const &ppOdClkVoltageLines);

/// Returns curve points for curve voltage ASICs. (4.20+, vega20+)
/// @param ppOdClkVoltageLines pp_od_clk_voltage data source contents
std::optional<std::vector<
    std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>>>
parseOdClkVoltCurvePoints(std::vector<std::string> const &ppOdClkVoltageLines);

/// Returns curve points voltage range from OD_RANGE data for curve voltage
/// ASICs. (4.20+, vega20+)
/// @param ppOdClkVoltageLines pp_od_clk_voltage data source contents
std::optional<
    std::vector<std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t>>>
parseOdClkVoltCurveVoltRange(std::vector<std::string> const &ppOdClkVoltageLines);

/// Returns whether pp_od_clk_voltage has known quirks.
/// @param ppOdClkVoltageLines pp_od_clk_voltage data source contents
bool ppOdClkVoltageHasKnownQuirks(
    std::vector<std::string> const &ppOdClkVoltageLines);

} // namespace AMD
} // namespace Utils
