//
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>
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
import QtQuick 2.9
import CoreCtrl.UIComponents 1.0

CPU_CPUFREQ_MODE {
  id: cpuFreqMode
  objectName: "CPU_CPUFREQ_MODE"

  width: modeSelector.width
  height: modeSelector.height

  onModesChanged: modeSelector.setModes(modes)
  onModeChanged: modeSelector.select(mode)

  ModeSelector {
    id: modeSelector
    headerTitle: qsTr("Performance scaling")
    contentParentObject: "CPU_CPUFREQ_MODE_Plug"

    onSelectionChanged: cpuFreqMode.changeMode(mode)
    onChildAdded: cpuFreqMode.setupChild(child)
  }
}
