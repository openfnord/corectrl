// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "../sensorgraphitem.h"
#include "core/qmlcomponentregistry.h"
#include "memfreq.h"
#include "units/units.h"
#include <QtGlobal>

namespace AMD {
namespace MemFreq {

bool const registered_ = QMLComponentRegistry::addQuickItemProvider(
    AMD::MemFreq::ItemID, []() {
      return new SensorGraphItem<units::frequency::megahertz_t, unsigned int>(
          AMD::MemFreq::ItemID, units::frequency::megahertz_t().abbreviation());
    });

char const *const trStrings[] = {
    QT_TRANSLATE_NOOP("SensorGraph", "AMD_MEM_FREQ"),
};

} // namespace MemFreq
} // namespace AMD
