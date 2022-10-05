configure_file(config.h.in config.h)

list(APPEND 3RDPARTY_INCLUDE_DIRECTORIES
  ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty
)

list (APPEND 3RDPARTY_COMMON_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/fmt/format.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/easyloggingpp/easylogging++.cc
)

list (APPEND 3RDPARTY_COMMON_DEFINITIONS
  ELPP_THREAD_SAFE
  ELPP_FORCE_USE_STD_THREAD
  ELPP_NO_LOG_TO_FILE
  ELPP_NO_DEFAULT_LOG_FILE
)

list (APPEND 3RDPARTY_SRC
  ${3RDPARTY_COMMON_SRC}
  ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/pugixml/pugixml.cpp
)

list (APPEND 3RDPARTY_DEFINITIONS
  ${3RDPARTY_COMMON_DEFINITIONS}
  UNIT_LIB_DISABLE_IOSTREAM
  DISABLE_PREDEFINED_UNITS
  ENABLE_PREDEFINED_DATA_UNITS
  ENABLE_PREDEFINED_ANGLE_UNITS
  ENABLE_PREDEFINED_TIME_UNITS
  ENABLE_PREDEFINED_ANGULAR_VELOCITY_UNITS
  ENABLE_PREDEFINED_FREQUENCY_UNITS
  ENABLE_PREDEFINED_VOLTAGE_UNITS
  ENABLE_PREDEFINED_POWER_UNITS
  ENABLE_PREDEFINED_TEMPERATURE_UNITS
  ENABLE_PREDEFINED_CONVERT_UNITS
  ENABLE_PREDEFINED_CONCENTRATION_UNITS
)

list(APPEND COMMON_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/common/fileutils.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/common/stringutils.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/common/cryptolayer.cpp
)

list(APPEND CORE_SRC
  core/filecache.cpp
  core/profileiconcache.cpp
  core/commandqueue.cpp
  core/sysmodel.cpp
  core/sysexplorer.cpp
  core/sysmodelfactory.cpp
  core/sysmodelsyncer.cpp
  core/session.cpp
  core/profileview.cpp
  core/profilepartview.cpp
  core/profileviewfactory.cpp
  core/profilestorage.cpp
  core/profilexmlparserfactory.cpp
  core/profilepartxmlparserprovider.cpp
  core/profilexmlparser.cpp
  core/profilepartxmlparser.cpp
  core/profilemanager.cpp
  core/profile.cpp
  core/profilepart.cpp
  core/profilepartprovider.cpp
  core/profilefactory.cpp
  core/ccpro/zipdatasource.cpp
  core/ccpro/zipdatasink.cpp
  core/ccpro/ccproparser.cpp
  core/info/cpuinfo.cpp
  core/info/gpuinfo.cpp
  core/info/swinfo.cpp
  core/info/hwiddatasource.cpp
  core/info/hwidtranslator.cpp
  core/components/commonutils.cpp
  core/components/cpuutils.cpp
  core/components/amdutils.cpp
  core/components/gpu.cpp
  core/components/gpuprofilepart.cpp
  core/components/gpuxmlparser.cpp
  core/components/cpu.cpp
  core/components/cpuprofilepart.cpp
  core/components/cpuxmlparser.cpp
  core/components/controls/control.cpp
  core/components/controls/controlmode.cpp
  core/components/controls/controlmodeprofilepart.cpp
  core/components/controls/controlmodexmlparser.cpp
  core/components/controls/controlgroup.cpp
  core/components/controls/controlgroupprofilepart.cpp
  core/components/controls/controlgroupxmlparser.cpp
)

list(APPEND INFO_SRC
  core/info/infoproviderregistry.cpp
  core/info/common/cpuinfolscpu.cpp
  core/info/common/cpuinfoproccpuinfo.cpp
  core/info/common/gpuinfouevent.cpp
  core/info/common/gpuinforevision.cpp
  core/info/common/gpuinfoopengl.cpp
  core/info/common/gpuinfovulkan.cpp
  core/info/common/swinfokernel.cpp
  core/info/common/swinfomesa.cpp
  core/info/amd/gpuinfopm.cpp
  core/info/amd/gpuinfopmoverdrive.cpp
  core/info/amd/gpuinfovbios.cpp
  core/info/amd/gpuinfovram.cpp
  core/info/amd/gpuinfouniqueid.cpp
)

