// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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

bool readAMDGPUVRamSize(int deviceFD, units::data::megabyte_t *size);

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

bool readRadeonVRamSize(int deviceFD, units::data::megabyte_t *size);

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
/// @param ppPowerProfileModeLines pp_power_profile_mode data source contents
std::optional<int> parsePowerProfileModeCurrentModeIndex(
    std::vector<std::string> const &ppPowerProfileModeLines);

/// Returns the available clock + voltage states for a control. (4.17+)
/// @param controlName name of the control
/// @param ppOdClkVoltageLines pp_od_clk_voltage data source contents
std::optional<std::vector<std::tuple<unsigned int, units::frequency::megahertz_t,
                                     units::voltage::millivolt_t>>>
parseOverdriveClksVolts(std::string_view controlName,
                        std::vector<std::string> const &ppOdClkVoltageLines);

/// Returns the state's clock range for a control. (4.18+)
/// @param controlName name of the control
/// @param ppOdClkVoltageLines pp_od_clk_voltage data source contents
std::optional<std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t>>
parseOverdriveClkRange(std::string_view controlName,
                       std::vector<std::string> const &ppOdClkVoltageLines);

/// Returns the state's voltage range. (4.18+)
/// @param ppOdClkVoltageLines pp_od_clk_voltage data source contents
std::optional<std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t>>
parseOverdriveVoltRange(std::vector<std::string> const &ppOdClkVoltageLines);

/// Returns the available clock states for a control. (4.20+, vega20+)
/// @param controlName name of the control
/// @param ppOdClkVoltageLines pp_od_clk_voltage data source contents
std::optional<std::vector<std::pair<unsigned int, units::frequency::megahertz_t>>>
parseOverdriveClks(std::string_view controlName,
                   std::vector<std::string> const &ppOdClkVoltageLines);

/// Returns the voltage curve points for curve voltage ASICs. (4.20+, vega20+)
/// @param ppOdClkVoltageLines pp_od_clk_voltage data source contents
std::optional<std::vector<
    std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>>>
parseOverdriveVoltCurve(std::vector<std::string> const &ppOdClkVoltageLines);

/// Returns voltage range of curve points for curve voltage ASICs. (4.20+, vega20+)
/// @param ppOdClkVoltageLines pp_od_clk_voltage data source contents
std::optional<std::vector<std::pair<
    std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t>,
    std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t>>>>
parseOverdriveVoltCurveRange(std::vector<std::string> const &ppOdClkVoltageLines);

/// Returns the voltage offset.
/// @param ppOdClkVoltageLines pp_od_clk_voltage data source contents
std::optional<units::voltage::millivolt_t>
parseOverdriveVoltOffset(std::vector<std::string> const &ppOdClkVoltageLines);

/// Returns a list containing the name of the available CLK controls.
/// @param ppOdClkVoltageLines pp_od_clk_voltage data source contents
std::optional<std::vector<std::string>>
parseOverdriveClkControls(std::vector<std::string> const &ppOdClkVoltageLines);

/// Translates a CLK control name to the control commit command id.
/// @param controlName name of the control
/// @returns commit command id
std::optional<std::string>
getOverdriveClkControlCmdId(std::string_view controlName);

/// Returns a list containing the indices of all out of range states present on
/// pp_od_clk_voltage for the given control name.
/// @param controlName name of the control
/// @param ppOdClkVoltageLines pp_od_clk_voltage data source contents
std::optional<std::vector<unsigned int>> ppOdClkVoltageFreqRangeOutOfRangeStates(
    std::string const &controlName,
    std::vector<std::string> const &ppOdClkVoltageLines);

/// Returns whether pp_od_clk_voltage has known frequency + voltage control quirks.
/// @param controlName name of the control
/// @param ppOdClkVoltageLines pp_od_clk_voltage data source contents
bool ppOdClkVoltageHasKnownFreqVoltQuirks(
    std::string const &controlName,
    std::vector<std::string> const &ppOdClkVoltageLines);

/// Returns whether pp_od_clk_voltage has known voltage curve control quirks.
/// @param ppOdClkVoltageLines pp_od_clk_voltage data source contents
bool ppOdClkVoltageHasKnownVoltCurveQuirks(
    std::vector<std::string> const &ppOdClkVoltageLines);

/// Returns true when overdrive clock + voltage state control is available.
bool hasOverdriveClkVoltControl(std::vector<std::string> const &data);

/// Returns true when overdrive clock control is available.
bool hasOverdriveClkControl(std::vector<std::string> const &data);

/// Returns true when overdrive voltage curve control is available.
bool hasOverdriveVoltCurveControl(std::vector<std::string> const &data);

/// Returns true when overdrive voltage offset control is available.
bool hasOverdriveVoltOffsetControl(std::vector<std::string> const &data);

} // namespace AMD
} // namespace Utils
