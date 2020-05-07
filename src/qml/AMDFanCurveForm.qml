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

AMD_FAN_CURVE {
  id: fanCurve
  objectName: "AMD_FAN_CURVE"

  width: contents.width
  height: contents.height

  onCurveChanged: {
    curveControl.removeCurve("curve")
    curveControl.addCurve("curve", Material.accent, points)
  }

  onFanStopChanged: {
    if (enabled)
      p.showFanStartCurve()
    else
      p.hideFanStartCurve()

    fanStop.checked = enabled
  }

  onFanStartValueChanged: {
    if (fanCurve.fanStop) {
      p.hideFanStartCurve()
      p.showFanStartCurve()
    }

    fanStartValue.value = value
  }

  onTemperatureRangeChanged: {
    curveControl.configureAxes(qsTr("Temperature"), "\u00B0C", min, max,
                               qsTr("PWM"), "%", 0, 100)

    // Reposition fanStart curve as its control point position depends
    // on temperature range
    if (fanCurve.fanStop) {
      p.hideFanStartCurve()
      p.showFanStartCurve()
    }
  }

  QtObject { // private stuff
    id: p

    function hideFanStartCurve() {
      curveControl.removeCurve("fanStart")
    }

    function showFanStartCurve() {
      var points = []
      points.push(Qt.point(fanCurve.maxTemp -
                          (fanCurve.maxTemp - fanCurve.minTemp) * .15,
                          fanCurve.fanStartValue))
      curveControl.addCurve("fanStart",
                            Style.CurveControl.curve_amd_start_color,
                            points)
    }
  }

  TextMetrics {
    id: tFMetrics
    text: "100"
  }

  Pane {
    id: contents
    padding: Style.g_padding

    ColumnLayout {
      spacing: 8

      CurveControl {
        id: curveControl
        minXDistance: 5
        width: 480
        height: 240

        onCurveChanged: {
          if (name === "curve")
            fanCurve.updateCurvePoint(oldPoint, newPoint)
          else if (name === "fanStart")
            fanCurve.fanStartValue = parseInt(Math.round(newPoint.y))
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

            onToggled: fanCurve.fanStop = checked
          }
        }

        Label {
          text: qsTr("Fan start")
          enabled: fanStop.checked
        }

        CIntInput {
          id: fanStartValue

          minValue: 0
          maxValue: 100

          enabled: fanStop.checked
          Layout.preferredWidth: tFMetrics.width + padding * 2

          onValueChanged: fanCurve.fanStartValue = value
        }
      }
    }
  }
}
