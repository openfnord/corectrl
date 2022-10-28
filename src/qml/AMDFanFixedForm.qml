// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import CoreCtrl.UIComponents 1.0
import "Style.js" as Style

AMD_FAN_FIXED {
  id: fanFixed
  objectName: "AMD_FAN_FIXED"

  width: contents.width
  height: contents.height

  onValueChanged: fanValue.value = value
  onFanStopChanged: fanStop.checked = enabled
  onFanStartValueChanged: fanStartValue.value = value

  TextMetrics {
    id: tFMetrics
    text: "100"
  }

  Pane {
    id: contents
    padding: Style.g_padding

    ColumnLayout {
      RowLayout {
        spacing: 8

        Slider {
          id: fanValue

          from: 0
          to: 100
          stepSize: 1

          onPressedChanged: {
            if (!pressed)
              fanFixed.changeValue(fanValue.value)
          }
        }

        CIntInput {
          value: fanValue.value
          minValue: fanValue.from
          maxValue: fanValue.to

          Layout.preferredWidth: tFMetrics.width + padding * 2

          onValueChanged: {
            fanValue.value = value
            fanFixed.changeValue(value)
          }
        }
      }

      RowLayout {
        spacing: 8

        Item {
          Layout.fillWidth: true
        }

        Item {
          implicitWidth: fanStop.width / (1 + Style.g_tweakScale)

          Switch {
            id: fanStop

            scale: Style.g_tweakScale
            anchors.centerIn: parent

            onToggled: fanFixed.enableFanStop(checked)
          }
        }

        Label {
          text: qsTr("Fan start")
          enabled: fanStop.checked
        }

        CIntInput {
          id: fanStartValue

          enabled: fanStop.checked

          minValue: fanValue.from
          maxValue: fanValue.to

          Layout.preferredWidth: tFMetrics.width + padding * 2

          onValueChanged: fanFixed.changeFanStartValue(value)
        }
      }
    }
  }
}
