// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import CoreCtrl.UIComponents 1.0

AMD_PM_OVERCLOCK {
  id: pmOclk
  objectName: "AMD_PM_OVERCLOCK"

  width: controls.width
  height: controls.height

  Pane {
    id: controls
    padding: 0

    Column {
      objectName: "AMD_PM_OVERCLOCK_Plug"

      property var childrenAdded: []
      onChildrenChanged: {
        for (var i = 0; i < children.length; ++i) {
          if (childrenAdded[children[i]] === undefined) {
            childrenAdded[children[i]] = children[i]
            pmOclk.setupChild(children[i])
          }
        }
      }
    }
  }
}
