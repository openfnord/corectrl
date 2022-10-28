// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

.pragma library;

var SysemTrayDefaults = {
  enabled: true,
  startMinimized: false,
  saveWindowGeometry: true,
};

var ComponentData = {
  components: [], // component labels indexed by component id
  sensors: []     // sensors data indexed by component id
};

function addComponentData(componentId, componentLabel, sensorId, sensorLabel) {
  if (ComponentData.components[componentId] === undefined)
    ComponentData.components[componentId] = componentLabel

  if (ComponentData.sensors[componentId] === undefined)
    ComponentData.sensors[componentId] = []

  var sensor = {
    id: sensorId,
    label: sensorLabel
  };

  ComponentData.sensors[componentId].push(sensor)
}

function componentIgnoredSensors(componentId, ignoredSensorsList) {
  var sensorList = []

  // Handle all input as a list.
  // NOTE This is needed because QSettings will return a QString
  // instead of a QStringList when there is only one ignored sensor.
  if (typeof(ignoredSensorsList) === "string") {
    var sensor = ignoredSensorsList
    ignoredSensorsList = []
    ignoredSensorsList.push(sensor)
  }

  for (var i = 0; i < ignoredSensorsList.length; ++i) {
    var componentList = ignoredSensorsList[i].split('/')
    if (componentId === componentList[0])
      sensorList.push(componentList[1])
  }

  return sensorList
}
