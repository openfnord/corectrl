// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
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
