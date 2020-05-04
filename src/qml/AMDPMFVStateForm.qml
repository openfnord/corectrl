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

AMD_PM_FV_STATE {
  id: pmFVState
  objectName: "AMD_PM_FV_STATE"

  width: contents.width
  height: contents.height

  onVoltRangeChanged: {
    p.voltMin = min
    p.voltMax = max

    gpufv.setVoltRange(min, max)
    memfv.setVoltRange(min, max)
  }
  onGpuFreqRangeChanged: {
    p.gpuMin = min
    p.gpuMax = max

    gpufv.setFreqRange(min, max)
  }
  onMemFreqRangeChanged: {
    p.memMin = min
    p.memMax = max

    memfv.setFreqRange(min, max)
  }

  onGpuVoltModeChanged: gpufv.voltManual = mode === "manual"
  onMemVoltModeChanged: memfv.voltManual = mode === "manual"

  onGpuStatesChanged: gpufv.setFVStates(states, p.gpuActiveStates,
                                        p.gpuMin, p.gpuMax,
                                        p.voltMin, p.voltMax)
  onMemStatesChanged: memfv.setFVStates(states, p.memActiveStates,
                                        p.memMin, p.memMax,
                                        p.voltMin, p.voltMax)

  onGpuActiveStatesChanged: {
    p.gpuActiveStates.length = 0
    p.gpuActiveStates = states
    gpufv.activeStates(states)
  }
  onMemActiveStatesChanged: {
    p.memActiveStates.length = 0
    p.memActiveStates = states
    memfv.activeStates(states)
  }

  onGpuStateChanged: gpufv.updateFVState(index, freq, volt)
  onMemStateChanged: memfv.updateFVState(index, freq, volt)

  QtObject {
    id: p

    property int gpuMin: 0
    property int gpuMax: 0
    property int memMin: 0
    property int memMax: 0
    property int voltMin: 0
    property int voltMax: 0

    property var gpuActiveStates: []
    property var memActiveStates: []
  }

  Pane {
    id: contents
    padding: Style.g_padding

    RowLayout {

      FVControl {
        id: gpufv
        Layout.fillHeight: true

        title: qsTr("GPU")

        onActiveStateChanged: pmFVState.changeGPUActiveState(index, active)
        onStateChanged: pmFVState.changeGPUState(index, freq, volt)
        onVoltManualChanged: pmFVState.changeGPUVoltMode(voltManual ? "manual" : "auto")
      }

      FVControl {
        id: memfv
        Layout.fillHeight: true

        title: qsTr("Memory")

        onActiveStateChanged: pmFVState.changeMemActiveState(index, active)
        onStateChanged: pmFVState.changeMemState(index, freq, volt)
        onVoltManualChanged: pmFVState.changeMemVoltMode(voltManual ? "manual" : "auto")
      }
    }
  }
}
