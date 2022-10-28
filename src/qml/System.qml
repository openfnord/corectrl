// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import CoreCtrl.UIComponents 1.0
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
