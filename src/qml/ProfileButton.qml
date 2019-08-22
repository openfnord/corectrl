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

Pane {
  id: btn

  property alias label: btnText.text
  property string name: ""
  property alias icon : icn.source
  property string exe
  property bool isGlobal: false
  property bool profileActivated: true

  implicitWidth: 237
  implicitHeight: 60

  padding: 0

  signal clicked()
  signal enableProfile(bool enable)
  signal edit()
  signal clone()
  signal exportTo()
  signal remove()

  ToolTip.delay: Style.ToolTip.delay
  ToolTip.timeout: Style.ToolTip.timeout
  ToolTip.visible: hovered
  ToolTip.text: label

  MouseArea {
    anchors.fill: parent
    propagateComposedEvents: true
    onClicked: btn.clicked()
  }

  background: Rectangle {
    color: btn.hovered ? Style.RectItem.bg_color_hover
                       : btn.profileActivated ? Style.RectItem.bg_color
                                              : Style.RectItem.bg_color_alt
  }

  RowLayout {
    anchors.fill: parent
    spacing: 0

    Item {
      Layout.fillHeight: true
      Layout.alignment: Qt.AlignLeft
      Layout.leftMargin: Style.RectItem.padding

      width: icn.width
      height: icn.height

      Image {
        id: icn
        cache: false
        anchors.centerIn: parent
        width: Style.g_icon.size
        height: Style.g_icon.size
        sourceSize.width: Style.g_icon.source_size
      }
    }

    Item {
      Layout.fillWidth: true
      Layout.fillHeight: true
      Layout.leftMargin: 10

      Label {
        id: btnText

        width: parent.width
        anchors.centerIn: parent

        elide: Text.ElideRight
        wrapMode: Text.WordWrap

        color: btn.profileActivated ?
                 Style.RectItem.text_color :
                 Style.RectItem.text_color_alt

        onTextChanged: {
          if (parent.height > 0) {
            height = parent.height
            height = Math.min(contentHeight, height)
          }
        }

        onContentSizeChanged: {
          // set height dynamically, to obtain centered wrapped and unwrapped text
          if (parent.height > 0)
            height = Math.min(contentHeight, parent.height)
        }
      }
    }

    Item {
      Layout.fillHeight: true
      Layout.alignment: Qt.AlignRight
      Layout.rightMargin: Style.RectItem.padding

      implicitWidth: editBtn.width

      ToolButton {
        id: editBtn

        text: Style.g_icon.MENU;
        font.pointSize: Style.g_text.icon_size

        anchors.centerIn: parent

        onClicked: menu.open()

        Menu {
          id: menu

          MenuItem {
            text: btn.profileActivated ? qsTr("Disable") : qsTr("Enable")
            enabled: !isGlobal
            hoverEnabled: Style.g_hover

            onTriggered: {
              btn.profileActivated = !btn.profileActivated
              btn.enableProfile(btn.profileActivated)
            }
          }

          MenuItem {
            text: qsTr("Edit...")
            enabled: !isGlobal
            hoverEnabled: Style.g_hover

            onTriggered: btn.edit()
          }

          MenuItem {
            text: qsTr("Clone...")
            hoverEnabled: Style.g_hover

            onTriggered: btn.clone()
          }

          MenuItem {
            text: qsTr("Export to...")
            hoverEnabled: Style.g_hover

            onTriggered: btn.exportTo()
          }

          MenuSeparator {}

          MenuItem {
            text: qsTr("Remove")
            enabled: !isGlobal
            hoverEnabled: Style.g_hover

            onTriggered: btn.remove()
          }
        }
      }
    }
  }
}
