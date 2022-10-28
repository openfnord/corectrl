// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import CoreCtrl.UIComponents 1.0
import "Style.js" as Style

Page {
  hoverEnabled: Style.g_hover

  PROFILE_MANAGER {
    id: profileManager
    objectName: "PROFILE_MANAGER"

    onInitProfiles: p.initProfileButtons(profiles)
    onProfileAdded: p.createProfileButton(name, exe, icon, hasCustomIcon, isActive)
    onProfileRemoved: p.removeProfileButton(name)
    onProfileSaved:  p.updateToolBarButtonsOnProfileSaved()
    onProfileInfoChanged: p.updateProfileButton(oldName, newName, exe, icon, hasCustomIcon, isActive)
    onProfileChanged: p.updateProfileUIState(name)
    onProfileActiveChanged: p.updateProfileButtonActiveState(name, active);
    onManualProfileToggled: p.updateProfileButtonManualToggle(name, active);
  }

  QtObject { // private stuff
    id: p

    property bool restoringProfile: false
    property bool unappliedSettings: false
    property bool unsavedSettings: false
    property var editedProfileBtn: undefined

    function initProfileButtons(profiles) {
      for (var i = 0; i < profiles.length; i+=5) {

        var name = profiles[i]
        var exe = profiles[i+1]
        var icon = profiles[i+2]
        var isActive = profiles[i+3]
        var hasCustomIcon = profiles[i+4]
        var isGlobal = i === 0

        profileModel.append({ "_label": isGlobal ? qsTranslate("ProfileManagerUI", name) : name,
                              "_name": name,
                              "_exe": exe,
                              "_icon": icon,
                              "_isGlobal": isGlobal,
                              "_hasCustomIcon": hasCustomIcon,
                              "_active": isActive,
                              "_toggledManual": false })
      }
    }

    function createProfileButton(name, exe, icon, hasCustomIcon, isActive) {
      profileModel.append({ "_label": name,
                            "_name": name,
                            "_exe": exe,
                            "_icon": icon,
                            "_isGlobal": false,
                            "_hasCustomIcon": hasCustomIcon,
                            "_active": isActive,
                            "_toggledManual": false })

      // move model element to the correct position
      for (var i = 1; i < profileModel.count; ++i) {
        if (name < profileModel.get(i)._name) {
          profileModel.move(profileModel.count - 1, i, 1)
          break
        }
      }
    }

    function removeProfileButton(name) {
      for (var i = 0; i < profileModel.count; ++i) {
        if (profileModel.get(i)._name === name) {
          profileModel.remove(i)
          break
        }
      }
    }

    function updateProfileButton(oldName, newName, exe, icon, hasCustomIcon, isActive) {
      // get model element old and new positions
      var from = -1
      var to = profileModel.count
      for (var i = 1; i < profileModel.count; ++i) {
        var itemName = profileModel.get(i)._name
        if (from < 0 && oldName === itemName) {
          from = i
          if (to < profileModel.count)
            break
        }
        if (to === profileModel.count && newName < itemName) {
          to = i
          if (from > 0)
            break
        }
      }

      if (from > 0) { // model element found
        to = from < to ? to - 1 : to
        profileModel.move(from, to, 1)

        // update model element
        var toggledManual = profileModel.get(to)._toggledManual
        profileModel.set(to, {"_label": newName,
                              "_name": newName,
                              "_exe": exe,
                              "_icon": icon,
                              "_hasCustomIcon": hasCustomIcon,
                              "_active": isActive,
                              "_toggledManual": exe.length > 0 ? false : toggledManual })

        refreshProfileButtonsIcon(icon)
      }
    }

    function updateProfileButtonActiveState(name, active) {
      for (var i = 1; i < profileModel.count; ++i) {
        if (profileModel.get(i)._name === name &&
            profileModel.get(i)._active !== active) {
          profileModel.set(i, {"_active": active})
          break
        }
      }
    }

    function updateProfileButtonManualToggle(name, active) {
      for (var i = 1; i < profileModel.count; ++i) {
        if (profileModel.get(i)._name === name) {
          profileModel.set(i, {"_toggledManual": active})
          break
        }
      }
    }

    function refreshProfileButtonsIcon(icon) {
      for (var i = 1; i < profileModel.count; ++i) {
        if (profileModel.get(i)._icon === icon) {
          profileModel.set(i, {"_icon": ""}) // force icon update
          profileModel.set(i, {"_icon": icon})
        }
      }
    }

    function updateToolBarProfileInfo(name, icon, isActive) {
      tbLabel.text = isActive ? name : name + " (" + qsTr("Disabled") + ")"
      tbLabel.color = isActive ? Style.ToolBar.text_color : Style.ToolBar.text_color_alt
      tbIcon.source = icon
    }

    function updateToolBarButtonsOnSettingsChanged() {
      unappliedSettings = true
      unsavedSettings = false
    }

    function updateToolBarButtonsOnProfileChanged() {
      unappliedSettings = false
      unsavedSettings = true
    }

    function updateToolBarButtonsOnProfileSaved() {
      unsavedSettings = false
    }

    function updateProfileUIState(name) {
      if (editedProfileBtn !== undefined) {
        var active = profileManager.isProfileActive(name)
        updateToolBarProfileInfo(editedProfileBtn.label, tbIcon.source, active)

        if (restoringProfile) {
          restoringProfile = false
          updateToolBarButtonsOnProfileSaved()
        }
        else {
          updateToolBarButtonsOnProfileChanged()
        }
      }
    }

    function goToProfileView() {
      if (unappliedSettings)
        applyProfileSettingsDlg.open()
      else
        exitProfileSettings()
    }

    function exitProfileSettings() {
      stack.pop()
      sysModelView.visible = false

      unappliedSettings = false
      unsavedSettings = false
      editedProfileBtn = undefined
    }

    function goToProfileSettingsView() {
      sysModelView.visible = true
      stack.push(sysModelView)
    }
  }

  ListModel {
    id: profileModel
  }

  Component {
    id: profileModelDelegate

    ProfileButton {
      id: pBtn

      label: _label
      name: _name
      icon: _icon
      exe: _exe
      isGlobal: _isGlobal
      profileActivated: _active
      toggledManual: _toggledManual

      onClicked: {
        p.editedProfileBtn = pBtn
        p.updateToolBarProfileInfo(label, icon, profileActivated)
        profileManager.loadSettings(name)
        p.goToProfileSettingsView()

        if (profileManager.isProfileUnsaved(name))
          p.unsavedSettings = true
      }

      onEnableProfile: {
        profileManager.activate(name, enable)
      }

      onEdit: {
        p.editedProfileBtn = pBtn

        infoDlg.title = qsTr("Profile properties")
        infoDlg.name = name
        infoDlg.exe = exe
        infoDlg.icon = icon
        infoDlg.hasCustomIcon = _hasCustomIcon
        infoDlg.newInfoAction = function(name, exe, icon) {
          p.editedProfileBtn = undefined
          if (pBtn.name !== name || pBtn.exe !== exe || pBtn.icon !== icon)
            profileManager.updateInfo(pBtn.name, name, exe, icon)
        }
        infoDlg.open()
      }

      onClone: {
        infoDlg.title = qsTr("New profile properties")
        infoDlg.name = ""
        infoDlg.exe = ""
        infoDlg.icon = isGlobal ? infoDlg.defaultIcon : icon
        infoDlg.hasCustomIcon = _hasCustomIcon
        infoDlg.forceAutomaticActivation = exe.length > 0
        infoDlg.newInfoAction = function(name, exe, icon) {
          profileManager.add(name, exe, icon, pBtn.name)
        }
        infoDlg.open()
      }

      onExportTo: {
        exportToDialog.profileName = name
        exportToDialog.open()
      }

      onRemove: {
        removeWarningDlg.profileBtn = pBtn
        removeWarningDlg.profileName = pBtn.name
        removeWarningDlg.open()
      }

      onToggleManualProfile: {
        profileManager.toggleManualProfile(name)
      }
    }
  }

  NativeFileDialog {
    id: exportToDialog
    title: qsTr("Export profile to...")
    nameFilters: [qsTr("CoreCtrl profile") + " (ccpro)(*.ccpro)"]
    defaultSuffix: "ccpro"
    saveDlg: true

    property string profileName: ""

    onAccepted: {
      if(!profileManager.exportProfile(profileName, file)) {
        errorDlg.textDlg = qsTr("Cannot export profile.\nCheck the permissions of the destination file and directory.")
        errorDlg.open()
      }
    }
  }

  ProfileInfoDialog {
    id: infoDlg
    defaultIcon: profileManager.defaultIcon()

    function openNewProfileDialog() {
      title = qsTr("New profile properties")
      name = ""
      exe = ""
      icon = defaultIcon
      hasCustomIcon = false
      newInfoAction = function(name, exe, icon) {
        profileManager.add(name, exe, icon, "defaultProfile")
      }
      open()
    }

    onUpdateProfileNameUsed: {
      if (p.editedProfileBtn !== undefined && profileName === p.editedProfileBtn.name)
        profileNameUsed = false
      else
        profileNameUsed = profileManager.isProfileNameInUse(profileName)
    }
    onUpdateExecutableNameUsed: {
      if (p.editedProfileBtn !== undefined && executableName === p.editedProfileBtn.exe)
        executableNameUsed = false
      else
        executableNameUsed = profileManager.isExecutableNameInUse(executableName)
    }
  }

  Dialog {
    id: errorDlg

    property alias textDlg: errorDlgLbl.text

    title: qsTr("Error")

    focus: true
    modal: true
    standardButtons: Dialog.Ok
    closePolicy: Popup.CloseOnEscape

    // center in parent
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    Label { id: errorDlgLbl }

    Component.onCompleted: {
      footer.standardButton(Dialog.Ok).hoverEnabled = Style.g_hover
    }
  }

  Dialog {
    id: removeWarningDlg

    title: qsTr("Warning")

    property var profileName
    property var profileBtn

    focus: true
    modal: true
    standardButtons: Dialog.Yes | Dialog.No
    closePolicy: Popup.CloseOnEscape

    // center in parent
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    Label {
      text: qsTr("This action is permantent.\nDo you really want to remove %1?")
              .arg(removeWarningDlg.profileName)
    }

    onAccepted: profileManager.remove(profileName)

    Component.onCompleted: {
      footer.standardButton(Dialog.Yes).hoverEnabled = Style.g_hover
      footer.standardButton(Dialog.No).hoverEnabled = Style.g_hover
    }
  }

  Dialog {
    id: applyProfileSettingsDlg

    title: qsTr("Warning")

    focus: true
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel
    closePolicy: Popup.CloseOnEscape

    // center in parent
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    Label {
      text: qsTr("Unapplied settings will be lost.\nDo you want to apply them now?")
    }

    onAccepted: {
      profileManager.applySettings(p.editedProfileBtn.name)
      p.exitProfileSettings()
      close()
    }

    onRejected: {
      p.exitProfileSettings()
      close()
    }

    Component.onCompleted: {
      footer.standardButton(Dialog.Ok).hoverEnabled = Style.g_hover
      footer.standardButton(Dialog.Cancel).hoverEnabled = Style.g_hover
    }
  }

  Dialog {
    id: resetProfileSettingsDlg

    title: qsTr("Warning")

    focus: true
    modal: true
    standardButtons: Dialog.Yes | Dialog.No
    closePolicy: Popup.CloseOnEscape

    // center in parent
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    Label {
      text: (p.unsavedSettings ? qsTr("Unsaved settings will be lost.\n") : "") +
            qsTr("Do you want to load the default settings?")
    }

    onAccepted: {
      profileManager.resetSettings(p.editedProfileBtn.name)
    }

    Component.onCompleted: {
      footer.standardButton(Dialog.Yes).hoverEnabled = Style.g_hover
      footer.standardButton(Dialog.No).hoverEnabled = Style.g_hover
    }
  }

  Dialog {
    id: restoreProfileSettingsDlg

    title: qsTr("Warning")

    focus: true
    modal: true
    standardButtons: Dialog.Yes | Dialog.No
    closePolicy: Popup.CloseOnEscape

    // center in parent
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    Label {
      text: (p.unsavedSettings ? qsTr("Current settings will be discarded.\n") : "") +
            qsTr("Do you want to load the saved settings?")
    }

    onAccepted: {
      p.restoringProfile = true
      profileManager.restoreSettings(p.editedProfileBtn.name)
    }

    Component.onCompleted: {
      footer.standardButton(Dialog.Yes).hoverEnabled = Style.g_hover
      footer.standardButton(Dialog.No).hoverEnabled = Style.g_hover
    }
  }

  header: ToolBar {
    Material.elevation: Style.ToolBar.Material.elevation
    Material.background: Style.ToolBar.bg_color

    RowLayout {
      anchors.fill: parent

      Label {
        text: qsTr("Manage profiles for your applications...")
        leftPadding: 10
        color: Style.ToolBar.text_color_msg
        visible: stack.depth === 1
      }

      ToolButton {
        text: Style.g_icon.BACK
        font.pointSize: Style.g_text.icon_size_tabbar
        visible: stack.depth > 1
        onClicked: p.goToProfileView()
      }

      Item {
        implicitWidth: tbIcon.width
        implicitHeight: tbIcon.height

        Image {
          id: tbIcon
          anchors.fill: parent
          fillMode: Image.PreserveAspectFit
          width: Style.g_icon.size
          height: Style.g_icon.size
          sourceSize.width: Style.g_icon.source_size
          visible: stack.depth > 1
        }
      }

      Label {
        id: tbLabel
        leftPadding: 5
        font.pointSize: Style.g_text.size + 2
        elide: Text.ElideRight
        Layout.maximumWidth: 300
        visible: stack.depth > 1
      }

      Rectangle {
        Layout.fillWidth: true
      }

      ToolButton {
        text: Style.g_icon.ADD
        font.pointSize: Style.g_text.icon_size_tabbar
        visible: stack.depth === 1
        onClicked: infoDlg.openNewProfileDialog()
      }

      ToolButton {
        text: qsTr("Save")
        visible: stack.depth > 1 && p.unsavedSettings
        onClicked: profileManager.saveSettings(p.editedProfileBtn.name)
      }

      ToolButton {
        text: qsTr("Apply")
        visible: stack.depth > 1 && p.unappliedSettings
        onClicked: profileManager.applySettings(p.editedProfileBtn.name)
      }

      ToolButton {
        text: qsTr("Restore")
        visible: stack.depth > 1 && p.unsavedSettings
        onClicked: restoreProfileSettingsDlg.open()
      }

      Label {
        text: Style.g_icon.SPLIT
        font.pointSize: Style.g_text.icon_size
        visible: stack.depth > 1
      }

      ToolButton {
        text: qsTr("Load from...")
        visible: stack.depth > 1

        NativeFileDialog {
          id: importFDialog
          title: qsTr("Load settings from...")
          nameFilters: [qsTr("CoreCtrl profile") + " (ccpro)(*.ccpro)"]
          defaultSuffix: "ccpro"

          onAccepted: {
            if (!profileManager.loadSettings(p.editedProfileBtn.name, file)) {
              errorDlg.textDlg = qsTr("Cannot load profile.\nInvalid or corrupted file.")
              errorDlg.open()
            }
          }
        }
        onClicked: importFDialog.open()
      }

      ToolButton {
        text: qsTr("Reset")
        visible: stack.depth > 1
        onClicked: resetProfileSettingsDlg.open()
      }
    }
  }

  StackView {
    id: stack
    initialItem: profilesView
    anchors.fill: parent
    clip: true

    Item {
      id: sysModelView
      objectName: "PROFILE_SYS_MODEL_Plug"
      visible: false

      onChildrenChanged: {
        for (var i = 0; i < children.length; ++i) {
          children[i].settingsChanged.disconnect(p.updateToolBarButtonsOnSettingsChanged)
          children[i].settingsChanged.connect(p.updateToolBarButtonsOnSettingsChanged)
        }
      }
    }

    Pane {
      id: profilesView
      padding: 2

      ScrollView {
        id: scrollview
        anchors.fill: parent
        clip: true
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOn
        ScrollBar.vertical.policy: ScrollBar.AlwaysOn
        ScrollBar.horizontal.visible: ScrollBar.horizontal.size < 1
        ScrollBar.vertical.visible: ScrollBar.vertical.size < 1

        Flow {
          width: scrollview.availableWidth
          height: scrollview.availableHeight
          spacing: Style.RectItemList.items_spacing

          Repeater {
            model: profileModel
            delegate: profileModelDelegate
          }

          move: Transition {
            NumberAnimation { properties: "x,y"; easing.type: Easing.InOutQuad }
          }
        }
      }
    }

    pushEnter: Transition {
      PropertyAnimation {
        property: "opacity"
        from: 0
        to: 1
        duration: Style.Profiles.opacity_anim_duration
      }
    }
    pushExit: Transition {
      PropertyAnimation {
        property: "opacity"
        from: 1
        to: 0
        duration: Style.Profiles.opacity_anim_duration
      }
    }
    popEnter: Transition {
      PropertyAnimation {
        property: "opacity"
        from: 0
        to: 1
        duration: Style.Profiles.opacity_anim_duration
      }
    }
    popExit: Transition {
      PropertyAnimation {
        property: "opacity"
        from: 1
        to: 0
        duration: Style.Profiles.opacity_anim_duration
      }
    }
  }
}
