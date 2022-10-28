// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
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
    anchors.fill: parent

    RowLayout {
      anchors.fill: parent

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
            Layout.fillHeight: true

            minXDistance: 5

            xTickCount: 3
            xMinorTickCount: 1
            yTickCount: 3
            yMinorTickCount: 1

            width: 400
            height: 299

            enabled: modeSw.checked

            onCurveChanged: pmVoltCurve.updatePoint(oldPoint, newPoint)
          }
        }
      }
    }
  }
}
