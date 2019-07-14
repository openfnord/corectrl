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
import QtQuick.Controls 2.2
import Radman.UIComponents 1.0

AMD_PM_OVERCLOCK {
  id: pmOclk
  objectName: "AMD_PM_OVERCLOCK"

  width: controls.width
  height: controls.height

  Pane {
    id: controls
    padding: 0

    Column {
      objectName: "AMD_PM_OVERCLOCK_Plug"

      property var childrenAdded: []
      onChildrenChanged: {
        for (var i = 0; i < children.length; ++i)
          if (childrenAdded[children[i].objectName] === undefined) {
            childrenAdded[children[i].objectName] = children[i]
            pmOclk.setupChild(children[i])
          }
      }
    }
  }
}
