// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import CoreCtrl.UIComponents 1.0

AMD_PM_FREQ_RANGE {
  id: pmFreqRange
  objectName: "AMD_PM_FREQ_RANGE"

  width: contents.width
  height: contents.height

  onControlLabelChanged: freqState.title = label

  onStateRangeChanged: {
    p.min = min
    p.max = max

    freqState.setFreqRange(min, max)
  }

  onStatesChanged: freqState.setFStates(states, p.min, p.max)
  onStateChanged: freqState.updateFState(index, freq)

  QtObject {
    id: p

    property int min: 0
    property int max: 0
  }

  Pane {
    id: contents
    padding: 0

    RowLayout {
      FreqStateControl {
        id: freqState
        Layout.fillHeight: true

        onStateChanged: pmFreqRange.changeState(index, freq)
      }
    }
  }
}