list(APPEND CONTROLS_SRC
  core/components/controls/noop.cpp
  core/components/controls/noopprofilepart.cpp
  core/components/controls/noopxmlparser.cpp
  core/components/controls/gpucontrolprovider.cpp
  core/components/controls/amd/pm/handlers/ppdpmhandler.cpp
  core/components/controls/amd/pm/pmperfmodeprovider.cpp
  core/components/controls/amd/pm/pmperfmodeprofilepart.cpp
  core/components/controls/amd/pm/pmperfmodexmlparser.cpp
  core/components/controls/amd/pm/auto/pmauto.cpp
  core/components/controls/amd/pm/auto/pmautolegacy.cpp
  core/components/controls/amd/pm/auto/pmautor600.cpp
  core/components/controls/amd/pm/auto/pmautoprovider.cpp
  core/components/controls/amd/pm/auto/pmautoprofilepart.cpp
  core/components/controls/amd/pm/auto/pmautoxmlparser.cpp
  core/components/controls/amd/pm/fixed/pmfixed.cpp
  core/components/controls/amd/pm/fixed/pmfixedlegacy.cpp
  core/components/controls/amd/pm/fixed/pmfixedr600.cpp
  core/components/controls/amd/pm/fixed/pmfixedprovider.cpp
  core/components/controls/amd/pm/fixed/pmfixedprofilepart.cpp
  core/components/controls/amd/pm/fixed/pmfixedxmlparser.cpp
  core/components/controls/amd/pm/advanced/pmadvancedprovider.cpp
  core/components/controls/amd/pm/advanced/pmadvancedprofilepart.cpp
  core/components/controls/amd/pm/advanced/pmadvancedxmlparser.cpp
  core/components/controls/amd/pm/advanced/dynamicfreq/pmdynamicfreq.cpp
  core/components/controls/amd/pm/advanced/dynamicfreq/pmdynamicfreqprovider.cpp
  core/components/controls/amd/pm/advanced/dynamicfreq/pmdynamicfreqprofilepart.cpp
  core/components/controls/amd/pm/advanced/dynamicfreq/pmdynamicfreqxmlparser.cpp
  core/components/controls/amd/pm/advanced/fixedfreq/pmfixedfreq.cpp
  core/components/controls/amd/pm/advanced/fixedfreq/pmfixedfreqprovider.cpp
  core/components/controls/amd/pm/advanced/fixedfreq/pmfixedfreqadvprovider.cpp
  core/components/controls/amd/pm/advanced/fixedfreq/pmfixedfreqprofilepart.cpp
  core/components/controls/amd/pm/advanced/fixedfreq/pmfixedfreqxmlparser.cpp
  core/components/controls/amd/pm/advanced/freqmode/pmfreqmodeprovider.cpp
  core/components/controls/amd/pm/advanced/freqmode/pmfreqmodeprofilepart.cpp
  core/components/controls/amd/pm/advanced/freqmode/pmfreqmodexmlparser.cpp
  core/components/controls/amd/pm/advanced/overclock/pmoverclockprovider.cpp
  core/components/controls/amd/pm/advanced/overclock/pmoverclockprofilepart.cpp
  core/components/controls/amd/pm/advanced/overclock/pmoverclockxmlparser.cpp
  core/components/controls/amd/pm/advanced/overclock/freqod/pmfreqod.cpp
  core/components/controls/amd/pm/advanced/overclock/freqod/pmfreqodprovider.cpp
  core/components/controls/amd/pm/advanced/overclock/freqod/pmfreqodprofilepart.cpp
  core/components/controls/amd/pm/advanced/overclock/freqod/pmfreqodxmlparser.cpp
  core/components/controls/amd/pm/advanced/overdrive/pmoverdrive.cpp
  core/components/controls/amd/pm/advanced/overdrive/pmoverdriveprovider.cpp
  core/components/controls/amd/pm/advanced/overdrive/pmoverdriveprofilepart.cpp
  core/components/controls/amd/pm/advanced/overdrive/pmoverdrivexmlparser.cpp
  core/components/controls/amd/pm/advanced/overdrive/freqvolt/pmfreqvolt.cpp
  core/components/controls/amd/pm/advanced/overdrive/freqvolt/pmfreqvoltprovider.cpp
  core/components/controls/amd/pm/advanced/overdrive/freqvolt/pmfreqvoltprofilepart.cpp
  core/components/controls/amd/pm/advanced/overdrive/freqvolt/pmfreqvoltxmlparser.cpp
  core/components/controls/amd/pm/advanced/overdrive/freqrange/pmfreqrange.cpp
  core/components/controls/amd/pm/advanced/overdrive/freqrange/pmfreqrangeprovider.cpp
  core/components/controls/amd/pm/advanced/overdrive/freqrange/pmfreqrangeprofilepart.cpp
  core/components/controls/amd/pm/advanced/overdrive/freqrange/pmfreqrangexmlparser.cpp
  core/components/controls/amd/pm/advanced/overdrive/voltcurve/pmvoltcurve.cpp
  core/components/controls/amd/pm/advanced/overdrive/voltcurve/pmvoltcurveprovider.cpp
  core/components/controls/amd/pm/advanced/overdrive/voltcurve/pmvoltcurveprofilepart.cpp
  core/components/controls/amd/pm/advanced/overdrive/voltcurve/pmvoltcurvexmlparser.cpp
  core/components/controls/amd/pm/advanced/overdrive/voltoffset/pmvoltoffset.cpp
  core/components/controls/amd/pm/advanced/overdrive/voltoffset/pmvoltoffsetprovider.cpp
  core/components/controls/amd/pm/advanced/overdrive/voltoffset/pmvoltoffsetprofilepart.cpp
  core/components/controls/amd/pm/advanced/overdrive/voltoffset/pmvoltoffsetxmlparser.cpp
  core/components/controls/amd/pm/advanced/powerprofile/pmpowerprofile.cpp
  core/components/controls/amd/pm/advanced/powerprofile/pmpowerprofileprovider.cpp
  core/components/controls/amd/pm/advanced/powerprofile/pmpowerprofileprofilepart.cpp
  core/components/controls/amd/pm/advanced/powerprofile/pmpowerprofilexmlparser.cpp
  core/components/controls/amd/pm/advanced/powercap/pmpowercap.cpp
  core/components/controls/amd/pm/advanced/powercap/pmpowercapprovider.cpp
  core/components/controls/amd/pm/advanced/powercap/pmpowercapprofilepart.cpp
  core/components/controls/amd/pm/advanced/powercap/pmpowercapxmlparser.cpp
  core/components/controls/amd/pm/powerstate/pmpowerstate.cpp
  core/components/controls/amd/pm/powerstate/pmpowerstateprovider.cpp
  core/components/controls/amd/pm/powerstate/pmpowerstateprofilepart.cpp
  core/components/controls/amd/pm/powerstate/pmpowerstatexmlparser.cpp
  core/components/controls/amd/pm/powerstate/pmpowerstatemodeprovider.cpp
  core/components/controls/amd/pm/powerstate/pmpowerstatemodeprofilepart.cpp
  core/components/controls/amd/pm/powerstate/pmpowerstatemodexmlparser.cpp
  core/components/controls/amd/fan/fanmodeprovider.cpp
  core/components/controls/amd/fan/fanmodeprofilepart.cpp
  core/components/controls/amd/fan/fanmodexmlparser.cpp
  core/components/controls/amd/fan/auto/fanauto.cpp
  core/components/controls/amd/fan/auto/fanautoprovider.cpp
  core/components/controls/amd/fan/auto/fanautoprofilepart.cpp
  core/components/controls/amd/fan/auto/fanautoxmlparser.cpp
  core/components/controls/amd/fan/fixed/fanfixed.cpp
  core/components/controls/amd/fan/fixed/fanfixedprovider.cpp
  core/components/controls/amd/fan/fixed/fanfixedprofilepart.cpp
  core/components/controls/amd/fan/fixed/fanfixedxmlparser.cpp
  core/components/controls/amd/fan/curve/fancurve.cpp
  core/components/controls/amd/fan/curve/fancurveprovider.cpp
  core/components/controls/amd/fan/curve/fancurveprofilepart.cpp
  core/components/controls/amd/fan/curve/fancurvexmlparser.cpp
  core/components/controls/cpucontrolprovider.cpp
  core/components/controls/cpu/cpufreq.cpp
  core/components/controls/cpu/cpufreqprovider.cpp
  core/components/controls/cpu/cpufreqprofilepart.cpp
  core/components/controls/cpu/cpufreqxmlparser.cpp
  core/components/controls/cpu/cpufreqmodeprovider.cpp
  core/components/controls/cpu/cpufreqmodeprofilepart.cpp
  core/components/controls/cpu/cpufreqmodexmlparser.cpp
)

