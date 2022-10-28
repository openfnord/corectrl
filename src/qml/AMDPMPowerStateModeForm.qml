// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

import QtQuick 2.15
import CoreCtrl.UIComponents 1.0

AMD_PM_POWERSTATE_MODE {
  id: pStateMode
  objectName: "AMD_PM_POWERSTATE_MODE"

  width: modeSelector.width
  height: modeSelector.height

  onModesChanged: modeSelector.setModes(modes)
  onModeChanged: modeSelector.select(mode)

  ModeSelector {
    id: modeSelector
    headerTitle: qsTr("Power management mode")
    contentParentObject: "AMD_PM_POWERSTATE_MODE_Plug"

    onSelectionChanged: pStateMode.changeMode(mode)
    onChildAdded: pStateMode.setupChild(child)
  }
}
