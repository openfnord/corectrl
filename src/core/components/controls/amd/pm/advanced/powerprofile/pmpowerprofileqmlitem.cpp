// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmpowerprofileqmlitem.h"

#include "core/qmlcomponentregistry.h"
#include "pmpowerprofile.h"
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QString>
#include <QtGlobal>
#include <QtQml>
#include <memory>

char const *const AMD::PMPowerProfileQMLItem::trStrings[] = {
    QT_TRANSLATE_NOOP("AMD::PMPowerProfileQMLItem", "3D_FULL_SCREEN"),
    QT_TRANSLATE_NOOP("AMD::PMPowerProfileQMLItem", "POWER_SAVING"),
    QT_TRANSLATE_NOOP("AMD::PMPowerProfileQMLItem", "VIDEO"),
    QT_TRANSLATE_NOOP("AMD::PMPowerProfileQMLItem", "VR"),
    QT_TRANSLATE_NOOP("AMD::PMPowerProfileQMLItem", "COMPUTE"),
};

class AMD::PMPowerProfileQMLItem::Initializer final
: public QMLItem::Initializer
, public AMD::PMPowerProfile::Exporter
{
 public:
  Initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine,
              AMD::PMPowerProfileQMLItem &qmlItem) noexcept
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
  void takePMPowerProfileMode(std::string const &mode) override;
  void takePMPowerProfileModes(std::vector<std::string> const &modes) override;

 private:
  AMD::PMPowerProfileQMLItem &outer_;
};

void AMD::PMPowerProfileQMLItem::Initializer::takeActive(bool active)
{
  outer_.takeActive(active);
}

void AMD::PMPowerProfileQMLItem::Initializer::takePMPowerProfileMode(
    std::string const &mode)
{
  outer_.takePMPowerProfileMode(mode);
}

void AMD::PMPowerProfileQMLItem::Initializer::takePMPowerProfileModes(
    std::vector<std::string> const &modes)
{
  outer_.takePMPowerProfileModes(modes);
}

AMD::PMPowerProfileQMLItem::PMPowerProfileQMLItem() noexcept
{
  setName(tr(AMD::PMPowerProfile::ItemID.data()));
}

void AMD::PMPowerProfileQMLItem::changeMode(QString const &mode)
{
  auto newMode = mode.toStdString();
  if (mode_ != newMode) {
    std::swap(mode_, newMode);
    emit modeChanged(mode);
    emit settingsChanged();
  }
}

void AMD::PMPowerProfileQMLItem::activate(bool active)
{
  takeActive(active);
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMPowerProfileQMLItem::provideImporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
AMD::PMPowerProfileQMLItem::provideExporter(Item const &)
{
  return {};
}

bool AMD::PMPowerProfileQMLItem::provideActive() const
{
  return active_;
}

void AMD::PMPowerProfileQMLItem::takeActive(bool active)
{
  active_ = active;
  setVisible(active);
}

std::string const &AMD::PMPowerProfileQMLItem::providePMPowerProfileMode() const
{
  return mode_;
}

void AMD::PMPowerProfileQMLItem::takePMPowerProfileMode(std::string const &mode)
{
  if (mode_ != mode) {
    mode_ = mode;
    emit modeChanged(QString::fromStdString(mode));
  }
}

void AMD::PMPowerProfileQMLItem::takePMPowerProfileModes(
    std::vector<std::string> const &modes)
{
  QList<QString> modeText;
  for (auto &mode : modes) {
    modeText.push_back(QString::fromStdString(mode));
    modeText.push_back(tr(mode.data()));
  }

  emit modesChanged(modeText);
}

std::unique_ptr<Exportable::Exporter> AMD::PMPowerProfileQMLItem::initializer(
    IQMLComponentFactory const &qmlComponentFactory,
    QQmlApplicationEngine &qmlEngine)
{
  return std::make_unique<AMD::PMPowerProfileQMLItem::Initializer>(
      qmlComponentFactory, qmlEngine, *this);
}

bool AMD::PMPowerProfileQMLItem::register_()
{
  QMLComponentRegistry::addQMLTypeRegisterer([]() {
    qmlRegisterType<AMD::PMPowerProfileQMLItem>(
        "CoreCtrl.UIComponents", 1, 0, AMD::PMPowerProfile::ItemID.data());
  });

  QMLComponentRegistry::addQMLItemProvider(
      AMD::PMPowerProfile::ItemID, [](QQmlApplicationEngine &engine) {
        QQmlComponent component(
            &engine, QStringLiteral("qrc:/qml/AMDPMPowerProfileForm.qml"));
        return qobject_cast<QMLItem *>(component.create());
      });

  return true;
}

bool const AMD::PMPowerProfileQMLItem::registered_ =
    AMD::PMPowerProfileQMLItem::register_();
