// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtCharts 2.15
import "Style.js" as Style

ChartView {
  property real minXDistance: 1.0
  property bool clampPointsYCoordinate: false

  property alias xTickCount: xAxis.tickCount
  property alias xMinorTickCount: xAxis.minorTickCount
  property alias yTickCount: yAxis.tickCount
  property alias yMinorTickCount: yAxis.minorTickCount

  id: chart

  antialiasing: true
  legend.visible: false
  backgroundColor: "#00000000"

  margins.top: 0
  margins.bottom: 0
  margins.left: 0
  margins.right: 0

  axes: [ValueAxis {
          id: xAxis
          labelFormat: Style.CurveControl.axis_label_format
          minorGridVisible: true
          minorTickCount: 1
          tickCount: 5 }
        ,
         ValueAxis {
          id: yAxis
          labelFormat: Style.CurveControl.axis_label_format
          minorGridVisible: true
          minorTickCount: 1
          tickCount: 3 }
        ]

  signal curveChanged(var name, var oldPoint, var newPoint)

  function configureAxes(xName, xUnit, xMin, xMax, yName, yUnit, yMin, yMax) {
    p.setAxisRange(xMin, xMax, yMin, yMax)
    p.setAxisTitleData(xName, yName, xUnit, yUnit)
    p.refreshAxes(enabled)

    // update outer range curve points
    for (var curveName in p.lineSeries) {
      var series = p.lineSeries[curveName]
      var oldFirst = series.at(0)
      var oldLast = series.at(series.count - 1)
      series.replace(oldFirst.x, oldFirst.y, p.lineFirstX, oldFirst.y)
      series.replace(oldLast.x, oldLast.y, p.lineLastX, oldLast.y)
    }
  }

  function addCurve(name, color, points) {
    if (p.lineSeries[name] === undefined) {
      var line = p.createLineSeries(name, color, points)
      p.lineSeries[name] = line

      // recreate control series, so control points are always on top
      var controlColors = p.controlColors()
      var controlPoints = p.controlPoints()
      for (var key in controlPoints) {
        chart.removeSeries(p.controlSeries[key])
        p.controlSeries[key] = p.createControlSeries(name,
                                               controlColors[key],
                                               controlPoints[key])
      }

      p.controlSeries[name] = p.createControlSeries(name, color, points)
    }
  }

  function removeCurve(name) {
    var line = p.lineSeries[name]
    if (line !== undefined) {
      var control = p.controlSeries[name]

      chart.removeSeries(line)
      chart.removeSeries(control)

      delete p.lineSeries[name]
      delete p.controlSeries[name]
    }
  }

  onEnabledChanged: {
    p.onEnableChanged(enabled)
  }

  QtObject { // private stuff
    id: p

    property var lineSeries: []
    property var controlSeries: []

    property string selectedCurve
    property var selectedPoint: undefined
    property int selectedPointIndex: -1

    property real prevX: 0
    property real nextX: 0

    property string xName: ""
    property string xUnit: ""
    property string yName: ""
    property string yUnit: ""

    property real xMin: 0
    property real xMax: 1
    property real yMin: 0
    property real yMax: 1

    property real lineFirstX: xMin - 1
    property real lineLastX: xMax + 1

    function createLineSeries(name, color, points) {
      var series = chart.createSeries(ChartView.SeriesTypeLine,
                                      name, xAxis, yAxis)
      series.name = name
      series.color = color
      series.opacity = chart.enabled ? Style.CurveControl.curve_opacity
                                     : Style.CurveControl.curve_opacity_alt
      series.width = 2

      // line first outer range point
      series.append(p.lineFirstX, points[0].y)

      for (var i = 0; i < points.length; ++i)
        series.append(points[i].x, points[i].y)

      // line last outer range point
      series.append(p.lineLastX, points[points.length - 1].y)

      return series
    }

    function createControlSeries(name, color, points) {
      var series = chart.createSeries(ChartView.SeriesTypeScatter,
                                      name, xAxis, yAxis)
      series.name = name
      series.color = color
      series.opacity = chart.enabled ? Style.CurveControl.curve_opacity
                                     : Style.CurveControl.curve_opacity_alt
      series.borderColor = color
      series.markerSize = 10
      series.borderWidth = 0

      for (var i = 0; i < points.length; ++i)
        series.append(points[i].x, points[i].y)

      return series
    }

    function onEnableChanged(enabled) {
      refreshAxes(enabled)

      // series opactity
      var opacity = enabled ? Style.CurveControl.curve_opacity
                            : Style.CurveControl.curve_opacity_alt
      for (var curveName in lineSeries) {
        var lseries = lineSeries[curveName]
        lseries.opacity = opacity
        var cseries = controlSeries[curveName]
        cseries.opacity = opacity
      }
    }

    function controlPoints() {
      var controlPoints = []
      for (var key in controlSeries) {
        var series = controlSeries[key]

        var points = []
        for (var i = 0; i < series.count; ++i)
          points.push(series.at(i))

        controlPoints[key] = points
      }

      return controlPoints
    }

    function controlColors() {
      var controlColors = []
      for (var key in controlSeries) {
        var series = controlSeries[key]
        controlColors[key] = series.color
      }

      return controlColors
    }

    function hasSeries() {
      for (var key in lineSeries)
        return true

      return false
    }

    function hasSelection() {
      return selectedPoint !== undefined
    }

    function mapToChart(point) {
      for (var key in controlSeries)
        return chart.mapToValue(point, controlSeries[key])
    }

    function setAxisRange(xMin, xMax, yMin, yMax) {
      xAxis.min = xMin
      xAxis.max = xMax
      yAxis.min = yMin
      yAxis.max = yMax

      this.xMin = xMin;
      this.xMax = xMax;
      this.yMin = yMin;
      this.yMax = yMax;
    }

    function setAxisTitleData(xName, yName, xUnit, yUnit) {
      this.xName = xName;
      this.yName = yName;
      this.xUnit = xUnit
      this.yUnit = yUnit
    }

    function refreshAxes(enabled) {
      xAxis.titleText = "<font color='"+ (enabled ? Style.CurveControl.axis_title_color
                                                  : Style.CurveControl.axis_title_color_alt)
                        + "'>" + xName + " (" + xUnit + ")</font>"
      xAxis.labelsColor = enabled ? Style.CurveControl.axis_label_color
                                  : Style.CurveControl.axis_label_color_alt
      xAxis.color = enabled ? Style.CurveControl.axis_color
                            : Style.CurveControl.axis_color_alt
      xAxis.gridLineColor = enabled ? Style.CurveControl.axis_grid_color
                                    : Style.CurveControl.axis_grid_color_alt
      xAxis.minorGridLineColor = enabled ? Style.CurveControl.axis_grid_minor_color
                                         : Style.CurveControl.axis_grid_minor_color_alt

      yAxis.titleText = "<font color='"+ (enabled ? Style.CurveControl.axis_title_color
                                                  : Style.CurveControl.axis_title_color_alt)
                        + "'>" + yName + " (" + yUnit + ")</font>"
      yAxis.labelsColor = enabled ? Style.CurveControl.axis_label_color
                                  : Style.CurveControl.axis_label_color_alt
      yAxis.color = enabled ? Style.CurveControl.axis_color
                            : Style.CurveControl.axis_color_alt
      yAxis.gridLineColor = enabled ? Style.CurveControl.axis_grid_color
                                    : Style.CurveControl.axis_grid_color_alt
      yAxis.minorGridLineColor = enabled ? Style.CurveControl.axis_grid_minor_color
                                         : Style.CurveControl.axis_grid_minor_color_alt
    }

    function updateGraphPoint(oldPoint, newPoint) {
      lineSeries[selectedCurve].replace(oldPoint.x, oldPoint.y, newPoint.x, newPoint.y)
      controlSeries[selectedCurve].replace(oldPoint.x, oldPoint.y, newPoint.x, newPoint.y)
    }

    function updateOuterRangePoints(oldY, newY) {
      if (selectedPointIndex == 0)
        lineSeries[selectedCurve].replace(lineFirstX, oldY, lineFirstX, newY)
      if (selectedPointIndex == controlSeries[selectedCurve].count - 1)
        lineSeries[selectedCurve].replace(lineLastX, oldY, lineLastX, newY)
    }

    function moveSelection(point) {
      // clamp point coordinates to axes range
      point.x = Math.min(Math.max(point.x, xMin), xMax)
      point.y = Math.min(Math.max(point.y, yMin), yMax)

      // limit x movement between points using minXDistance
      if (selectedPointIndex > 0)
        point.x = Math.max(point.x, prevX + minXDistance)
      if (selectedPointIndex < controlSeries[selectedCurve].count - 1)
        point.x = Math.min(point.x, nextX - minXDistance)

      if (clampPointsYCoordinate)
        clampOtherPointsYCoordinate()

      updateGraphPoint(selectedPoint, point)
      updateOuterRangePoints(selectedPoint.y, point.y)

      // emit curveChanged signal
      curveChanged(selectedCurve, selectedPoint, point)

      selectedPoint = point

      return point
    }

    function clampOtherPointsYCoordinate() {
      var series = controlSeries[selectedCurve]
      for (var i = 0; i < series.count; ++i) {
        // skip selected point
        if (selectedPointIndex == i)
          continue

        var point = series.at(i)
        if ((i < selectedPointIndex && point.y > selectedPoint.y) ||
            (i > selectedPointIndex && point.y < selectedPoint.y)) {
          point.y = selectedPoint.y
          var oldPoint = series.at(i)
          updateGraphPoint(oldPoint, point)
          curveChanged(selectedCurve, oldPoint, point)
        }
      }
    }

    function findCloserIndex(series, target) {
      var distRange = (xAxis.max - xAxis.min) / 20
      var closerPointIndex = -1
      var minDist = 10000
      for (var i = 0; i < series.count; ++i) {
        var point = series.at(i)
        var distance = Math.sqrt(Math.pow(point.x - target.x, 2) +
                                 Math.pow(point.y - target.y, 2))

        if (distance < minDist && distance < distRange) {
          minDist = distance
          closerPointIndex = i
        }
      }

      return closerPointIndex
    }

    function selectCloser(point) {

      // pick the first closer point in controls
      for (var key in  controlSeries) {
        var control = controlSeries[key]
        selectedPointIndex = findCloserIndex(control, point)
        if (selectedPointIndex != -1)
          break
      }

      if (selectedPointIndex != -1) {
        selectedPoint = control.at(selectedPointIndex)

        // find point's curve
        var curveFound = false
        for (var curveName in lineSeries) {
          var series = lineSeries[curveName]
          for (var i = 0; i < series.count; ++i) {
            if (series.at(i) === selectedPoint) {
              curveFound = true
              break
            }
          }

          if (curveFound) {
            selectedCurve = curveName
            break
          }
        }

        if (selectedPointIndex > 0)
          prevX = control.at(selectedPointIndex - 1).x

        if (selectedPointIndex < control.count - 1)
          nextX = control.at(selectedPointIndex + 1).x
      }
      else {
        deselect()
      }

      return selectedPoint
    }

    function deselect() {
      selectedPoint = undefined
      selectedPointIndex = -1
      selectedCurve = ""
    }
  }

  ToolTip {
    id: tooltip
    delay: 0
    timeout: -1
    margins: -1

    function updatePosition(x, y) {
      x = Math.max(0, Math.min(x, Math.round(parent.width - width)))
      y = Math.max(0, Math.min(y + 25, Math.round(parent.height - height)))

      tooltip.x = x
      tooltip.y = y
    }
  }

  MouseArea {
    id: mouseArea
    anchors.fill: parent

    onPressed: {      
      if (p.hasSeries()) {
        var outerPoint = p.mapToChart(Qt.point(mouse.x, mouse.y))
        var point = p.selectCloser(outerPoint)

        // update tooltip
        if (point !== undefined) {
          tooltip.updatePosition(mouse.x, mouse.y)
          tooltip.text = Math.round(point.x) + p.xUnit + " " + Math.round(point.y) + p.yUnit
          tooltip.visible = true
        }
      }
    }

    onReleased: {
      if (p.hasSeries()) {
        p.deselect()
        tooltip.visible = false
      }
    }

    onPositionChanged: {
      if (p.hasSeries() && p.hasSelection()) {
        var point = p.mapToChart(Qt.point(mouse.x, mouse.y))
        point = p.moveSelection(point)

        // update tooltip
        tooltip.updatePosition(mouse.x, mouse.y)
        tooltip.text = Math.round(point.x) + p.xUnit + " " + Math.round(point.y) + p.yUnit
      }
    }
  }
}

