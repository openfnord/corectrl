// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import CoreCtrl.UIComponents 1.0
import "Style.js" as Style

AMD_PM_POWER_PROFILE {
  id: pmPowerProfile
  objectName: "AMD_PM_POWER_PROFILE"

  width: contents.width
  height: contents.height

  onModeChanged: {
    for (var i = 0; i < listModel.count; ++i) {
      if (listModel.get(i).mode === mode) {
        cbMode.lastIndex = i
        cbMode.currentIndex = i
        break;
      }
    }
  }

  onModesChanged: {
    listModel.clear()

    for (var i = 0; i < modes.length; i+=2) {
      var element = listElement.createObject()
      element.mode = modes[i]
      element.text = modes[i + 1]
      listModel.append(element)
    }

    cbMode.updateWidth()
  }

  ListModel { id: listModel }
  Component {
    id: listElement

    ListElement {
      property string text
      property string mode
    }
  }

  Pane {
    id: contents
    padding: Style.g_padding

    RowLayout {
      Label {
        text: qsTr("Power profile")
        rightPadding: 6
      }

      CComboBox {
        id: cbMode
        model: listModel

        property int lastIndex: 0

        onActivated: {
          if (lastIndex !== currentIndex) {
            lastIndex = currentIndex

            var mode = model.get(currentIndex).mode
            pmPowerProfile.changeMode(mode)
          }
        }
      }
    }
  }
}
