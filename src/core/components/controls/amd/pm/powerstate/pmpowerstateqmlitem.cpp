// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmpowerstateqmlitem.h"

#include "core/qmlcomponentregistry.h"
#include "pmpowerstate.h"
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QString>
#include <QtGlobal>
#include <QtQml>
#include <memory>

char const *const AMD::PMPowerStateQMLItem::trStrings[] = {
    QT_TRANSLATE_NOOP("ControlModeQMLItem", "AMD_PM_POWERSTATE"),
    QT_TRANSLATE_NOOP("AMD::PMPowerStateQMLItem", "battery"),
    QT_TRANSLATE_NOOP("AMD::PMPowerStateQMLItem", "balanced"),
    QT_TRANSLATE_NOOP("AMD::PMPowerStateQMLItem", "performance"),
};

class AMD::PMPowerStateQMLItem::Initializer final
: public QMLItem::Initializer
, public AMD::PMPowerState::Exporter
{
 public:
  Initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine,
              AMD::PMPowerStateQMLItem &qmlItem) noexcept
  : QMLItem::Initializer(qmlComponentFactory, qmlEngine)
  , outer_(qmlItem)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takeActive(bool active) override;

  void takePMPowerStateMode(std::string const &mode) override;
  void takePMPowerStateModes(std::vector<std::string> const &modes) override;

 private:
  AMD::PMPowerStateQMLItem &outer_;
};

void AMD::PMPowerStateQMLItem::Initializer::takeActive(bool active)
{
  outer_.takeActive(active);
}

void AMD::PMPowerStateQMLItem::Initializer::takePMPowerStateMode(
    std::string const &mode)
{
  outer_.takePMPowerStateMode(mode);
}

void AMD::PMPowerStateQMLItem::Initializer::takePMPowerStateModes(
    std::vector<std::string> const &modes)
{
  outer_.takePMPowerStateModes(modes);
}

AMD::PMPowerStateQMLItem::PMPowerStateQMLItem() noexcept
{
  setName(tr(AMD::PMPowerState::ItemID.data()));
}

void AMD::PMPowerStateQMLItem::changeMode(QString const &mode)
{
  auto newMode = mode.toStdString();
  if (mode_ != newMode) {
    std::swap(mode_, newMode);
    emit modeChanged(mode);
    emit settingsChanged();
  }
}

void AMD::PMPowerStateQMLItem::activate(bool active)
{
  takeActive(active);
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMPowerStateQMLItem::provideImporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
AMD::PMPowerStateQMLItem::provideExporter(Item const &)
{
  return {};
}

bool AMD::PMPowerStateQMLItem::provideActive() const
{
  return active_;
}

std::string const &AMD::PMPowerStateQMLItem::providePMPowerStateMode() const
{
  return mode_;
}

void AMD::PMPowerStateQMLItem::takeActive(bool active)
{
  active_ = active;
  setVisible(active);
}

void AMD::PMPowerStateQMLItem::takePMPowerStateMode(std::string const &mode)
{
  if (mode_ != mode) {
    mode_ = mode;
    emit modeChanged(QString::fromStdString(mode_));
  }
}

std::unique_ptr<Exportable::Exporter> AMD::PMPowerStateQMLItem::initializer(
    IQMLComponentFactory const &qmlComponentFactory,
    QQmlApplicationEngine &qmlEngine)
{
  return std::make_unique<AMD::PMPowerStateQMLItem::Initializer>(
      qmlComponentFactory, qmlEngine, *this);
}

void AMD::PMPowerStateQMLItem::takePMPowerStateModes(
    std::vector<std::string> const &modes)
{
  QList<QString> modeTextVector;
  for (auto mode : modes) {
    modeTextVector.push_back(QString::fromStdString(mode));
    modeTextVector.push_back(tr(mode.data()));
  }
  emit modesChanged(modeTextVector);
}

bool AMD::PMPowerStateQMLItem::register_()
{
  QMLComponentRegistry::addQMLTypeRegisterer([]() {
    qmlRegisterType<AMD::PMPowerStateQMLItem>("CoreCtrl.UIComponents", 1, 0,
                                              AMD::PMPowerState::ItemID.data());
  });

  QMLComponentRegistry::addQMLItemProvider(
      AMD::PMPowerState::ItemID, [](QQmlApplicationEngine &engine) {
        QQmlComponent component(
            &engine, QStringLiteral("qrc:/qml/AMDPMPowerStateForm.qml"));
        return qobject_cast<QMLItem *>(component.create());
      });

  return true;
}

bool const AMD::PMPowerStateQMLItem::registered_ =
    AMD::PMPowerStateQMLItem::register_();
