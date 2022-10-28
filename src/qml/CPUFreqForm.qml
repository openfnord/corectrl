// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import CoreCtrl.UIComponents 1.0
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

  Pane {
    id: contents
    padding: Style.g_padding

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
