// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import "Style.js" as Style

Page {
  id: modeSelector

  property alias headerTitle: label.text
  property var headerBackground: Style.ModeSelector.header.bg_color
  property alias contentParentObject: contentsAnchor.objectName
  property var contentBackground: Style.ModeSelector.body.bg_color

  /// Fired when the selected mode changed.
  /// @param mode key of the new selected mode
  signal selectionChanged(string mode)

  /// Fired when a new child is added
  /// @param added child
  signal childAdded(var child)

  // Sets the modes of the selector.
  // @param modes array of elements where each pair of elements
  // corresponds to [mode_key, mode_text] for each [i, i+1]
  function setModes(modes) {
    listModel.clear()

    for (var i = 0; i < modes.length; i+=2) {
      var element = listElement.createObject()
      element.mode = modes[i]
      element.text = modes[i + 1]

      listModel.append(element)
    }

    cbMode.updateWidth()
  }

  /// Selects a mode.
  /// @param mode key of the mode to be selected
  function select(mode) {
    if (cbMode.currentMode !== mode)
      cbMode.setMode(mode)
  }

  ListModel {
    id: listModel
  }

  Component {
    id: listElement

    ListElement {
      property string text
      property string mode
    }
  }

  header: Pane {
    padding: Style.ModeSelector.header.padding
    Material.background: headerBackground

    RowLayout {
      Label {
        id: label
        rightPadding: 20
      }

      CComboBox {
        id: cbMode
        model: listModel

        property string currentMode: ""

        function setMode(mode) {
          for (var i = 0; i < listModel.count; i++) {
            if (listModel.get(i).mode === mode) {
              var lastMode = currentMode
              currentMode = mode
              currentIndex = i
              contentsAnchor.toggleActive(lastMode, currentMode)
              break;
            }
          }
        }

        onActivated: {
          if (currentMode !== model.get(currentIndex).mode) {
            var lastMode = currentMode
            currentMode = model.get(currentIndex).mode
            contentsAnchor.toggleActive(lastMode, currentMode)
          }
        }
      }

      Rectangle {
        Layout.fillWidth: true
      }
    }
  }

  Pane {
    padding: 0
    anchors.fill: parent
    Material.background: contentBackground

    Row {
      id: contentsAnchor
      anchors.centerIn: parent

      function toggleActive(from, to) {
        if (childrenAdded[from] !== undefined &&
            childrenAdded[to] !== undefined) {
          childrenAdded[from].activate(false)
        }

        if (childrenAdded[to] !== undefined) {
          childrenAdded[to].activate(true)
          modeSelector.selectionChanged(to)
        }
      }

      property var childrenAdded: []
      onChildrenChanged: {
        for (var i = 0; i < children.length; ++i) {
          if (childrenAdded[children[i].objectName] === undefined) {
            childrenAdded[children[i].objectName] = children[i]
            modeSelector.childAdded(children[i])
          }
        }
      }
    }
  }
}
