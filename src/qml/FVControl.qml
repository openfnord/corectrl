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
  property bool showVolt: true
  property alias voltManual: manualVolt.checked

  signal activeStateChanged(int index, bool active)
  signal stateChanged(int index, int freq, int volt)

  function setFVStates(states, activeStates, freqMin, freqMax, voltMin, voltMax) {
    stateModel.clear()

    for (var i = 0; i < states.length; i += 3) {

      var activeState = false
      for (var j = 0; j < activeStates.length; ++j)
        if (activeStates[j] === states[i]) {
          activeState = true
          break
        }

      stateModel.append({"_index": states[i],
                         "_active": activeState,
                         "_freq": states[i + 1],
                         "_freqMin": freqMin,
                         "_freqMax": freqMax,
                         "_volt": states[i + 2],
                         "_voltMin": voltMin,
                         "_voltMax": voltMax})
    }
  }

  function setFStates(states, activeStates, freqMin, freqMax) {
    stateModel.clear()

    for (var i = 0; i < states.length; i += 2) {

      var activeState = false
      for (var j = 0; j < activeStates.length; ++j)
        if (activeStates[j] === states[i]) {
          activeState = true
          break
        }

      stateModel.append({"_index": states[i],
                         "_active": activeState,
                         "_freq": states[i + 1],
                         "_freqMin": freqMin,
                         "_freqMax": freqMax,
                         "_volt": 0,
                         "_voltMin": 0,
                         "_voltMax": 0})
    }
  }

  function updateFVState(index, freq, volt) {
    for (var i = 0; i < stateModel.count; ++i) {
      if (stateModel.get(i)._index === index) {
        stateModel.set(i, {"_freq": freq, "_volt": volt})
        break
      }
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

  function setVoltRange(min, max) {
    for (var i = 0; i < stateModel.count; ++i)
      stateModel.set(i, {"_voltMin": min, "_voltMax": max})
  }

  function activeStates(indices) {
    if (indices.length > 0) {
      var activeStatesCount = 0;
      for (var i = 0; i < stateModel.count; ++i) {
        var stateIndex = stateModel.get(i)._index

        var activeState = false
        for (var j = 0; j < indices.length; ++j)
          if (indices[j] === stateIndex) {
            activeState = true
            ++activeStatesCount
            break
          }

        stateModel.set(i, {"_active": activeState})
      }

      stateModel.activeStatesCount = activeStatesCount
    }
  }

  TextMetrics {
    id: tFMetrics
    text: "9999"
  }

  ListModel {
    id: stateModel

    property int activeStatesCount: 0
  }

  Component {
    id: stateCtl

    ColumnLayout {
      Label {
        text: qsTr("STATE") + " " + _index
        enabled: stateCkb.checked
        Layout.alignment: Qt.AlignHCenter
      }

      CheckBox {
        id: stateCkb
        checked: _active
        enabled: stateModel.activeStatesCount === 1 && !checked ||
                 stateModel.activeStatesCount > 1

        Layout.alignment: Qt.AlignHCenter
        scale: 0.75
        topPadding: -1
        bottomPadding: -1

        onToggled: control.activeStateChanged(_index, checked)
      }

      Slider {
        id: stateFreqSld
        enabled: stateCkb.checked

        Layout.alignment: Qt.AlignHCenter
        orientation: Qt.Vertical

        value: _freq
        from: _freqMin
        to: _freqMax
        stepSize: 1

        onPressedChanged: {
          if (!pressed)
            control.stateChanged(_index, value, _volt)
        }
      }

      CIntInput {
        value: stateFreqSld.value
        minValue: stateFreqSld.from
        maxValue: stateFreqSld.to

        enabled: stateCkb.checked
        Layout.preferredWidth: tFMetrics.width + padding * 2

        onValueChanged: control.stateChanged(_index, value, _volt)
      }

      CIntInput {
        value: _volt
        minValue: _voltMin
        maxValue: _voltMax

        enabled: stateCkb.checked
        visible: showVolt && manualVolt.checked
        Layout.preferredWidth: tFMetrics.width + padding * 2

        onValueChanged: control.stateChanged(_index, _freq, value)
      }

      TextField {
        text: qsTr("Auto")

        enabled: false
        visible: showVolt && !manualVolt.checked

        padding: Style.TextField.padding
        horizontalAlignment: TextInput.AlignHCenter
        Layout.preferredWidth: tFMetrics.width + padding * 2
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

        ColumnLayout {
          visible: showVolt

          Item { Layout.preferredHeight: 5 }

          RowLayout {
            Item { Layout.fillWidth: true }

            Item {
              implicitWidth: manualVolt.width / (1 + Style.g_tweakScale)

              Switch {
                id: manualVolt

                scale: Style.g_tweakScale
                anchors.centerIn: parent
              }
            }

            Label { text: qsTr("Voltage") + " (mV)" }
          }

          Item { Layout.preferredHeight: 10 }
        }
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
