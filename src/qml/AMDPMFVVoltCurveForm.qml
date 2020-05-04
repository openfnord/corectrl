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

AMD_PM_FV_VOLTCURVE {
  id: pmFVVoltCurve
  objectName: "AMD_PM_FV_VOLTCURVE"

  width: contents.width
  height: contents.height

  onVoltRangeChanged: {
    p.voltMin = min
    p.voltMax = max

    p.configureVoltCurve()
  }
  onGpuFreqRangeChanged: {
    p.gpuMin = min
    p.gpuMax = max

    gpufv.setFreqRange(min, max)
    p.configureVoltCurve()
  }
  onMemFreqRangeChanged: {
    p.memMin = min
    p.memMax = max

    memfv.setFreqRange(min, max)
  }

  onVoltModeChanged: manualVolt.checked = mode === "manual"

  onVoltCurveChanged: {
    voltCurve.removeCurve("volt")
    voltCurve.addCurve("volt", Material.accent, points)
  }

  onGpuStatesChanged: gpufv.setFStates(states, p.gpuMin, p.gpuMax)
  onMemStatesChanged: memfv.setFStates(states, p.memMin, p.memMax)

  onGpuStateChanged: gpufv.updateFState(index, freq)
  onMemStateChanged: memfv.updateFState(index, freq)

  QtObject {
    id: p

    property int gpuMin: 0
    property int gpuMax: 0
    property int memMin: 0
    property int memMax: 0
    property int voltMin: 0
    property int voltMax: 0

    function configureVoltCurve() {
      if (gpuMin != gpuMax && voltMin != voltMax)
        voltCurve.configureAxes(qsTr("Frequency"), "MHz", gpuMin, gpuMax,
                                qsTr("Voltage"), "mV", voltMin, voltMax)
    }
  }

  Pane {
    id: contents
    padding: Style.g_padding

    RowLayout {

      FreqStateControl {
        id: gpufv
        Layout.fillHeight: true

        title: qsTr("GPU")

        onStateChanged: pmFVVoltCurve.changeGPUState(index, freq)
      }

      FreqStateControl {
        id: memfv
        Layout.fillHeight: true

        title: qsTr("Memory")

        onStateChanged: pmFVVoltCurve.changeMemState(index, freq)
      }

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
              implicitWidth: manualVolt.width / (1 + Style.g_tweakScale)

              Switch {
                id: manualVolt

                scale: Style.g_tweakScale
                anchors.centerIn: parent

                onToggled: pmFVVoltCurve.changeVoltMode(checked ? "manual" : "auto")
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

            enabled: manualVolt.checked

            onCurveChanged: pmFVVoltCurve.updateVoltCurvePoint(oldPoint, newPoint)
          }
        }
      }
    }
  }
}
