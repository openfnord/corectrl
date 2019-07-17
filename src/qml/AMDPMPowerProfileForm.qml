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

AMD_PM_POWER_PROFILE {
  id: pmPowerProfile
  objectName: "AMD_PM_POWER_PROFILE"

  width: contents.width
  height: contents.height

  onModeChanged: {
    for (var i = 0; i < listModel.count; ++i) {
      if (listModel.get(i).mode === mode) {
        cbMode.lastIndex = i
        cbMode.currentIndex = i
        break;
      }
    }
  }

  onModesChanged: {
    listModel.clear()

    for (var i = 0; i < modes.length; i+=2) {
      var element = listElement.createObject()
      element.mode = modes[i]
      element.text = modes[i + 1]
      listModel.append(element)
    }

    cbMode.updateWidth()
  }

  ListModel { id: listModel }
  Component {
    id: listElement

    ListElement {
      property string text
      property string mode
    }
  }

  Pane {
    id: contents
    padding: Style.g_padding

    RowLayout {
      Label {
        text: qsTr("Power profile")
        rightPadding: 6
      }

      CComboBox {
        id: cbMode
        model: listModel

        property int lastIndex: 0

        onActivated: {
          if (lastIndex !== currentIndex) {
            lastIndex = currentIndex

            var mode = model.get(currentIndex).mode
            pmPowerProfile.changeMode(mode)
          }
        }
      }
    }
  }
}
