// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import CoreCtrl.UIComponents 1.0
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
