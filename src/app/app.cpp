// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "app.h"

#include "common/stringutils.h"
#include "core/isession.h"
#include "core/isysmodelsyncer.h"
#include "core/iuifactory.h"
#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include "helper/ihelpercontrol.h"
#include "settings.h"
#include "systray.h"
#include "units/units.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QIcon>
#include <QLocale>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QTranslator>
#include <Qt>
#include <QtGlobal>
#include <algorithm>
#include <utility>

#if defined(_DEBUG)
#include <QQmlDebuggingEnabler>
#endif

App::App(std::unique_ptr<IHelperControl> &&helperControl,
         std::shared_ptr<ISysModelSyncer> sysSyncer,
         std::unique_ptr<ISession> &&session,
         std::unique_ptr<IUIFactory> &&uiFactory) noexcept
: QObject()
, appInfo_(App::Name, App::VersionStr)
, singleInstance_(App::Name)
, helperControl_(std::move(helperControl))
, sysSyncer_(std::move(sysSyncer))
, session_(std::move(session))
, uiFactory_(std::move(uiFactory))
{
}

App::~App() = default;

int App::exec(int argc, char **argv)
{
  QCoreApplication::setApplicationName(QString(App::Name.data()).toLower());
  QCoreApplication::setApplicationVersion(App::VersionStr.data());
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

  // Ignore QT_STYLE_OVERRIDE. It breaks the qml theme.
  if (qEnvironmentVariableIsSet("QT_STYLE_OVERRIDE")) {
    LOG(INFO) << fmt::format(
        "Ignoring QT_STYLE_OVERRIDE environment variable.");
    qunsetenv("QT_STYLE_OVERRIDE");
  }

  QApplication app(argc, argv);

#if defined(_DEBUG)
  QQmlDebuggingEnabler enabler;
#endif

  int const minHelperTimeout = helperControl_->minExitTimeout().to<int>();
  int const helperTimeout{std::max(180000, // default helper timeout in milliseconds
                                   minHelperTimeout)};
  setupCmdParser(cmdParser_, minHelperTimeout, helperTimeout);
  cmdParser_.process(app);

  // exit if there is another instance running
  if (!singleInstance_.mainInstance(app.arguments()))
    return 0;

  noop_ = cmdParser_.isSet("help") || cmdParser_.isSet("version");
  if (noop_)
    return 0;

  QString lang = cmdParser_.isSet("lang") ? cmdParser_.value("lang")
                                          : QLocale().system().name();
  QTranslator translator;
  if (!translator.load(QStringLiteral(":/translations/lang_") + lang)) {
    LOG(INFO) << fmt::format("No translation found for locale {}",
                             lang.toStdString());
    LOG(INFO) << fmt::format("Using en_EN translation.");
    translator.load(QStringLiteral(":/translations/lang_en_EN"));
  }
  app.installTranslator(&translator);
  app.setWindowIcon(QIcon::fromTheme(QString(App::Name.data()).toLower()));

  // Ensure that the application do not implicitly call to quit after closing
  // the last window, which is not the desired behaviour when minimize to
  // system tray is being used.
  app.setQuitOnLastWindowClosed(false);

  try {
    settings_ = std::make_unique<Settings>(QString(App::Name.data()).toLower());

    int timeoutValue = helperTimeout;
    if (cmdParser_.isSet("helper-timeout") &&
        Utils::String::toNumber<int>(
            timeoutValue, cmdParser_.value("helper-timeout").toStdString())) {
      timeoutValue = std::max(helperControl_->minExitTimeout().to<int>(),
                              timeoutValue);
    }

    helperControl_->init(units::time::millisecond_t(timeoutValue));
    sysSyncer_->init();
    session_->init(sysSyncer_->sysModel());

    QQmlApplicationEngine qmlEngine;
    buildUI(qmlEngine);

    // Load and apply stored settings
    settings_->signalSettings();

    showMainWindow(!toSysTray());

    return app.exec();
  }
  catch (std::exception const &e) {
    LOG(WARNING) << e.what();
    LOG(WARNING) << "Initialization failed";
    LOG(WARNING) << "Exiting...";
    return -1;
  }

  return 0;
}

void App::exit()
{
  if (!noop_) {
    sysSyncer_->stop();
    helperControl_->stop();
  }
}

void App::showMainWindow(bool show)
{
  if (show) {
    mainWindow_->show();
    mainWindow_->raise();
    mainWindow_->requestActivate();
  }
  else {
    if (sysTray_->isVisible())
      mainWindow_->hide();
    else
      mainWindow_->showMinimized();
  }
}

void App::onNewInstance(QStringList args)
{
  cmdParser_.parse(args);

  if (cmdParser_.isSet("toggle-manual-profile")) {
    auto profileName = cmdParser_.value("toggle-manual-profile");
    if (!profileName.isEmpty() && profileName.length() < 512)
      session_->toggleManualProfile(profileName.toStdString());
  }
  else {
    auto show = true;

    if (cmdParser_.isSet("minimize-systray"))
      show = false;
    else if (cmdParser_.isSet("toggle-window-visibility")) {
      // When the window is minimized, calling show() will raise it.
      auto minimized =
          ((mainWindow_->windowState() & Qt::WindowState::WindowMinimized) ==
           Qt::WindowState::WindowMinimized);

      show = minimized ? true : !mainWindow_->isVisible();
    }

    showMainWindow(show);
  }
}

void App::onSysTrayActivated()
{
  showMainWindow(!mainWindow_->isVisible());
}

void App::onSettingChanged(QString const &key, QVariant const &value)
{
  sysTray_->settingChanged(key, value);
  sysSyncer_->settingChanged(key, value);
}

