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

AMD_PM_POWERCAP {
  id: powerCap
  objectName: "AMD_PM_POWERCAP"

  width: contents.width
  height: contents.height

  onValueChanged: {
    powerValue.value = value
    valueInput.value = value
  }
  onRangeChanged: {
    powerValue.from = min
    powerValue.to = max
  }

  TextMetrics {
    id: tFMetrics
    text: "999"
  }

  Pane {
    id: contents
    padding: Style.g_padding

    RowLayout {

      Label { text: qsTr("Power limit") + " (W)" }

      Slider {
        id: powerValue

        from: 0
        to: 0
        stepSize: 1

        onPressedChanged: {
          if (!pressed)
            powerCap.changeValue(powerValue.value)
        }

        onMoved: valueInput.value = value
      }

      CIntInput {
        id: valueInput

        Layout.preferredWidth: tFMetrics.width + padding * 2

        minValue: powerValue.from
        maxValue: powerValue.to

        onValueChanged: powerCap.changeValue(value)
      }
    }
  }
}
