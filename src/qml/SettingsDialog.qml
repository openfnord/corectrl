// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
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
