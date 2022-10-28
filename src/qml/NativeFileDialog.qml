// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

import QtQuick 2.15
import Qt.labs.platform 1.1

// Native file dialog
// Requires:
//  · link against Qt5::Widgets
//  · use QApplication instead QGuiApplication

FileDialog {
  property bool saveDlg: false

  fileMode: saveDlg ? FileDialog.SaveFile : FileDialog.OpenFile
  folder: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
  options: FileDialog.ReadOnly

  /// Returns the selected file name.
  function fileName() {
    var path = file.toString()
    return path.slice(path.lastIndexOf("/") + 1)
  }
}
