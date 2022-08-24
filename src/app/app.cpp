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

    setupMainWindowBasedOnSysTrayState();

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
  auto show = true;

  cmdParser_.parse(args);
  if (cmdParser_.isSet("minimize-systray"))
    show = false;

  showMainWindow(show);
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

void App::setupMainWindowBasedOnSysTrayState()
{
  auto minimizeArgIsSet = cmdParser_.isSet("minimize-systray");
  if (minimizeArgIsSet || settings_->getValue("sysTray", true).toBool()) {

    sysTray_->show();
    showMainWindow(minimizeArgIsSet
                       ? false
                       : !settings_->getValue("startOnSysTray", false).toBool());
  }
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
  });
}

void App::buildUI(QQmlApplicationEngine &qmlEngine)
{
  sysTray_ = new SysTray(&*session_, QApplication::instance());
  connect(sysTray_, &SysTray::quit, this, &QApplication::quit);
  connect(sysTray_, &SysTray::activated, this, &App::onSysTrayActivated);
  connect(sysTray_, &SysTray::showMainWindowToggled, this, &App::showMainWindow);

  qmlEngine.rootContext()->setContextProperty("appInfo", &appInfo_);
  qmlEngine.rootContext()->setContextProperty("settings", &*settings_);
  qmlEngine.rootContext()->setContextProperty("systemTray", sysTray_);

  uiFactory_->build(qmlEngine, sysSyncer_->sysModel(), *session_);
  mainWindow_ = qobject_cast<QQuickWindow *>(qmlEngine.rootObjects().value(0));
  connect(mainWindow_, &QQuickWindow::visibleChanged, &*sysTray_,
          &SysTray::onMainWindowVisibleChanged);
  connect(&qmlEngine, &QQmlApplicationEngine::quit, QApplication::instance(),
          &QApplication::quit);
  connect(QApplication::instance(), &QApplication::aboutToQuit, this, &App::exit);
  connect(&*settings_, &Settings::settingChanged, this, &App::onSettingChanged);
  connect(&singleInstance_, &SingleInstance::newInstance, this,
          &App::onNewInstance);
}
