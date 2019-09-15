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
.pragma library;

var SysemTrayDefaults = {
  enabled: true,
  startMinimized: false,
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
