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
import QtQuick.Layouts 1.3
import "Style.js" as Style

Dialog {
  id: settingsDlg

  title: qsTr("Settings")

  focus: true
  modal: true
  standardButtons: Dialog.Ok | Dialog.Cancel
  closePolicy: Popup.CloseOnEscape

  // center in parent
  x: (parent.width - width) / 2
  y: (parent.height - height) / 2

  ColumnLayout {
    anchors.fill: parent

    TabBar {
      id: tabBar
      Layout.fillWidth: true
      hoverEnabled: Style.g_hover

      Repeater {
        model: [qsTr("General"), qsTr("Workarounds")]

        TabButton {
          text: modelData

          background: Rectangle {
            color: hovered ? Style.Dialog.tabs.bg_color_alt
                           : Style.Dialog.tabs.bg_color
          }
        }
      }
    }

    StackLayout {
      currentIndex: tabBar.currentIndex

      SettingsGeneral {
        id: general
        onSettingsChanged: footer.standardButton(Dialog.Ok).enabled = true
      }

      SettingsWorkarounds {
        id: workarounds
        onSettingsChanged: footer.standardButton(Dialog.Ok).enabled = true
      }
    }
  }

  onOpened: {
    general.opened()
    workarounds.opened()

    footer.standardButton(Dialog.Ok).enabled = false
  }

  onAccepted: {
    general.accepted()
    workarounds.accepted()

    close()
  }

  onRejected: close()

  Component.onCompleted: {
    footer.standardButton(Dialog.Ok).enabled = false
    footer.standardButton(Dialog.Ok).hoverEnabled = Style.g_hover
    footer.standardButton(Dialog.Cancel).hoverEnabled = Style.g_hover
  }
}
