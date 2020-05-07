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
.pragma library;

var g_padding = 12;
var g_hover = true;
var g_tweakScale = 0.78;

var g_text = {
  size: 9,
  icon_size: 12,
  icon_size_tabbar: 16,
  family: "Sans"
};

var g_icon = {
  size: 32,
  source_size: 64,

  // unicode button icons
  MENU:    "\u22EE", // ⋮
  MORE:    "\u22EF", // ⋯
  BACK:    "\u2190", // ←
  ADD:     "+",
  SPLIT:   "|",
};

var ToolBar = {
  bg_color: "#2A2A2A",
  text_color_msg: "#DADADA",
  text_color: "#FFFFFF",
  text_color_alt: "#909090",

  Material: {
    elevation: 1
  }
}

var RectItem = {
  bg_color: "#484848",
  bg_color_alt: "#404040",
  bg_color_hover: "#505050",

  text_color: "#FAFAFA",
  text_color_alt: "#909090",

  padding: 10
}

var RectItemList = {
  items_spacing: 2
};

var TabBar = {
  main_footer: {
    bg_color: "#555555",
  }
};

var TabButton = {
  bg_color: "#FF383838",
  bg_color_alt: "#FF484848"
};

var TextField = {
  padding: 8,
};


var Material = {
  elevation: 1,
  accent: "orangered",
  accent_alt: "#909090"
};

var ModeSelector = {
  header: {
    bg_color: "#FF404040",
    padding: 8,
  },
  body: {
    bg_color: "#FF393939"
  }
};

var Controls = {
  items_spacing: 4
};

var Graph = {
  bg_color: "#20FFFFFF",
  ctl_bg_color: "#AA202020"
};

var Profiles = {
  opacity_anim_duration: 200
};

var ToolTip = {
  delay: 1000,
  timeout: 3500
};

var CurveControl = {
  curve_amd_start_color: "lightskyblue",
  curve_color: "#ABFFFFFF",

  curve_opacity: 1.0,
  curve_opacity_alt: 0.3,

  axis_title_color: "#F0FFFFFF",
  axis_title_color_alt: "#80FFFFFF",

  axis_label_format: "%i",

  axis_label_color: "#C7FFFFFF",
  axis_label_color_alt: "#77FFFFFF",

  axis_color: "#E0FFFFFF",
  axis_color_alt: "#20FFFFFF",

  axis_grid_color: "#80FFFFFF",
  axis_grid_color_alt: "#20FFFFFF",

  axis_grid_minor_color: "#30FFFFFF",
  axis_grid_minor_color_alt: "#09FFFFFF",
};

var FVControl = {
  border_color: "#40666666",
};

var Dialog = {
  bg_list_color: "#FF343434",
  tabs: {
    bg_color: "#FF525252",
    bg_color_alt: "#FF626262",
  },
};

var GroupBox = {
  text_size: 11,
  text_bold: true,
  bg_color: "transparent",
  bg_border_color: "#FF747474",
  bg_border_color_alt: "#40747474",
  bg_radius: 2
};
