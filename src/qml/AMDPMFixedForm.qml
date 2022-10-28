// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import CoreCtrl.UIComponents 1.0
import "Style.js" as Style

AMD_PM_FIXED {
  id: pmFixed
  objectName: "AMD_PM_FIXED"

  width: contents.width
  height: contents.height

  Component {
    id: modeButton

    Button {
      property string mode;
      checkable: true;
    }
  }

  onModesChanged: {
    for (var i = 0; i < modes.length; i+=2) {
      var button = modeButton.createObject(btnContainer);
      button.mode = modes[i]
      button.text = modes[i + 1]
    }
    btnContainer.forceLayout()
  }

  onModeChanged: {
    // check the button specified by mode
    var buttons = btnContainer.children
    for (var i = 0; i < buttons.length; i++) {
      if (buttons[i].mode === mode) {
        buttons[i].checked = true
        btnGroup.lastChecked = buttons[i]
        break;
      }
    }
  }

  ButtonGroup {
    id: btnGroup
    property var lastChecked
    buttons: btnContainer.children

    onClicked: {
      if (lastChecked !== button) {
        lastChecked = button
        pmFixed.changeMode(button.mode)
      }
    }
  }

  Pane {
    id: contents
    padding: Style.g_padding

    Row {
      id: btnContainer
      spacing: 8
    }
  }
}
