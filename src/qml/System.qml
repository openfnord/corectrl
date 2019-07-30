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
import Radman.UIComponents 1.0
import "Style.js" as Style

Page {
  hoverEnabled: Style.g_hover

  SYSTEM_INFO {
    id: systemInfo
    objectName: "SYSTEM_INFO"

    onAddSystemInfo: p.newInfoComponent(componentName, info)
  }

  QtObject {
    id: p

    function newInfoComponent(componentName, info) {
      var tab = tabBtnComponent.createObject(tabBar)
      tab.text = componentName
      tabBar.addItem(tab)

      var infoPane = infoPaneComponent.createObject(infoPages)
      infoPane.addInfo(info)
    }
  }

  Component {
    id: infoPaneComponent

    InfoPane {}
  }

  Component {
    id: tabBtnComponent

    TabButton {
      id: tabBtn

      background: Rectangle {
        color: tabBtn.hovered ? Style.TabButton.bg_color_alt
                              : Style.TabButton.bg_color
      }
    }
  }

  SettingsDialog {
    id: settingsDlg
  }

  ColumnLayout {
    spacing: 0
    anchors.fill: parent

    TabBar {
      id: tabBar
      Layout.fillWidth: true

      Component.onCompleted: {
        var aboutTab = tabBtnComponent.createObject(tabBar)
        aboutTab.text = appInfo.name
        tabBar.addItem(aboutTab)
      }
    }

    StackLayout {
      id: infoPages
      currentIndex: tabBar.currentIndex

      About {}
    }
  }

  header: ToolBar {
    Material.elevation: Style.ToolBar.Material.elevation
    Material.background: Style.ToolBar.bg_color

    RowLayout {
      anchors.fill: parent

      Label {
        text: qsTr("Information and application settings...")
        leftPadding: 10
        color: Style.ToolBar.text_color_msg
      }

      Rectangle {
        Layout.fillWidth: true
      }

      ToolButton {
        text: qsTr("Settings")
        visible: tabBar.currentIndex === 0
        onClicked: settingsDlg.open()
      }

      ToolButton {
        text: qsTr("Copy all")
        visible: tabBar.currentIndex > 0
        onClicked: systemInfo.copyToClipboard()
      }
    }
  }
}
