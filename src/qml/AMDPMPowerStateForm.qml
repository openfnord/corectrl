//
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>
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
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import Radman.UIComponents 1.0
import "Style.js" as Style

AMD_PM_POWERSTATE {
  id: pmPowerState
  objectName: "AMD_PM_POWERSTATE"

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
        pmPowerState.changeMode(button.mode)
      }
    }
  }

  Page {
    id: contents

    header: Pane {
      leftPadding: Style.ModeSelector.header.padding
      rightPadding: Style.ModeSelector.header.padding
      topPadding: 25
      bottomPadding: 24
      Material.background: Style.ModeSelector.header.bg_color

      Row {
        Label {
          text: qsTr("Power management state")

        }
      }
    }

    Pane {
      anchors.fill: parent
      padding: Style.g_padding
      Material.background: Style.ModeSelector.body.bg_color

      Row {
        id: btnContainer
        spacing: 8
      }
    }
  }
}
