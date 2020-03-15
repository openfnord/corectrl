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
#include <algorithm>
#include <utility>

#if defined(_DEBUG)
#include <QQmlDebuggingEnabler>
#endif

App::App(std::unique_ptr<IHelperControl> &&helperControl,
         std::shared_ptr<ISysModelSyncer> sysSyncer,
         std::unique_ptr<ISession> &&session,
         std::unique_ptr<IUIFactory> &&uiFactory, QObject *parent) noexcept
: QObject(parent)
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

  QApplication app(argc, argv);

#if defined(_DEBUG)
  QQmlDebuggingEnabler enabler;
#endif

  int const helperTimeout{std::max(180000, // default helper timeout in milliseconds
                                   helperControl_->minExitTimeout().to<int>())};
  QCommandLineParser cmdParser;
  cmdParser.addHelpOption();
  cmdParser.addVersionOption();
  cmdParser.addOptions({
      {{"l", "lang"},
       "Forces a specific <language>, given in locale format. Example: "
       "en_EN.",
       "language"},
      {{"t", "helper-timeout"},
       "Sets helper auto exit timeout. "
       "The helper process kills himself when no signals are received from "
       "the "
       "application before the timeout expires.\nValues lesser than " +
           QString::number(helperControl_->minExitTimeout().to<int>()) +
           +" milliseconds will be ignored.\nDefault value: " +
           QString::number(helperTimeout) + " milliseconds.",
       "milliseconds"},
  });
  cmdParser.process(app);

  if (singleInstance_.tryMakeMainInstance()) {
    QString lang = cmdParser.isSet("lang") ? cmdParser.value("lang")
                                           : QLocale().system().name();
    QTranslator translator;
    if (!translator.load(QStringLiteral(":/translations/lang_") + lang)) {
      LOG(WARNING) << fmt::format("No translation found for locale {}",
                                  lang.toStdString());
      LOG(WARNING) << fmt::format("Using en_EN translation.");
      translator.load(QStringLiteral(":/translations/lang_en_EN"));
    }
    app.installTranslator(&translator);
    app.setWindowIcon(QIcon::fromTheme(QString(App::Name.data()).toLower()));

    noop_ = cmdParser.isSet("help") || cmdParser.isSet("version");
    if (!noop_) {
      try {
        settings_ =
            std::make_unique<Settings>(QString(App::Name.data()).toLower());

        int timeoutValue = helperTimeout;
        if (cmdParser.isSet("helper-timeout") &&
            Utils::String::toNumber<int>(
                timeoutValue, cmdParser.value("helper-timeout").toStdString())) {
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

        return app.exec();
      }
      catch (std::exception const &e) {
        LOG(WARNING) << e.what();
        LOG(WARNING) << "Initialization failed";
        LOG(WARNING) << "Exiting...";
        return -1;
      }
    }
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

void App::showMainWindow()
{
  if (mainWindow_ != nullptr) {
    mainWindow_->show();
    mainWindow_->raise();
    mainWindow_->requestActivate();
  }
}

void App::onSettingChanged(QString const &key, QVariant const &value)
{
  sysTray_->settingChanged(key, value);
  sysSyncer_->settingChanged(key, value);
}

void App::buildUI(QQmlApplicationEngine &qmlEngine)
{
  connect(&qmlEngine, &QQmlApplicationEngine::quit, QApplication::instance(),
          &QApplication::quit);
  connect(QApplication::instance(), &QApplication::aboutToQuit, this, &App::exit);
  connect(settings_.get(), &Settings::settingChanged, this,
          &App::onSettingChanged);

  sysTray_ = std::make_unique<SysTray>(this);
  if (settings_->getValue("sysTray", true).toBool())
    sysTray_->show();

  qmlEngine.rootContext()->setContextProperty("appInfo", &appInfo_);
  qmlEngine.rootContext()->setContextProperty("settings", settings_.get());
  qmlEngine.rootContext()->setContextProperty("systemTray", sysTray_.get());

  uiFactory_->build(qmlEngine, sysSyncer_->sysModel(), *session_);

  mainWindow_ = qobject_cast<QQuickWindow *>(qmlEngine.rootObjects().value(0));
  connect(&singleInstance_, &SingleInstance::newInstance, this,
          &App::showMainWindow);
}
