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
import QtQuick.Layouts 1.3
import "Style.js" as Style
import "Settings.js" as Settings

ApplicationWindow {
  id: appWindow

  title: appInfo.name
  width: 970
  height: 600
  visible: true

  font.family: Style.g_text.family
  font.pointSize: Style.g_text.size
  font.capitalization: Font.MixedCase

  Material.theme: Material.Dark
  Material.accent: Style.Material.accent

  StackLayout {
    id: components

    anchors.fill: parent
    currentIndex: tabBar.currentIndex

    Profiles {}
    System {}
  }

  onVisibleChanged: components.visible = visible

  footer: TabBar {
    id: tabBar
    font.bold: true
    font.capitalization: Font.AllUppercase
    Material.background: Style.TabBar.main_footer.bg_color
    hoverEnabled: Style.g_hover

    TabButton {
        text: qsTr("Profiles")
    }
    TabButton {
        text: qsTr("System")
    }
  }

  Connections {
    target: systemTray

    onAvailable: {
      var showSysTray = settings.getValue("sysTray",
                                          Settings.SysemTrayDefaults.enabled)
      if (showSysTray) {
        systemTray.show()
        appWindow.visible = !settings.getValue("startOnSysTray",
                                               Settings.SysemTrayDefaults.startMinimized)
      }
    }

    onActivated: {
      if (!appWindow.visible)
        appWindow.show()
      else
        appWindow.hide()
    }

    onQuit: {
      systemTray.hide()
      Qt.quit()
    }
  }

  onClosing: {
    if (!systemTray.isAvailable() || !systemTray.isVisible())
      Qt.quit()
  }

  Component.onCompleted: {
    if (systemTray.isAvailable() && systemTray.isVisible())
      appWindow.visible = !settings.getValue("startOnSysTray",
                                             Settings.SysemTrayDefaults.startMinimized)
  }
}
