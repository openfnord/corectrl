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
import QtQuick 2.15
import QtQuick.Controls 2.15
import CoreCtrl.UIComponents 1.0

AMD_PM_ADVANCED {
  id: pmAdv
  objectName: "AMD_PM_ADVANCED"

  width: controls.width
  height: controls.height

  Pane {
    id: controls
    padding: 0

    Flow {
      objectName: "AMD_PM_ADVANCED_Plug"

      width: {
        var maxWidth = 0
        for (var i = 0; i < children.length; ++i)
          maxWidth = Math.max(maxWidth, children[i].width)
        return maxWidth
      }

      property var childrenAdded: []
      onChildrenChanged: {
        for (var i = 0; i < children.length; ++i) {
          if (childrenAdded[children[i]] === undefined) {
            childrenAdded[children[i]] = children[i]
            pmAdv.setupChild(children[i])
          }
        }
      }
    }
  }
}