list(APPEND SENSORS_SRC
  core/components/sensors/graphitemprofilepart.cpp
  core/components/sensors/graphitemxmlparser.cpp
  core/components/sensors/gpusensorprovider.cpp
  core/components/sensors/amd/gpufreq.cpp
  core/components/sensors/amd/memfreq.cpp
  core/components/sensors/amd/gputemp.cpp
  core/components/sensors/amd/junctiontemp.cpp
  core/components/sensors/amd/memorytemp.cpp
  core/components/sensors/amd/gpuvolt.cpp
  core/components/sensors/amd/power.cpp
  core/components/sensors/amd/activity.cpp
  core/components/sensors/amd/memusage.cpp
  core/components/sensors/amd/fanspeedrpm.cpp
  core/components/sensors/amd/fanspeedperc.cpp
  core/components/sensors/cpusensorprovider.cpp
  core/components/sensors/cpu/cpufreqpack.cpp
)

list(APPEND UI_COMPONENTS_SRC
  core/uifactory.cpp
  core/qmlitem.cpp
  core/profilemanagerui.cpp
  core/systeminfoui.cpp
  core/qmlcomponentfactory.cpp
  core/qmlcomponentregistry.cpp
  core/sysmodelqmlitem.cpp
  core/components/gpuqmlitem.cpp
  core/components/cpuqmlitem.cpp
  core/components/sensors/graphitem.cpp
  core/components/sensors/amd/gpufreqgraphitem.cpp
  core/components/sensors/amd/memfreqgraphitem.cpp
  core/components/sensors/amd/gputempgraphitem.cpp
  core/components/sensors/amd/junctiontempgraphitem.cpp
  core/components/sensors/amd/memorytempgraphitem.cpp
  core/components/sensors/amd/gpuvoltgraphitem.cpp
  core/components/sensors/amd/powergraphitem.cpp
  core/components/sensors/amd/activitygraphitem.cpp
  core/components/sensors/amd/memusagegraphitem.cpp
  core/components/sensors/amd/fanspeedrpmgraphitem.cpp
  core/components/sensors/amd/fanspeedpercgraphitem.cpp
  core/components/sensors/cpu/cpufreqpackgraphitem.cpp
  core/components/controls/controlmodeqmlitem.cpp
  core/components/controls/controlgroupqmlitem.cpp
  core/components/controls/noopqmlitem.cpp
  core/components/controls/amd/pm/pmperfmodeqmlitem.cpp
  core/components/controls/amd/pm/auto/pmautoqmlitem.cpp
  core/components/controls/amd/pm/fixed/pmfixedqmlitem.cpp
  core/components/controls/amd/pm/advanced/pmadvancedqmlitem.cpp
  core/components/controls/amd/pm/advanced/dynamicfreq/pmdynamicfreqqmlitem.cpp
  core/components/controls/amd/pm/advanced/fixedfreq/pmfixedfreqqmlitem.cpp
  core/components/controls/amd/pm/advanced/freqmode/pmfreqmodeqmlitem.cpp
  core/components/controls/amd/pm/advanced/overclock/pmoverclockqmlitem.cpp
  core/components/controls/amd/pm/advanced/overclock/freqod/pmfreqodqmlitem.cpp
  core/components/controls/amd/pm/advanced/overdrive/pmoverdriveqmlitem.cpp
  core/components/controls/amd/pm/advanced/overdrive/freqvolt/pmfreqvoltqmlitem.cpp
  core/components/controls/amd/pm/advanced/overdrive/freqrange/pmfreqrangeqmlitem.cpp
  core/components/controls/amd/pm/advanced/overdrive/voltcurve/pmvoltcurveqmlitem.cpp
  core/components/controls/amd/pm/advanced/overdrive/voltoffset/pmvoltoffsetqmlitem.cpp
  core/components/controls/amd/pm/advanced/powerprofile/pmpowerprofileqmlitem.cpp
  core/components/controls/amd/pm/advanced/powercap/pmpowercapqmlitem.cpp
  core/components/controls/amd/pm/powerstate/pmpowerstateqmlitem.cpp
  core/components/controls/amd/pm/powerstate/pmpowerstatemodeqmlitem.cpp
  core/components/controls/amd/fan/fanmodeqmlitem.cpp
  core/components/controls/amd/fan/auto/fanautoqmlitem.cpp
  core/components/controls/amd/fan/fixed/fanfixedqmlitem.cpp
  core/components/controls/amd/fan/curve/fancurveqmlitem.cpp
  core/components/controls/cpu/cpufreqqmlitem.cpp
  core/components/controls/cpu/cpufreqmodeqmlitem.cpp
)

