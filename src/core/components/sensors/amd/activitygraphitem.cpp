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
#include "../sensorgraphitem.h"
#include "activity.h"
#include "core/qmlcomponentregistry.h"
#include "units/units.h"
#include <QtGlobal>

namespace AMD {
namespace Activity {

bool const registered_ = QMLComponentRegistry::addQuickItemProvider(
    AMD::Activity::ItemID, []() {
      return new SensorGraphItem<units::dimensionless::scalar_t, unsigned int>(
          AMD::Activity::ItemID, "%");
    });

char const *const trStrings[] = {
    QT_TRANSLATE_NOOP("SensorGraph", "AMD_ACTIVITY"),
};

} // namespace Activity
} // namespace AMD
