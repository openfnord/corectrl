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
