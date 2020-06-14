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
import QtQuick.Layouts 1.3
import "Style.js" as Style

Dialog {
  id: dlg

  property string name
  property string exe
  property url icon

  property var newInfoAction: function(name, exe, icon) {}

  signal updateProfileNameUsed(string profileName)
  property bool profileNameUsed: false

  signal updateExecutableNameUsed(string executableName)
  property bool executableNameUsed: false

  focus: true
  modal: true
  standardButtons: Dialog.Ok | Dialog.Cancel
  closePolicy: Popup.CloseOnEscape

  // center in parent
  x: (parent.width - width) / 2
  y: (parent.height - height) / 2

  QtObject { // private stuff
    id: p

    property bool nameOK: false
    property bool exeOK: false
    property bool iconOK: false

    function updateOKButtonState() {
      dlg.footer.standardButton(Dialog.Ok).enabled = nameOK && exeOK && iconOK
    }

    function resetState() {
      newInfoAction = function(name, exe, icon) {}
    }
  }

  onRejected: p.resetState()
  onAccepted: {
    newInfoAction(dlg.name, dlg.exe, dlg.icon)
    p.resetState()
  }

  onNameChanged: {
    if (name !== nameTf.text)
      nameTf.text = name

    p.nameOK = name && name.length > 0 && !/^\s+$/.test(name)

    if (p.nameOK) {
      updateProfileNameUsed(name)
      p.nameOK &= !profileNameUsed
    }

    p.updateOKButtonState()
  }

  onExeChanged: {
    if (exe !== exeTf.text)
      exeTf.text = exe

    p.exeOK = exe && exe.length > 0 && !/\\|\/|\||\x00|\*|`|;|:|'|"/.test(exe)

    if(p.exeOK) {
      updateExecutableNameUsed(exe)
      p.exeOK &= !executableNameUsed
    }

    p.updateOKButtonState()
  }

  onIconChanged: {
    if (icon !== icnBtn.path)
      icnBtn.path = icon

    p.iconOK = icon !== null && icon.toString().length > 0
    p.updateOKButtonState()
  }

  GridLayout {
    anchors.fill: parent
    columns: 2
    columnSpacing: 10

    Label {
      text: qsTr("Name:")
      Layout.alignment: Qt.AlignRight
    }

    TextField {
      id: nameTf
      Layout.alignment: Qt.AlignLeft
      selectByMouse: true
      hoverEnabled: Style.g_hover
      placeholderText: qsTr("Profile name")

      onTextEdited: dlg.name = text
    }

    Label {
      text: qsTr("Executable:")
      Layout.alignment: Qt.AlignRight
    }
    Row {
      spacing: 5

      TextField {
        id: exeTf
        selectByMouse: true
        hoverEnabled: Style.g_hover
        placeholderText: qsTr("Executable name")

        onTextEdited: dlg.exe = text
      }

      Button {
        text: Style.g_icon.MORE
        font.pointSize: Style.g_text.icon_size
        implicitWidth: 25
        implicitHeight: 35
        spacing: 0
        padding: 0
        hoverEnabled: Style.g_hover
        Material.elevation: Style.Material.elevation

        NativeFileDialog {
          id: exeFDialog
          title: qsTr("Select an executable file")

          onAccepted: {
            var fileName = exeFDialog.fileName()
            exeTf.text = fileName
            dlg.exe = fileName
          }
        }
        onClicked: exeFDialog.open()
      }
    }

    Label {
      text: qsTr("Icon:")
      Layout.alignment: Qt.AlignRight
    }

    Button {
      id: icnBtn
      property alias path: icnBtnImage.source

      Layout.alignment: Qt.AlignLeft
      hoverEnabled: Style.g_hover
      Material.elevation: Style.Material.elevation

      contentItem: Item {
        implicitWidth: icnBtnImage.width
        implicitHeight: icnBtnImage.height

        Image {
          id: icnBtnImage
          anchors.centerIn: parent
          fillMode: Image.Stretch
          width: Style.g_icon.size
          height: Style.g_icon.size
          sourceSize.width: Style.g_icon.source_size
        }
      }

      NativeFileDialog {
        id: icnFDialog
        title: qsTr("Select an icon")
        nameFilters: [qsTr("Images") + "(*.png *.bmp *.jpg *.jpeg *.ico *.icns *.svg)"]

        onAccepted: {
          icnBtnImage.source = file
          dlg.icon = file
        }
      }
      onClicked: icnFDialog.open()
    }
  }

  Component.onCompleted: {
    p.resetState()
    p.updateOKButtonState()

    dlg.footer.standardButton(Dialog.Ok).hoverEnabled = Style.g_hover
    dlg.footer.standardButton(Dialog.Cancel).hoverEnabled = Style.g_hover
  }
}
