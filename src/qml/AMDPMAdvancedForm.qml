// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import CoreCtrl.UIComponents 1.0

AMD_PM_ADVANCED {
  id: pmAdv
  objectName: "AMD_PM_ADVANCED"

  width: controls.width
  height: controls.height

  Pane {
    id: controls
    padding: 0

    Flow {
      objectName: "AMD_PM_ADVANCED_Plug"

      width: {
        var maxWidth = 0
        for (var i = 0; i < children.length; ++i)
          maxWidth = Math.max(maxWidth, children[i].width)
        return maxWidth
      }

      property var childrenAdded: []
      onChildrenChanged: {
        for (var i = 0; i < children.length; ++i) {
          if (childrenAdded[children[i]] === undefined) {
            childrenAdded[children[i]] = children[i]
            pmAdv.setupChild(children[i])
          }
        }
      }
    }
  }
}