list(APPEND APP_SRC
  app/app.cpp
  app/appfactory.cpp
  app/settings.cpp
  app/singleinstance.cpp
  app/systray.cpp
  helper/helpercontrol.cpp
  helper/helpermonitor.cpp
  helper/helpersysctl.cpp
)

list (APPEND TRANSLATABLE_FILES
  ${UI_COMPONENTS_SRC}
  app/systray.cpp
  qml/main.qml
  qml/ProfileButton.qml
  qml/ProfileInfoDialog.qml
  qml/FVControl.qml
  qml/FreqStateControl.qml
  qml/Profiles.qml
  qml/System.qml
  qml/About.qml
  qml/SettingsDialog.qml
  qml/SettingsGeneral.qml
  qml/SettingsWorkarounds.qml
  qml/SysModelForm.qml
  qml/CPUForm.qml
  qml/GPUForm.qml
  qml/SensorGraph.qml
  qml/NoopForm.qml
  qml/AMDPMPerfModeForm.qml
  qml/AMDPMAutoForm.qml
  qml/AMDPMFixedForm.qml
  qml/AMDPMAdvancedForm.qml
  qml/AMDPMDynamicFreqForm.qml
  qml/AMDPMFixedFreqForm.qml
  qml/AMDPMFreqModeForm.qml
  qml/AMDPMOverclockForm.qml
  qml/AMDPMFreqVoltForm.qml
  qml/AMDPMFreqRangeForm.qml
  qml/AMDPMVoltCurveForm.qml
  qml/AMDPMVoltOffsetForm.qml
  qml/AMDPMFreqOdForm.qml
  qml/AMDPMPowerProfileForm.qml
  qml/AMDPMPowerCapForm.qml
  qml/AMDPMPowerStateForm.qml
  qml/AMDPMPowerStateModeForm.qml
  qml/AMDFanModeForm.qml
  qml/AMDFanAutoForm.qml
  qml/AMDFanFixedForm.qml
  qml/AMDFanCurveForm.qml
  qml/CPUFreqForm.qml
  qml/CPUFreqModeForm.qml
)

