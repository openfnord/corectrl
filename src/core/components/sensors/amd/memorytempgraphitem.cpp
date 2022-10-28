// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "../sensorgraphitem.h"
#include "core/qmlcomponentregistry.h"
#include "memorytemp.h"
#include "units/units.h"
#include <QtGlobal>

namespace AMD {
namespace MemoryTemp {

bool const registered_ = QMLComponentRegistry::addQuickItemProvider(
    AMD::MemoryTemp::ItemID, []() {
      return new SensorGraphItem<units::temperature::celsius_t, int>(
          AMD::MemoryTemp::ItemID, "\u00B0C");
    });

char const *const trStrings[] = {
    QT_TRANSLATE_NOOP("SensorGraph", "AMD_GPU_MEMORY_TEMP"),
};

} // namespace MemoryTemp
} // namespace AMD
