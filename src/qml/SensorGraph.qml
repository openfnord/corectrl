// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import QtCharts 2.15
import "Style.js" as Style

Rectangle {
  id: graph
  objectName: "SensorGraph"

  implicitWidth: parent.width
  implicitHeight: 210

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

  Connections {
    target: settings

    function onSettingChanged(key, value) {
      if (key === "UI/splitview-sensorgraph-controls-width") {
        controls.SplitView.preferredWidth = value
      }
    }
  }

  SplitView {
    spacing: 0
    anchors.fill: parent
    orientation: Qt.Horizontal

    onResizingChanged: {
      if (!resizing) {
        settings.setValue("UI/splitview-sensorgraph-controls-width",
                          controls.SplitView.preferredWidth)
      }
    }

    Rectangle {
      id: controls
      implicitWidth: 220
      color: Style.Graph.ctl_bg_color

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
      SplitView.fillWidth: true

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