bool App::toSysTray()
{
  bool hideMainWindow{false};

  auto minimizeArgIsSet = cmdParser_.isSet("minimize-systray");
  if (minimizeArgIsSet || settings_->getValue("sysTray", true).toBool()) {

    sysTray_->show();
    hideMainWindow = minimizeArgIsSet
                         ? true
                         : settings_->getValue("startOnSysTray", false).toBool();
  }

  return hideMainWindow;
}

void App::setupCmdParser(QCommandLineParser &parser, int minHelperTimeout,
                         int helperTimeout) const
{
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addOptions({
      {{"l", "lang"},
       "Forces a specific <language>, given in locale format. Example: "
       "en_EN.",
       "language"},
      {{"m", "toggle-manual-profile"},
       "When an instance of the application is already running, it will toggle "
       "the manual profile whose name is <\"profile name\">.",
       "\"profile name\""},
      {"minimize-systray",
       "Minimizes the main window either to the system tray (when "
       "available) or to the taskbar.\nWhen an instance of the application is "
       "already running, the action will be applied to its main window."},
      {{"t", "helper-timeout"},
       "Sets helper auto exit timeout. "
       "The helper process kills himself when no signals are received from "
       "the application before the timeout expires.\nValues lesser than " +
           QString::number(minHelperTimeout) +
           +" milliseconds will be ignored.\nDefault value: " +
           QString::number(helperTimeout) + " milliseconds.",
       "milliseconds"},
      {"toggle-window-visibility",
       "When an instance of the application is already running, it will toggle "
       "the main window visibility showing or minimizing it, either to the "
       "taskbar or to system tray."},
  });
}

void App::buildUI(QQmlApplicationEngine &qmlEngine)
{
  qmlEngine.rootContext()->setContextProperty("appInfo", &appInfo_);
  qmlEngine.rootContext()->setContextProperty("settings", &*settings_);

  uiFactory_->build(qmlEngine, sysSyncer_->sysModel(), *session_);
  mainWindow_ = qobject_cast<QQuickWindow *>(qmlEngine.rootObjects().value(0));
  setupMainWindowGeometry();

  connect(&qmlEngine, &QQmlApplicationEngine::quit, QApplication::instance(),
          &QApplication::quit);
  connect(QApplication::instance(), &QApplication::aboutToQuit, this, &App::exit);
  connect(&*settings_, &Settings::settingChanged, this, &App::onSettingChanged);
  connect(&singleInstance_, &SingleInstance::newInstance, this,
          &App::onNewInstance);

  sysTray_ = new SysTray(&*session_, mainWindow_);
  connect(sysTray_, &SysTray::quit, this, &QApplication::quit);
  connect(sysTray_, &SysTray::activated, this, &App::onSysTrayActivated);
  connect(sysTray_, &SysTray::showMainWindowToggled, this, &App::showMainWindow);
  connect(mainWindow_, &QQuickWindow::visibleChanged, &*sysTray_,
          &SysTray::onMainWindowVisibleChanged);
  qmlEngine.rootContext()->setContextProperty("systemTray", sysTray_);
}

void App::setupMainWindowGeometry()
{
  restoreMainWindowGeometry();

  // The geometry save timer is used to reduce the window geometry changed
  // events fired within a time interval into a single event that will save the
  // window geometry.
  geometrySaveTimer_.setInterval(2000);
  geometrySaveTimer_.setSingleShot(true);
  connect(&geometrySaveTimer_, &QTimer::timeout, this,
          &App::saveMainWindowGeometry);

  connect(mainWindow_, &QWindow::heightChanged, this,
          [&](int) { geometrySaveTimer_.start(); });
  connect(mainWindow_, &QWindow::widthChanged, this,
          [&](int) { geometrySaveTimer_.start(); });
  connect(mainWindow_, &QWindow::xChanged, this,
          [&](int) { geometrySaveTimer_.start(); });
  connect(mainWindow_, &QWindow::yChanged, this,
          [&](int) { geometrySaveTimer_.start(); });
}

void App::saveMainWindowGeometry()
{
  if (!settings_->getValue("saveWindowGeometry", true).toBool())
    return;

  if (mainWindow_ == nullptr)
    return;

  auto windowGeometry = mainWindow_->geometry();

  auto savedXPos =
      settings_->getValue("Window/main-x-pos", DefaultWindowGeometry.x()).toInt();
  if (savedXPos != windowGeometry.x())
    settings_->setValue("Window/main-x-pos", windowGeometry.x());

  auto savedYPos =
      settings_->getValue("Window/main-y-pos", DefaultWindowGeometry.y()).toInt();
  if (savedYPos != windowGeometry.y())
    settings_->setValue("Window/main-y-pos", windowGeometry.y());

  auto savedWidth =
      settings_->getValue("Window/main-width", DefaultWindowGeometry.width())
          .toInt();
  if (savedWidth != windowGeometry.width())
    settings_->setValue("Window/main-width", windowGeometry.width());

  auto savedHeight =
      settings_->getValue("Window/main-height", DefaultWindowGeometry.height())
          .toInt();
  if (savedHeight != windowGeometry.height())
    settings_->setValue("Window/main-height", windowGeometry.height());
}

void App::restoreMainWindowGeometry()
{
  if (mainWindow_ == nullptr)
    return;

  auto x =
      settings_->getValue("Window/main-x-pos", DefaultWindowGeometry.x()).toInt();
  auto y =
      settings_->getValue("Window/main-y-pos", DefaultWindowGeometry.y()).toInt();
  auto width =
      settings_->getValue("Window/main-width", DefaultWindowGeometry.width())
          .toInt();
  auto height =
      settings_->getValue("Window/main-height", DefaultWindowGeometry.height())
          .toInt();

  mainWindow_->setGeometry(x, y, width, height);
}
