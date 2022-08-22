//
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>
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
