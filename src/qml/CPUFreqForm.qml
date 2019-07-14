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
import Radman.UIComponents 1.0
import "Style.js" as Style

CPU_CPUFREQ {
  id: cpuFreq
  objectName: "CPU_CPUFREQ"

  width: contents.width
  height: contents.height

  onScalingGovernorsChanged: {
    scalingGovernorListModel.clear()

    for (var i = 0; i < governors.length; i+=2) {
      var element = scalingGovernorListElement.createObject()
      element.governor= governors[i]
      element.text = governors[i + 1]
      scalingGovernorListModel.append(element)
    }

    cbScalingGovernor.updateWidth()
  }

  onScalingGovernorChanged: {
    for (var i = 0; i < scalingGovernorListModel.count; ++i) {
      if (scalingGovernorListModel.get(i).governor === governor) {
        cbScalingGovernor.lastIndex = i
        cbScalingGovernor.currentIndex = i
        break;
      }
    }
  }

  ListModel { id: scalingGovernorListModel }
  Component {
    id: scalingGovernorListElement

    ListElement {
      property string text
      property string governor
    }
  }

  Page {
    id: contents

    header: Pane {
      leftPadding: Style.ModeSelector.header.padding
      rightPadding: Style.ModeSelector.header.padding
      topPadding: 25
      bottomPadding: 24
      Material.background: Style.ModeSelector.header.bg_color

      Row { Label { text: qsTr("CPU performance scaling") } }
    }

    Pane {
      anchors.fill: parent
      padding: Style.g_padding
      Material.background: Style.ModeSelector.body.bg_color

      RowLayout {
        Label {
          text: qsTr("Frequency governor")
          rightPadding: 6
        }

        CComboBox {
          id: cbScalingGovernor
          model: scalingGovernorListModel

          property int lastIndex: 0

          onActivated: {
            if (lastIndex !== currentIndex) {
              lastIndex = currentIndex

              var governor = model.get(currentIndex).governor
              cpuFreq.changeScalingGovernor(governor)
            }
          }
        }
      }
    }
  }
}
