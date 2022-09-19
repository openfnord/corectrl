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
import "Settings.js" as Settings

ColumnLayout {

  signal settingsChanged()

  function opened() {
    p.refreshState()
  }

  function accepted() {
    p.updateSettings()
  }

  Component.onCompleted:  p.readSettings()

  QtObject {
    id: p

    property bool sysTrayIcon: Settings.SysemTrayDefaults.enabled
    property bool startOnSysTray: Settings.SysemTrayDefaults.startMinimized
    property bool saveWindowGeometry: Settings.SysemTrayDefaults.saveWindowGeometry

    function refreshState() {
      sysTrayIconCb.enabled = systemTray.isAvailable()
      sysTrayIconCb.checked = sysTrayIcon
      startOnSysTrayCb.checked = startOnSysTray
      saveWindowGeometryCb.checked = saveWindowGeometry
    }

    function readSettings() {
      sysTrayIcon = settings.getValue("sysTray", sysTrayIcon)
      startOnSysTray = settings.getValue("startOnSysTray", startOnSysTray)
      saveWindowGeometry = settings.getValue("saveWindowGeometry", saveWindowGeometry)
    }

    function updateSettings() {
      if (sysTrayIconCb.checked !== sysTrayIcon) {
        sysTrayIcon = sysTrayIconCb.checked
        settings.setValue("sysTray", sysTrayIconCb.checked)
      }

      if (startOnSysTrayCb.checked !== startOnSysTray) {
        startOnSysTray = startOnSysTrayCb.checked
        settings.setValue("startOnSysTray", startOnSysTrayCb.checked)
      }

      if (saveWindowGeometryCb.checked !== saveWindowGeometry) {
        saveWindowGeometry = saveWindowGeometryCb.checked
        settings.setValue("saveWindowGeometry", saveWindowGeometryCb.checked)
      }
    }
  }

  CheckBox {
    id: sysTrayIconCb
    text: qsTr("Show system tray icon")

    hoverEnabled: Style.g_hover

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    onToggled: settingsChanged()
  }

  CheckBox {
    id: startOnSysTrayCb
    enabled: sysTrayIconCb.enabled && sysTrayIconCb.checked
    text: qsTr("Start minimized on system tray")

    hoverEnabled: Style.g_hover

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    onToggled: settingsChanged()
  }

  CheckBox {
    id: saveWindowGeometryCb
    text: qsTr("Save window geometry")

    hoverEnabled: Style.g_hover

    leftPadding: 0
    rightPadding: 0
    topPadding: 0
    bottomPadding: 0

    onToggled: settingsChanged()
  }
}
