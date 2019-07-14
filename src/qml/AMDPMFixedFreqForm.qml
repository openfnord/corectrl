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

AMD_PM_FIXED_FREQ {
  id: pmFrequency
  objectName: "AMD_PM_FIXED_FREQ"

  width: contents.width
  height: contents.height

  onSclkIndexChanged: sclkIndex.value = index
  onMclkIndexChanged: mclkIndex.value = index

  onSclkStatesChanged: {
    if (states.length > 0) {

      p.sclkStateLbls.length = 0
      p.sclkStateIndices.length = 0

      for (var i = 0; i < states.length; i += 2) {
        p.sclkStateIndices.push(states[i])
        p.sclkStateLbls.push(states[i + 1])
      }
      sclkIndex.to = p.sclkStateLbls.length - 1
    }
  }

  onMclkStatesChanged: {
    if (states.length > 0) {

      p.mclkStateLbls.length = 0
      p.mclkStateIndices.length = 0

      for (var i = 0; i < states.length; i += 2) {
        p.mclkStateIndices.push(states[i])
        p.mclkStateLbls.push(states[i + 1])
      }
      mclkIndex.to = p.mclkStateLbls.length - 1
    }
  }

  QtObject {
    id: p

    property var sclkStateLbls: []
    property var mclkStateLbls: []
    property var sclkStateIndices: []
    property var mclkStateIndices: []
  }

  TextMetrics {
    id: tFMetrics
    text: "0000 MHz"
  }

  Pane {
    id: contents
    padding: Style.g_padding

    ColumnLayout {
      spacing: 0

      RowLayout {
        spacing: 8
        visible: sclkIndex.from < sclkIndex.to

        Item { Layout.fillWidth: true }

        Label { text: qsTr("GPU") }

        Slider {
          id: sclkIndex

          value: 0
          from: 0
          to: 0
          stepSize: 1

          onPressedChanged: {
            if (!pressed)
              pmFrequency.changeSclkIndex(p.sclkStateIndices[sclkIndex.value])
          }
        }

        Label {
          text: sclkIndex.to > 0 ? p.sclkStateLbls[sclkIndex.value]
                                 : ""
          horizontalAlignment: Text.AlignRight
          Layout.preferredWidth: tFMetrics.width
        }
      }

      RowLayout {
        spacing: 8
        visible: mclkIndex.from < mclkIndex.to

        Item { Layout.fillWidth: true }

        Label { text: qsTr("Memory") }

        Slider {
          id: mclkIndex

          value: 0
          from: 0
          to: 0
          stepSize: 1

          onPressedChanged: {
            if (!pressed)
              pmFrequency.changeMclkIndex(p.mclkStateIndices[mclkIndex.value])
          }
        }

        Label {
          text: mclkIndex.to > 0 ? p.mclkStateLbls[mclkIndex.value]
                                 : ""
          horizontalAlignment: Text.AlignRight
          Layout.preferredWidth: tFMetrics.width
        }
      }
    }
  }
}
