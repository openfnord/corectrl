// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import CoreCtrl.UIComponents 1.0
import "Style.js" as Style

AMD_PM_FREQ_OD {
  id: pmFreqOd
  objectName: "AMD_PM_FREQ_OD"

  width: contents.width
  height: contents.height

  onSclkOdChanged: sclkOd.value = value
  onMclkOdChanged: mclkOd.value = value

  onSclkChanged: sclkFreq.text = value;
  onMclkChanged: mclkFreq.text = value;


  TextMetrics {
    id: freqTFMetrics
    text: "0000 MHz"
  }

  TextMetrics {
    id: odTFMetrics
    text: "+ 00 %"
  }

  Pane {
    id: contents
    padding: Style.g_padding

    ColumnLayout {
      spacing: 0

      RowLayout {
        spacing: 8

        Item { Layout.fillWidth: true }

        Label { text: qsTr("GPU") }

        Slider {
          id: sclkOd

          value: 0
          from: 0
          to: 20
          stepSize: 1

          onMoved: pmFreqOd.changeSclkOd(sclkOd.value)
        }

        Label {
          text: "+ " + sclkOd.value + " %"
          horizontalAlignment: Text.AlignRight
          Layout.preferredWidth: odTFMetrics.width
        }

        Label {
          id: sclkFreq
          horizontalAlignment: Text.AlignRight
          Layout.preferredWidth: freqTFMetrics.width
        }
      }

      RowLayout {
        spacing: 8

        Item { Layout.fillWidth: true }

        Label { text: qsTr("Memory") }

        Slider {
          id: mclkOd

          value: 0
          from: 0
          to: 20
          stepSize: 1

          onMoved: pmFreqOd.changeMclkOd(mclkOd.value)
        }

        Label {
          text: "+ " + mclkOd.value + " %"
          horizontalAlignment: Text.AlignRight
          Layout.preferredWidth: odTFMetrics.width
        }

        Label {
          id: mclkFreq
          horizontalAlignment: Text.AlignRight
          Layout.preferredWidth: freqTFMetrics.width
        }
      }
    }
  }
}
