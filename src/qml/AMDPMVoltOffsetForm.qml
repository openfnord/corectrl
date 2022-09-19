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
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls.Material 2.15
import CoreCtrl.UIComponents 1.0
import "Style.js" as Style

AMD_PM_VOLT_OFFSET {
  id: pmVoltOffset
  objectName: "AMD_PM_VOLT_OFFSET"

  width: contents.width
  height: contents.height

  onValueChanged: {
    offsetSld.value = value
  }

  onRangeChanged: {
    offsetSld.from = min
    offsetSld.to = max
  }

  Pane {
    id: contents
    padding: Style.g_padding

    background: Rectangle {
      border.color: Style.FVControl.border_color
      border.width: 2
      color: "#00000000"
    }

    TextMetrics {
      id: tFMetrics
      text: "-000"
    }

    GridLayout {
      columns: 2
      anchors.fill: parent

      RowLayout {
        Layout.columnSpan: 2

        Label {
          id: title
          text: qsTr("Voltage")
          font.pointSize: 11
          font.bold: true
        }

        Item { Layout.fillWidth: true }

        Item {
          visible: offsetSld.value > 0

          implicitWidth: warningIcn.width
          implicitHeight: warningIcn.height

          ToolTip.text: qsTr("WARNING: Operating range not available. Use with caution!")
          ToolTip.visible: ma.containsMouse
          MouseArea {
              id: ma
              anchors.fill: parent
              hoverEnabled: true
          }

          Image {
            id: warningIcn
            source: "qrc:/images/WarningIcon"
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
            width: Style.g_icon.small_size
            height: Style.g_icon.small_size
            sourceSize.width: Style.g_icon.size
          }
        }
      }

      Pane {
        padding: Style.FVControl.inner_padding
        bottomPadding: 0
        Layout.columnSpan: 2

        ColumnLayout {
          Layout.fillWidth: true

          Label {
            text: qsTr("OFFSET")
            Layout.alignment: Qt.AlignHCenter
          }

          Label {
            text: "mV"
            font.pointSize: 8
            Layout.alignment: Qt.AlignHCenter
          }

          Slider {
            id: offsetSld

            Layout.alignment: Qt.AlignHCenter
            orientation: Qt.Vertical

            stepSize: 1

            onPressedChanged: {
              if (!pressed)
                pmVoltOffset.changeValue(value)
            }
          }

          CIntInput {
            value: offsetSld.value
            minValue: offsetSld.from
            maxValue: offsetSld.to
            Layout.preferredWidth: tFMetrics.width + padding * 2
            Layout.alignment: Qt.AlignHCenter

            onValueChanged: pmVoltOffset.changeValue(value)
          }
        }
      }
    }
  }
}
