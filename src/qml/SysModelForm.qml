// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import CoreCtrl.UIComponents 1.0
import "Style.js" as Style

SYS_MODEL {
  id: sysModel
  objectName: "SYS_MODEL"

  anchors.fill: parent

  Component {
    id: tabButtonComponent

    TabButton {
      id: tabBtn

      property var component: null
      property bool active: component != null ? component.enabled : true
      text: component != null ? component.name : ""
      width: implicitWidth

      contentItem: RowLayout {
        Text {
          id: tabBtnText
          text: tabBtn.text
          font: tabBtn.font
          color: tabBtn.active ? tabBtn.checked ? Style.Material.accent
                                                : Material.foreground
                               : Style.Material.accent_alt
          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
          elide: Text.ElideRight
        }

        Item {
          implicitWidth: swc.width / 2

          Switch {
            id: swc

            padding: 0
            rotation: -90
            scale: Style.g_tweakScale
            checked: tabBtn.active
            anchors.centerIn: parent

            onToggled: {
              tabBtn.component.enabled = swc.checked
              tabBtn.component.activate(swc.checked)
              sysModel.settingsChanged()
            }
          }
        }
      }

      background: Rectangle {
        color: tabBtn.hovered ? Style.TabButton.bg_color_alt
                              : Style.TabButton.bg_color
      }
    }
  }

  ColumnLayout {
    spacing: 0
    anchors.fill: parent

    TabBar {
      id: tabBar
      Layout.fillWidth: true
    }

    StackLayout {
      id: sysModelplug
      objectName: "SYS_MODEL_Plug"
      currentIndex: tabBar.currentIndex

      property var childrenAdded: []
      function isNewChild(child) {
        for (var i = 0; i < childrenAdded.length; ++i)
          if (childrenAdded[i] === child)
            return false

        return true
      }

      onChildrenChanged: {
        for (var i = 0; i < children.length; ++i) {

          var sysComponent = children[i]
          if (isNewChild(sysComponent)) {
            childrenAdded.push(sysComponent)

            // create and setup the new component
            var tab = tabButtonComponent.createObject(tabBar)
            tab.component = sysComponent

            tabBar.addItem(tab)
            sysModel.setupChild(sysComponent)
          }
        }
      }
    }
  }
}
