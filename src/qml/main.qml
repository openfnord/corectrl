// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import "Style.js" as Style
import "Settings.js" as Settings

ApplicationWindow {
  id: appWindow

  title: appInfo.name

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

  onClosing: {
    if (!systemTray.isAvailable() || !systemTray.isVisible())
      Qt.quit()
  }
}
