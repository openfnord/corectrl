// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "../sensorgraphitem.h"
#include "core/qmlcomponentregistry.h"
#include "power.h"
#include "units/units.h"
#include <QtGlobal>

namespace AMD {
namespace Power {

bool const registered_ = QMLComponentRegistry::addQuickItemProvider(
    AMD::Power::ItemID, []() {
      return new SensorGraphItem<units::power::watt_t, unsigned int>(
          AMD::Power::ItemID, units::power::watt_t().abbreviation());
    });

char const *const trStrings[] = {
    QT_TRANSLATE_NOOP("SensorGraph", "AMD_POWER"),
};

} // namespace Power
} // namespace AMD
