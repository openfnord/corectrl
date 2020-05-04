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
import "Style.js" as Style

Pane {
  id: control

  padding: Style.g_padding
  background: Rectangle {
    border.color: Style.FVControl.border_color
    border.width: 2
    color: "#00000000"
  }

  property alias title: title.text

  signal activeStateChanged(int index, bool active)
  signal stateChanged(int index, int freq)

  function setFStates(states, freqMin, freqMax) {
    stateModel.clear()

    for (var i = 0; i < states.length; i += 2) {

      stateModel.append({"_index": states[i],
                         "_freq": states[i + 1],
                         "_freqMin": freqMin,
                         "_freqMax": freqMax})
    }
  }

  function updateFState(index, freq) {
    for (var i = 0; i < stateModel.count; ++i) {
      if (stateModel.get(i)._index === index) {
        stateModel.set(i, {"_freq": freq})
        break
      }
    }
  }

  function setFreqRange(min, max) {
    for (var i = 0; i < stateModel.count; ++i)
      stateModel.set(i, {"_freqMin": min, "_freqMax": max})
  }

  TextMetrics {
    id: tFMetrics
    text: "9999"
  }

  ListModel {
    id: stateModel

    function getStateLabel(stateIndex) {
      if (stateIndex === 0)
        return qsTr("MINIMUM")
      else if (stateModel.count === 1 || stateIndex === stateModel.count - 1)
        return qsTr("MAXIMUM")
      else
        return qsTr("STATE") + " " + stateIndex
    }
  }

  Component {
    id: stateCtl

    ColumnLayout {
      Label {
        text: stateModel.getStateLabel(_index)
        Layout.alignment: Qt.AlignHCenter
      }

      Slider {
        id: stateFreqSld

        Layout.alignment: Qt.AlignHCenter
        orientation: Qt.Vertical

        value: _freq
        from: _freqMin
        to: _freqMax
        stepSize: 1

        onPressedChanged: {
          if (!pressed)
            control.stateChanged(_index, value)
        }
      }

      CIntInput {
        value: stateFreqSld.value
        minValue: stateFreqSld.from
        maxValue: stateFreqSld.to
        Layout.preferredWidth: tFMetrics.width + padding * 2

        onValueChanged: control.stateChanged(_index, value)
      }
    }
  }

  ColumnLayout {
    anchors.fill: parent

    GridLayout {
      columns: 2

      ColumnLayout {
        Layout.fillHeight: true

        Label {
          id: title
          font.pointSize: 11
          font.bold: true
        }

        Item { Layout.fillHeight: true }

        Label {
          id: freqLbl
          text: qsTr("Frequency") + " (MHz)"
          Layout.alignment: Qt.AlignRight
        }

        Item { Layout.preferredHeight: 10 }
      }

      RowLayout {
        id: freqStates

        spacing: 15
        Layout.fillWidth: true

        Repeater {
          model: stateModel
          delegate: stateCtl
        }
      }
    }
  }
}
