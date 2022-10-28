// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

import QtQuick 2.15
import CoreCtrl.UIComponents 1.0
import "Style.js" as Style

AMD_PM_FREQ_MODE {
  id: freqMode
  objectName: "AMD_PM_FREQ_MODE"

  width: modeSelector.width
  height: modeSelector.height

  onModesChanged: modeSelector.setModes(modes)
  onModeChanged: modeSelector.select(mode)

  ModeSelector {
    id: modeSelector
    headerTitle: qsTr("Frequency")
    headerBackground: Style.ModeSelector.body.bg_color
    contentParentObject: "AMD_PM_FREQ_MODE_Plug"

    onSelectionChanged: freqMode.changeMode(mode)
    onChildAdded: freqMode.setupChild(child)
  }
}
