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
import QtQuick.Layouts 1.3
import Radman.UIComponents 1.0
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
