// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "../sensorgraphitem.h"
#include "core/qmlcomponentregistry.h"
#include "gpufreq.h"
#include "units/units.h"
#include <QtGlobal>

namespace AMD {
namespace GPUFreq {

bool const registered_ = QMLComponentRegistry::addQuickItemProvider(
    AMD::GPUFreq::ItemID, []() {
      return new SensorGraphItem<units::frequency::megahertz_t, unsigned int>(
          AMD::GPUFreq::ItemID, units::frequency::megahertz_t().abbreviation());
    });

char const *const trStrings[] = {
    QT_TRANSLATE_NOOP("SensorGraph", "AMD_GPU_FREQ"),
};

} // namespace GPUFreq
} // namespace AMD
