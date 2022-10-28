// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

import QtQuick 2.15
import CoreCtrl.UIComponents 1.0

AMD_PM_PERFMODE {
  id: pMode
  objectName: "AMD_PM_PERFMODE"

  width: modeSelector.width
  height: modeSelector.height

  onModesChanged: modeSelector.setModes(modes)
  onModeChanged: modeSelector.select(mode)

  ModeSelector {
    id: modeSelector
    headerTitle: qsTr("Performance mode")
    contentParentObject: "AMD_PM_PERFMODE_Plug"

    onSelectionChanged: pMode.changeMode(mode)
    onChildAdded: pMode.setupChild(child)
  }
}
