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

AMD_PM_FREQ_VOLT {
  id: pmFreqVolt
  objectName: "AMD_PM_FREQ_VOLT"

  width: contents.width
  height: contents.height

  onControlLabelChanged: fv.title = label

  onVoltRangeChanged: {
    p.voltMin = min
    p.voltMax = max

    fv.setVoltRange(min, max)
  }

  onFreqRangeChanged: {
    p.freqMin = min
    p.freqMax = max

    fv.setFreqRange(min, max)
  }

  onVoltModeChanged: fv.voltManual = mode === "manual"

  onStatesChanged: fv.setFVStates(states, p.activeStates,
                                  p.freqMin, p.freqMax,
                                  p.voltMin, p.voltMax)

  onStateChanged: fv.updateFVState(index, freq, volt)

  onActiveStatesChanged: {
    p.activeStates.length = 0
    p.activeStates = states
    fv.activeStates(states)
  }

  QtObject {
    id: p

    property int freqMin: 0
    property int freqMax: 0
    property int voltMin: 0
    property int voltMax: 0

    property var activeStates: []
  }

  Pane {
    id: contents
    padding: 0

    RowLayout {
      FVControl {
        id: fv
        Layout.fillHeight: true

        onVoltManualChanged: pmFreqVolt.changeVoltMode(voltManual ? "manual" : "auto")
        onStateChanged: pmFreqVolt.changeState(index, freq, volt)
        onActiveStateChanged: pmFreqVolt.changeActiveState(index, active)
      }
    }
  }
}
