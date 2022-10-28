// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "../sensorgraphitem.h"
#include "core/qmlcomponentregistry.h"
#include "cpufreqpack.h"
#include "units/units.h"
#include <QtGlobal>

namespace CPUFreqPack {

bool const registered_ = QMLComponentRegistry::addQuickItemProvider(
    CPUFreqPack::ItemID, []() {
      return new SensorGraphItem<units::frequency::megahertz_t, unsigned int>(
          CPUFreqPack::ItemID, units::frequency::megahertz_t().abbreviation());
    });

char const *const trStrings[] = {
    QT_TRANSLATE_NOOP("SensorGraph", "CPU_FREQ_PACK"),
};

} // namespace CPUFreqPack
