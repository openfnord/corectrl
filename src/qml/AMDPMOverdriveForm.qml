// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import CoreCtrl.UIComponents 1.0
import "Style.js" as Style

AMD_PM_OVERDRIVE {
  id: pmOverdrive
  objectName: "AMD_PM_OVERDRIVE"

  width: controls.width
  height: controls.height

  Pane {
    id: controls
    padding: Style.g_padding

    Row {
      objectName: "AMD_PM_OVERDRIVE_Plug"
      spacing: 5

      property var childrenAdded: []
      onChildrenChanged: {
        for (var i = 0; i < children.length; ++i) {
          if (childrenAdded[children[i]] === undefined) {
            childrenAdded[children[i]] = children[i]
            pmOverdrive.setupChild(children[i])
          }
        }
      }
    }
  }
}
