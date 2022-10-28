// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import CoreCtrl.UIComponents 1.0
import "Style.js" as Style

NOOP {
  objectName: "NOOP"

  width: contents.width
  height: contents.height

  Pane {
    id: contents
    padding: Style.g_padding

    RowLayout {
      anchors.fill: parent

      Item {
        Layout.rightMargin: Style.g_padding
        implicitWidth: warningIcn.width
        implicitHeight: warningIcn.height

        Image {
          id: warningIcn
          source: "qrc:/images/WarningIcon"
          fillMode: Image.PreserveAspectFit
          anchors.fill: parent
          width: Style.g_icon.size
          height: Style.g_icon.size
          sourceSize.width: Style.g_icon.source_size
        }
      }

      ColumnLayout {
        Label {
          text: qsTr("Warning!")
          font.pointSize: 12
          font.bold: true
        }

        Label { text: qsTr("The component will not be controlled") }
      }
    }
  }
}
