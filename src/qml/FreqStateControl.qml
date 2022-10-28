// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
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

      Label {
        text: "MHz"
        font.pointSize: 8
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
        Layout.alignment: Qt.AlignHCenter

        onValueChanged: control.stateChanged(_index, value)
      }
    }
  }

  ColumnLayout {
    anchors.fill: parent

    GridLayout {
      columns: 2

      Label {
        id: title
        font.pointSize: 11
        font.bold: true
        Layout.columnSpan: 2
      }

      Pane {
        padding: Style.FVControl.inner_padding
        bottomPadding: 0
        Layout.columnSpan: 2

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
}
