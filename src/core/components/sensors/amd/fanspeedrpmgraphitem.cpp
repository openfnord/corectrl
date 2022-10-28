// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "../sensorgraphitem.h"
#include "core/qmlcomponentregistry.h"
#include "fanspeedrpm.h"
#include "units/units.h"
#include <QtGlobal>

namespace AMD {
namespace FanSpeedRPM {

bool const registered_ = QMLComponentRegistry::addQuickItemProvider(
    AMD::FanSpeedRPM::ItemID, []() {
      return new SensorGraphItem<units::angular_velocity::revolutions_per_minute_t,
                                 unsigned int>(
          AMD::FanSpeedRPM::ItemID,
          units::angular_velocity::revolutions_per_minute_t().abbreviation());
    });

char const *const trStrings[] = {
    QT_TRANSLATE_NOOP("SensorGraph", "AMD_FAN_SPEED_RPM"),
};

} // namespace FanSpeedRPM
} // namespace AMD
