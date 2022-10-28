// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import "Style.js" as Style

Dialog {
  id: dlg

  property string name
  property string exe
  property url icon
  property url defaultIcon
  property bool hasCustomIcon
  property bool forceAutomaticActivation: false

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

    property url originalIcon
    property url selectedIcon

    property bool nameOK: false
    property bool exeOK: false
    property bool iconOK: false

    function validProfileName(name) {
      return name && name.trim().length > 0
    }

    function validFileName(name) {
      return name && name.trim().length > 0 &&
             !/\\|\/|\||\x00|\*|`|;|:|'|"/.test(name)
    }

    function validateName() {
      nameOK = activationCb.currentIndex === 0 ? validProfileName(name) :
                                                 validFileName(name)
      return nameOK
    }

    function validateExe() {
      exeOK = activationCb.currentIndex === 0 ? validFileName(exe) :
                                                true
      return exeOK
    }

    function updateOKButtonState() {
      dlg.footer.standardButton(Dialog.Ok).enabled = nameOK && exeOK && iconOK
    }

    function resetState() {
      newInfoAction = function(name, exe, icon) {}
      originalIcon = ""
      selectedIcon = ""
      forceAutomaticActivation = false
      activationCb.currentIndex = -1
    }
  }

  onAboutToShow: {
    activationCb.currentIndex = forceAutomaticActivation ? 0 :
                                                           exe.length > 0 ? 0 : 1
    customizeIconCb.checked = hasCustomIcon
    p.originalIcon = icon
  }
  onClosed: p.resetState()
  onAccepted: newInfoAction(dlg.name, activationCb.currentIndex === 0 ? dlg.exe : "", dlg.icon)

  onNameChanged: {
    if (name !== nameTf.text)
      nameTf.text = name

    if (p.validateName()) {
      updateProfileNameUsed(name)
      p.nameOK &= !profileNameUsed
    }

    p.updateOKButtonState()
  }

  onExeChanged: {
    if (exe !== exeTf.text)
      exeTf.text = exe

    if (p.validateExe()) {
      updateExecutableNameUsed(exe)
      p.exeOK &= !executableNameUsed
    }

    p.updateOKButtonState()
  }

  onIconChanged: {
    if (icon !== icnBtn.path)
      icnBtn.path = icon

    p.iconOK = icon.toString().length > 0
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
      text: qsTr("Activation:")
      Layout.alignment: Qt.AlignRight
    }

    CComboBox {
      id: activationCb
      Layout.alignment: Qt.AlignLeft
      Layout.fillWidth: true
      hoverEnabled: Style.g_hover

      model: ListModel {
        ListElement { text: qsTr("Automatic") }
        ListElement { text: qsTr("Manual") }
      }

      onCurrentIndexChanged: {
        p.validateName()
        p.validateExe()
        p.updateOKButtonState()
      }
    }

    Label {
      text: qsTr("Executable:")
      Layout.alignment: Qt.AlignRight
      visible: activationCb.currentIndex === 0
    }
    Row {
      visible: activationCb.currentIndex === 0
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
      Layout.alignment: Qt.AlignLeft

      enabled: customizeIconCb.checked
      property alias path: icnBtnImage.source

      hoverEnabled: Style.g_hover
      Material.elevation: Style.Material.elevation

      contentItem: Item {
        implicitWidth: icnBtnImage.width
        implicitHeight: icnBtnImage.height

        Image {
          id: icnBtnImage
          anchors.centerIn: parent
          fillMode: Image.PreserveAspectFit
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
          p.selectedIcon = file
        }
      }
      onClicked: icnFDialog.open()
    }

    CheckBox {
      id: customizeIconCb
      Layout.alignment: Qt.AlignLeft
      Layout.row: activationCb.currentIndex === 0 ? 4 : 3
      Layout.column: 1

      text: qsTr("Customize icon")

      hoverEnabled: Style.g_hover

      leftPadding: 0
      rightPadding: 0
      topPadding: 0
      bottomPadding: 0

      onToggled: {
        var customIcon = dlg.hasCustomIcon ? p.originalIcon : ""
        var customIconToShow = p.selectedIcon.toString().length > 0 ? p.selectedIcon : customIcon
        dlg.icon = checked ? customIconToShow : dlg.defaultIcon
      }
    }
  }

  Component.onCompleted: {
    p.resetState()
    p.updateOKButtonState()

    dlg.footer.standardButton(Dialog.Ok).hoverEnabled = Style.g_hover
    dlg.footer.standardButton(Dialog.Cancel).hoverEnabled = Style.g_hover
  }
}
