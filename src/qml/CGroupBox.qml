// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import "Style.js" as Style

GroupBox {
  id: control

  property alias showLine: bg.visible

  label: Label {
    text: control.title
    font.pointSize: Style.GroupBox.text_size
    font.bold: Style.GroupBox.text_bold
    width: control.width
    horizontalAlignment: Text.AlignHCenter
    y: control.padding / 2
  }

  background: Rectangle {
    id: bg

    color: Style.GroupBox.bg_color
    border.color: enabled ? Style.GroupBox.bg_border_color
                          : Style.GroupBox.bg_border_color_alt
    radius: Style.GroupBox.bg_radius
  }
}
