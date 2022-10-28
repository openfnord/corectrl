// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import "Style.js" as Style

TextField {
  property int value: 0
  property alias minValue: range.bottom
  property alias maxValue: range.top

  text: value

  hoverEnabled: Style.g_hover
  selectByMouse: true

  padding: Style.TextField.padding
  horizontalAlignment: TextInput.AlignHCenter

  validator: IntValidator {
    id: range
    bottom: 0
    top: 0
  }

  onEditingFinished: {
    var newValue = parseInt(text)
    if (newValue !== value)
      p.updateValue(newValue)
  }

  QtObject {
    id: p

    property bool restoreTextRequest: false
    property string newText: ""

    property bool updateValueRequest: false
    property int newValue: 0

    function restoreText(text) {
      newText = text
      restoreTextRequest = true
      restoreTextRequest = false
    }

    function updateValue(value) {
      newValue = value
      updateValueRequest = true
      updateValueRequest = false
    }

    function validateInput() {
      if (!(text && text.length > 0)) {
        restoreText(value)
      }
      else {
        var newValue = parseInt(text)
        if (newValue < range.bottom) {
          if (value !== range.bottom)
            updateValue(range.bottom)
          else
            restoreText(value)
        }
        else if (newValue > range.top) {
          if (value !== range.top)
            updateValue(range.top)
          else
            restoreText(value)
        }
      }
    }
  }

  Binding on value {
    when: p.updateValueRequest
    value: p.newValue
  }

  Binding on text {
    when: p.restoreTextRequest
    value: p.newText
  }

  Keys.onPressed: {
    if (event.key === Qt.Key_Tab ||
        event.key === Qt.Key_Enter ||
        event.key === Qt.Key_Return)
      p.validateInput()
    else if (event.key === Qt.Key_Escape)
      p.restoreText(value)
  }

  onFocusChanged: {
    if (!activeFocus)
      p.validateInput()
  }
}
