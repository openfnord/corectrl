// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "Style.js" as Style

Pane {
  id: btn

  property alias label: btnText.text
  property string name: ""
  property alias icon : icn.source
  property string exe: ""
  property bool isGlobal: false
  property bool profileActivated: true
  property alias toggledManual: manualSwc.checked

  implicitWidth: 240
  implicitHeight: 60

  padding: 0

  signal clicked()
  signal enableProfile(bool enable)
  signal toggleManualProfile(string name)
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
        fillMode: Image.PreserveAspectFit
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

      implicitWidth: manualSwc.width / 3

      Switch {
        id: manualSwc
        visible: exe.length == 0

        padding: 0
        rotation: -90
        scale: Style.g_tweakScale
        anchors.centerIn: parent

        onToggled: btn.toggleManualProfile(name)
      }
    }

    Item {
      Layout.fillHeight: true
      Layout.alignment: Qt.AlignRight
      Layout.rightMargin: 0

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
            enabled: !isGlobal && exe.length > 0
            hoverEnabled: Style.g_hover

            onTriggered: btn.enableProfile(!btn.profileActivated)
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
