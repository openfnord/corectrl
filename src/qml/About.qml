// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import "Style.js" as Style

Pane {
  ColumnLayout {
    spacing: 20

    RowLayout {
      spacing: 20

      Image {
        source: "qrc:/images/AppIcon"
        fillMode: Image.PreserveAspectFit
        width: 128
        height: 128
        sourceSize.width: 128
      }

      ColumnLayout {
        Item { Layout.fillHeight: true }

        Label {
          text: appInfo.name
          font.pointSize: Style.g_text.size + 5
          font.bold: true
        }

        Label {
          text: appInfo.version
          font.pointSize: Style.g_text.size + 2
        }

        Label {
          text: qsTr("Control your hardware with ease using application profiles")
          font.pointSize: Style.g_text.size + 1
        }

        Label {
          text: qsTr("by") + " Juan Palacios"
          font.bold: true
        }
      }

      ColumnLayout {
        Item { Layout.fillHeight: true }

        Label {
          text: qsTr("Links") + ": <ul>" +
                "<li><a href=\"https://gitlab.com/corectrl/corectrl\">" + qsTr("Project") + "</a></li>" +
                "<li><a href=\"https://gitlab.com/corectrl/corectrl/issues\">" + qsTr("Issue tracker") + "</a></li>" +
                "<li><a href=\"https://gitlab.com/corectrl/corectrl/wikis/home\">"+ qsTr("Wiki") +"</a></li>" +
                "<li><a href=\"https://gitlab.com/corectrl/corectrl/wikis/FAQ\">"+ qsTr("FAQ") +"</a></li>" +
                "</ul>"

          font.pointSize: Style.g_text.size + 1
          onLinkActivated: Qt.openUrlExternally(link)

          MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.NoButton
            cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
          }
        }
      }
    }
  }
}
