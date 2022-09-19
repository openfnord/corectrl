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
