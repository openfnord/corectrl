// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "Style.js" as Style
import "Settings.js" as Settings

ColumnLayout {

  signal settingsChanged()

  function opened() {
    if (sensorComponentsModel.count < 1)
      p.initialize()
    else
      p.refreshState()
  }

  function accepted() {
    p.updateSettings()
  }

  Component.onCompleted: p.readSettings()

  QtObject {
    id: p

    property var ignoredSensors: []

    function sensorIsIgnored(sensor) {
      return ignoredSensors.some(function(element) {
        return element === sensor
      })
    }

    function initialize() {
      sensorComponentsModel.clear()

      for (var component in Settings.ComponentData.components) {
        var element = sensorComponentsModelElement.createObject()
        element.id = component
        element.text = Settings.ComponentData.components[component]
        sensorComponentsModel.append(element)

        var sensorsModelElement = sensorsModel.createObject()
        sensorComponentsModel.sensorsModels.push(sensorsModelElement)

        var componentIdx = sensorComponentsModel.count - 1
        var sensors = Settings.ComponentData.sensors[component]
        for (var i = 0; i < sensors.length; ++i) {
          sensorsModelElement.append({"_componentIdx": componentIdx,
                                      "_index": i,
                                      "_id": sensors[i].id,
                                      "_label": sensors[i].label,
                                      "_active": !sensorIsIgnored(component + "/" + sensors[i].id)})
        }

        var sensorListViewElement = sensorListView.createObject(sensorViews)
        sensorListViewElement.parent = sensorViews
        sensorListViewElement.model = sensorsModelElement
        sensorListViewElement.visible = false
        sensorViews.views.push(sensorListViewElement)
      }

      if (sensorComponentsModel.count > 0) {
        cbSensorsComponents.currentIndex = 0
        cbSensorsComponents.updateWidth()
      }
    }

    function refreshState() {
      sensorComponentsModel.sensorsModels.forEach(function(sensorModel) {
        for (var i = 0; i < sensorModel.count; ++i) {
          var sensor = sensorModel.get(i)
          var sensorName = sensorComponentsModel.get(sensor._componentIdx).id + "/" + sensor._id
          sensorModel.set(i, {"_active": !sensorIsIgnored(sensorName)})
        }
      })
    }

    function readSettings() {
      ignoredSensors = settings.getStringList("Workarounds/ignoredSensors", [])
    }

    function updateSettings() {
      var newIgnoredSensors = []

      sensorComponentsModel.sensorsModels.forEach(function(sensorModel) {
        for (var i = 0; i < sensorModel.count; ++i) {
          var sensor = sensorModel.get(i)
          if (!sensor._active) {
            var sensorName = sensorComponentsModel.get(sensor._componentIdx).id + "/" + sensor._id
            newIgnoredSensors.push(sensorName)
          }
        }
      })

      if (!(ignoredSensors.length === newIgnoredSensors.length &&
            ignoredSensors.every(function(element, index) {
              return element === newIgnoredSensors[index];
            }))) {

        ignoredSensors = newIgnoredSensors
        settings.setStringList("Workarounds/ignoredSensors", ignoredSensors)
      }
    }
  }

  CGroupBox {
    title: qsTr("Sensors")
    showLine: false

    Layout.fillWidth: true
    Layout.fillHeight: true
    padding: 0

    ColumnLayout {
      anchors.fill: parent

      TextArea {
        text: qsTr("Disabled sensors won't be updated from hardware")
        font.pointSize: Style.g_text.size - 1
        readOnly: true
        activeFocusOnPress: false
        wrapMode: TextEdit.WordWrap
        horizontalAlignment: TextEdit.AlignHCenter
        width: 280
      }

      RowLayout {
        Label { text: qsTr("Device") }

        CComboBox {
          id: cbSensorsComponents
          model: sensorComponentsModel

          hoverEnabled: Style.g_hover

          property int lastIndex: -1
          onCurrentIndexChanged: {
            if (lastIndex !== -1)
              sensorViews.views[lastIndex].visible = false

            if (currentIndex !== -1)
              sensorViews.views[currentIndex].visible = true

            lastIndex = currentIndex
          }
        }
      }

      Rectangle {
        id: sensorViews
        property var views: []

        Layout.fillWidth: true
        height: 120
        color: Style.Dialog.bg_list_color
      }
    }
  }

  ListModel {
    id: sensorComponentsModel

    property var sensorsModels: []
  }

  Component {
    id: sensorComponentsModelElement

    ListElement {
      property string id
      property string text
    }
  }

  Component {
    id: sensorsModel

    ListModel {}
  }

  Component {
    id: sensorsModelDelegate

    RowLayout {
      CheckBox {
        checked: _active
        hoverEnabled: Style.g_hover

        leftPadding: 0
        rightPadding: 0
        topPadding: 1
        bottomPadding: 1

        onToggled: {
          if (sensorComponentsModel.sensorsModels[_componentIdx] !== undefined) {
            sensorComponentsModel.sensorsModels[_componentIdx].set(_index, {"_active": checked})
            settingsChanged()
          }
        }
      }

      Label {
        text: _label
        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
      }
    }
  }

  Component {
    id: sensorListView

    ListView {
      delegate: sensorsModelDelegate

      anchors.fill: parent
      clip: true

      ScrollBar.vertical: ScrollBar {
        policy: ScrollBar.AlwaysOn
        visible: size < 1
        hoverEnabled: Style.g_hover
      }

      ScrollBar.horizontal: ScrollBar {
        policy: ScrollBar.AlwaysOn
        visible: size < 1
        hoverEnabled: Style.g_hover
      }
    }
  }
}
