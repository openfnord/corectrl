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
