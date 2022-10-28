// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import "Style.js" as Style

Pane {

  width: parent.width
  height: parent.height

  padding: 2

  function addInfo(infoPairArray) {
    for (var i = 0; i < infoPairArray.length; i+=2) {
      infoModel.append({ "_title": infoPairArray[i],
                         "_text": infoPairArray[i + 1]})
    }
  }

  ListModel {
    id: infoModel
  }

  Component {
    id: infoUnit

    Pane {
      implicitWidth: 237
      implicitHeight: 60

      padding: Style.RectItem.padding

      background: Rectangle {
        color: hovered ? Style.RectItem.bg_color_hover
                       : Style.RectItem.bg_color
      }

      ToolTip.delay: Style.ToolTip.delay
      ToolTip.timeout: Style.ToolTip.timeout
      ToolTip.visible: hovered
      ToolTip.text: title.text + ": " + info.text

      ColumnLayout {
        spacing: 4
        anchors.fill: parent

        Label {
          id: title
          text: _title
          elide: Text.ElideRight
          color: Style.RectItem.text_color
          Layout.fillWidth: true
        }

        Label {
          id: info
          text: _text
          elide: Text.ElideRight
          color: Style.RectItem.text_color_alt
          Layout.fillWidth: true
        }
      }
    }
  }

  ScrollView {
    id: scrollview
    anchors.fill: parent
    clip: true
    ScrollBar.horizontal.policy: ScrollBar.AlwaysOn
    ScrollBar.vertical.policy: ScrollBar.AlwaysOn
    ScrollBar.horizontal.visible: ScrollBar.horizontal.size < 1
    ScrollBar.vertical.visible: ScrollBar.vertical.size < 1

    Flow {
      id: flow
      width: scrollview.availableWidth
      height: scrollview.availableHeight
      spacing: Style.RectItemList.items_spacing

      Repeater {
        model: infoModel
        delegate: infoUnit
      }

      move: Transition {
        NumberAnimation { properties: "x,y"; easing.type: Easing.InOutQuad }
      }
    }
  }
}
