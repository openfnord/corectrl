// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "../sensorgraphitem.h"
#include "core/qmlcomponentregistry.h"
#include "memusage.h"
#include "units/units.h"
#include <QtGlobal>

namespace AMD {
namespace MemUsage {

bool const registered_ = QMLComponentRegistry::addQuickItemProvider(
    AMD::MemUsage::ItemID, []() {
      return new SensorGraphItem<units::data::megabyte_t, unsigned int>(
          AMD::MemUsage::ItemID, units::data::megabyte_t().abbreviation());
    });

char const *const trStrings[] = {
    QT_TRANSLATE_NOOP("SensorGraph", "AMD_MEM_USAGE"),
};

} // namespace MemUsage
} // namespace AMD
