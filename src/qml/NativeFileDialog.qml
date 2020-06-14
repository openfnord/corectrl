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
import QtQuick 2.9
import Qt.labs.platform 1.0

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
