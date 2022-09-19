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
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import CoreCtrl.UIComponents 1.0
import "Style.js" as Style
import "Settings.js" as Settings

GPU {
  id: gpu
  objectName: "GPU"

  onNewGraphItem: {
    sensorGraph.addItem(item)
    Settings.addComponentData("GPU" + gpu.index, "GPU " + gpu.index,
                              item.name,
                              qsTranslate("SensorGraph", item.name))
  }

  ColumnLayout {
    spacing: 0
    anchors.fill: parent

    SensorGraph {
      id: sensorGraph
      Layout.fillWidth: true

      Connections {
        target: settings

        function onSettingChanged(key, value) {
          if (key === "Workarounds/ignoredSensors") {
            var sensors = Settings.componentIgnoredSensors("GPU" + gpu.index,
                                                           value)
            sensorGraph.ignoredSensors(sensors)
          }
        }
      }
    }

    Pane {
      padding: 0
      Layout.fillWidth: true
      Layout.fillHeight: true

      ScrollView {
        id: scrollview
        anchors.fill: parent
        clip: true
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOn
        ScrollBar.vertical.policy: ScrollBar.AlwaysOn
        ScrollBar.horizontal.visible: ScrollBar.horizontal.size < 1
        ScrollBar.vertical.visible: ScrollBar.vertical.size < 1

        Flow {
          id: flow
          objectName: "GPU_Plug"
          width: scrollview.availableWidth
          height: scrollview.availableHeight
          spacing: Style.Controls.items_spacing

          property var childrenAdded: []
          onChildrenChanged: {
            for (var i = 0; i < children.length; ++i)
              if (childrenAdded[children[i].objectName] === undefined) {
                childrenAdded[children[i].objectName] = children[i]
                gpu.setupChild(children[i])
              }
          }

          move: Transition {
            NumberAnimation { properties: "x,y"; easing.type: Easing.InOutQuad }
          }
        }

        Component.onCompleted: {
          // BUG Scroll on control views does not work.
          // ScrollView will interfere with any children that uses MouseArea.
          // This stops that behaviour, but also disables scroll on the ScrollView.
          scrollview.contentItem.interactive = false
        }
      }
    }
  }
}
