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
import "Style.js" as Style

Pane {
  ColumnLayout {
    spacing: 20

    RowLayout {
      spacing: 20

      Image {
        source: "qrc:/images/AppIcon"
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