list(APPEND TRANSLATIONS
  translations/lang_en_EN.ts
  translations/lang_es_ES.ts
  translations/lang_ru_RU.ts
  translations/lang_fr_FR.ts
  translations/lang_ca_ES.ts
  translations/lang_bg_BG.ts
  translations/lang_cs_CZ.ts
  translations/lang_nl_NL.ts
  translations/lang_de_DE.ts
)

set_source_files_properties(${TRANSLATIONS} PROPERTIES
  OUTPUT_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/translations
)

option (UPDATE_TRANSLATIONS "Update translation files" OFF)
if (UPDATE_TRANSLATIONS)
  qt5_create_translation(QM_FILES ${TRANSLATABLE_FILES} ${TRANSLATIONS} OPTIONS -noobsolete)
else (UPDATE_TRANSLATIONS)
  qt5_add_translation(QM_FILES ${TRANSLATIONS})
endif (UPDATE_TRANSLATIONS)
add_custom_target(translations DEPENDS ${QM_FILES})

qt5_add_resources(CPP_QRC resources.qrc)

set(CMAKE_AUTOMOC ON)
add_library(corectrl_lib SHARED
  ${3RDPARTY_SRC}
  ${COMMON_SRC}
  ${CORE_SRC}
  ${INFO_SRC}
  ${CONTROLS_SRC}
  ${SENSORS_SRC}
  ${APP_SRC}
  ${UI_COMPONENTS_SRC}
)
set(CMAKE_AUTOMOC OFF)

target_include_directories(corectrl_lib PRIVATE
  ${CMAKE_CURRENT_BINARY_DIR}
  ${3RDPARTY_INCLUDE_DIRECTORIES}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${QUAZIP_INCLUDE_DIRS}
  ${Botan_INCLUDE_DIRS}
)
target_compile_features(corectrl_lib PRIVATE ${TARGET_COMMON_COMPILE_FEATURES})
target_compile_definitions(corectrl_lib PRIVATE ${3RDPARTY_DEFINITIONS})
target_link_libraries(corectrl_lib PRIVATE
  Qt5::Quick
  Qt5::Charts
  Qt5::Widgets
  Qt5::DBus
  stdc++fs
  pthread
  ${QUAZIP_LIBRARIES}
  ${Botan_LIBRARIES}
)
set_target_properties(corectrl_lib PROPERTIES OUTPUT_NAME corectrl)

add_executable(corectrl
  main.cpp
  ${CPP_QRC}
  ${QM_FILES}
)
target_include_directories(corectrl PRIVATE
  ${3RDPARTY_INCLUDE_DIRECTORIES}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${Botan_INCLUDE_DIRS}
)
target_compile_features(corectrl PRIVATE ${TARGET_COMMON_COMPILE_FEATURES})
target_compile_definitions(corectrl PRIVATE ${3RDPARTY_DEFINITIONS})
target_link_libraries(corectrl PRIVATE
  corectrl_lib
  Qt5::Core
  Qt5::Network
  Qt5::Widgets
)

install(TARGETS corectrl corectrl_lib
        RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})

add_subdirectory(helper)
