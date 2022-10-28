// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

import QtQuick 2.15
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
