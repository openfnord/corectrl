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
import "Style.js" as Style

Dialog {
  id: settingsDlg

  title: qsTr("Settings")

  focus: true
  modal: true
  standardButtons: Dialog.Apply | Dialog.Discard
  closePolicy: Popup.CloseOnEscape

  // center in parent
  x: (parent.width - width) / 2
  y: (parent.height - height) / 2

  Column {
    CheckBox {
      id: sysTrayIcon
      text: qsTr("Show system tray icon")

      onToggled: footer.standardButton(Dialog.Apply).enabled = true
    }

    CheckBox {
      id: startOnSysTray
      enabled: sysTrayIcon.enabled && sysTrayIcon.checked
      text: qsTr("Start minimized on system tray")

      onToggled: footer.standardButton(Dialog.Apply).enabled = true
    }
  }

  onOpened: {
    footer.standardButton(Dialog.Apply).enabled = false

    sysTrayIcon.enabled = systemTray.isAvailable()
    sysTrayIcon.checked = settings.getValue("sysTray", true)
    startOnSysTray.checked = settings.getValue("startOnSysTray", true)
  }

  onApplied: {
    settings.setValue("sysTray", sysTrayIcon.checked)
    settings.setValue("startOnSysTray", startOnSysTray.checked)
    close()
  }

  onDiscarded: close()

  Component.onCompleted: {
    footer.standardButton(Dialog.Apply).enabled = false
    footer.standardButton(Dialog.Apply).hoverEnabled = Style.g_hover
    footer.standardButton(Dialog.Discard).hoverEnabled = Style.g_hover
  }
}
