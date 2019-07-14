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
import QtQuick.Controls.Material 2.2
import "Style.js" as Style

ComboBox {
  id: comboBox
  textRole: 'text'
  implicitWidth: modelWidth + rightPadding + leftPadding
  Material.elevation: Style.Material.elevation
  wheelEnabled: true

  property real modelWidth

  function updateWidth() {
    modelWidth = 0;

    for (var i = 0; i < model.count; i++) {
      metrics.text = model.get(i).text
      modelWidth = Math.max(metrics.width, modelWidth)
    }

    metrics.text = "##" // add extra space for the ComboBox decoration
    modelWidth += metrics.width
  }

  TextMetrics {
    id: metrics
  }
}
