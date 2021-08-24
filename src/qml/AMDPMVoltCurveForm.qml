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
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3
import CoreCtrl.UIComponents 1.0
import "Style.js" as Style

AMD_PM_VOLT_CURVE {
  id: pmVoltCurve
  objectName: "AMD_PM_VOLT_CURVE"

  width: contents.width
  height: contents.height

  onModeChanged: modeSw.checked = mode === "manual"
  onPointsRangeChanged: {
    voltCurve.configureAxes(qsTr("Frequency"), "MHz", freqMin, freqMax,
                            qsTr("Voltage"), "mV", voltMin, voltMax)
  }
  onPointsChanged: {
    voltCurve.removeCurve("volt")
    voltCurve.addCurve("volt", Material.accent, points)
  }

  Pane {
    id: contents
    padding: 0

    RowLayout {

      Pane {
        Layout.fillHeight: true

        padding: Style.g_padding
        background: Rectangle {
          border.color: Style.FVControl.border_color
          border.width: 2
          color: "#00000000"
        }

        ColumnLayout {
          anchors.fill: parent

          RowLayout {
            Label {
              text: qsTr("Voltage")
              font.pointSize: 11
              font.bold: true
            }

            Item {
              implicitWidth: modeSw.width / (1 + Style.g_tweakScale)

              Switch {
                id: modeSw

                scale: Style.g_tweakScale
                anchors.centerIn: parent

                onToggled: pmVoltCurve.changeMode(checked ? "manual" : "auto")
              }
            }

            Item { Layout.fillWidth: true }
          }

          CurveControl {
            id: voltCurve
            Layout.alignment: Qt.AlignCenter

            minXDistance: 5

            xTickCount: 3
            xMinorTickCount: 1
            yTickCount: 3
            yMinorTickCount: 1

            width: 240
            height: 240

            enabled: modeSw.checked

            onCurveChanged: pmVoltCurve.updatePoint(oldPoint, newPoint)
          }
        }
      }
    }
  }
}
