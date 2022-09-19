//
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>
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
import QtQuick 2.15
import QtQuick.Controls 2.15
import CoreCtrl.UIComponents 1.0
import "Style.js" as Style

AMD_PM_OVERDRIVE {
  id: pmOverdrive
  objectName: "AMD_PM_OVERDRIVE"

  width: controls.width
  height: controls.height

  Pane {
    id: controls
    padding: Style.g_padding

    Row {
      objectName: "AMD_PM_OVERDRIVE_Plug"
      spacing: 5

      property var childrenAdded: []
      onChildrenChanged: {
        for (var i = 0; i < children.length; ++i) {
          if (childrenAdded[children[i]] === undefined) {
            childrenAdded[children[i]] = children[i]
            pmOverdrive.setupChild(children[i])
          }
        }
      }
    }
  }
}
