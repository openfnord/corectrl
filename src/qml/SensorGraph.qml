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
import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3
import QtCharts 2.2
import "Style.js" as Style

Rectangle {
  id: graph
  objectName: "SensorGraph"

  implicitWidth: parent.width
  implicitHeight: 160

  color: Style.Graph.bg_color

  function addItem(item) {
    p.addGrapItem(item)
  }

  function ignoredSensors(sensors) {
    p.refreshIgnored(sensors)
  }

  QtObject { // private stuff
    id: p

    property var itemsArray: []

    function refreshIgnored(sensors) {
      for (var i = 0; i < itemsArray.length; ++i) {
        itemsArray[i].ignored = sensors.some(function(item) {
          return itemsArray[i].name === item
        })
      }
    }

    function updateItems() {
      for (var i = 0; i < itemsArray.length; ++i)
        itemsArray[i].update()
    }

    function activateGraphItem(index, active) {
      itemsArray[index].active = active
      controlsModel.set(index, { "_active": active })
    }

    function addGrapItem(item) {
      itemsArray.push(item)
      configureItem(item)
      addItemControl(item)
    }

    function configureItem(item) {
      var xAxis = axis.createObject(chart)
      var yAxis = axis.createObject(chart)
      var series = chart.createSeries(ChartView.SeriesTypeLine,
                                      qsTranslate("SensorGraph", item.name),
                                      xAxis, yAxis)
      item.configure(series, xAxis, yAxis)
    }

    function addItemControl(item) {
      var itemIndex = controlsModel.count

      item.valueChanged.connect(function (value) {
        controlsModel.set(itemIndex, { "_value": value })
      })

      item.activeChanged.connect(function (active) {
        controlsModel.set(itemIndex, { "_active": active })
      })

      item.colorChanged.connect(function (color) {
        controlsModel.set(itemIndex, { "_color": color })
      })

      item.ignoredChanged.connect(function (ignored) {
        controlsModel.set(itemIndex, { "_ignored": ignored})
      })

      item.yAxisRangeChanged.connect(function (min, max) {
        for (var i = 0; i < itemsArray.length; ++i)
          if (itemsArray[i] !== item && itemsArray[i].unit === item.unit)
            itemsArray[i].updateYAxisRange(min, max)
      })

      controlsModel.append({ "_index": itemIndex,
                             "_label": qsTranslate("SensorGraph", item.name),
                             "_name": item.name,
                             "_value": item.value,
                             "_unit": item.unit,
                             "_color": item.color,
                             "_active": true,
                             "_ignored": false })
    }
  }

  Component {
    id: axis
    ValueAxis { visible: false }
  }

  Component {
    id: controlsModelDelegate

    RowLayout {
      property var index: _index
      spacing: 0
      enabled: !_ignored

      CheckBox {
        checked: _active

        leftPadding: -5
        rightPadding: 0
        topPadding: -2
        bottomPadding: -2

        scale: 0.75
        Material.accent: _color

        onToggled: p.activateGraphItem(index, checked)
      }

      Label {
        text: _label + ": "
        font.pointSize: Style.g_text.size - 1
        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
      }

      Label {
        text: _ignored ? qsTr("n/a") : _value
        font.pointSize: Style.g_text.size - 1
        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
      }

      Label {
        text: " " + _unit
        visible: !_ignored
        font.pointSize: Style.g_text.size - 1
        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
      }
    }
  }

  ListModel {
    id: controlsModel
  }

  Row {
    spacing: 0
    anchors.fill: parent

    Rectangle {
      id: controls

      color: Style.Graph.ctl_bg_color
      width: 180
      height: parent.height

      ScrollView {
        id: ctlScrollView
        anchors.fill: parent

        leftPadding: 8
        rightPadding: 8
        topPadding: 10
        bottomPadding: 10

        clip: true
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOn
        ScrollBar.vertical.policy: ScrollBar.AlwaysOn
        ScrollBar.horizontal.visible: ScrollBar.horizontal.size < 1
        ScrollBar.vertical.visible: ScrollBar.vertical.size < 1

        /// WORKAROUND QTBUG-74000
        contentWidth: columnLayout.implicitWidth
        contentHeight: columnLayout.implicitHeight

        ColumnLayout {
          id: columnLayout

          Repeater {
            model: controlsModel
            delegate: controlsModelDelegate
          }
        }
      }
    }

    ChartView {
      id: chart

      width: parent.width - controls.width
      height: parent.height

      antialiasing: true
      legend.visible: false
      backgroundColor: "#00000000"

      margins.left: 0
      margins.right: 0

      Timer {
        interval: 1000
        running: true
        repeat: true
        onTriggered: p.updateItems()
      }
    }
  }
}
