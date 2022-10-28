// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "../sensorgraphitem.h"
#include "core/qmlcomponentregistry.h"
#include "gputemp.h"
#include "units/units.h"
#include <QtGlobal>

namespace AMD {
namespace GPUTemp {

bool const registered_ = QMLComponentRegistry::addQuickItemProvider(
    AMD::GPUTemp::ItemID, []() {
      return new SensorGraphItem<units::temperature::celsius_t, int>(
          AMD::GPUTemp::ItemID, "\u00B0C");
    });

char const *const trStrings[] = {
    QT_TRANSLATE_NOOP("SensorGraph", "AMD_GPU_TEMP"),
};

} // namespace GPUTemp
} // namespace AMD
