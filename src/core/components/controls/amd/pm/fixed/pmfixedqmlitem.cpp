// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmfixedqmlitem.h"

#include "core/qmlcomponentregistry.h"
#include "pmfixed.h"
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QString>
#include <QtGlobal>
#include <QtQml>
#include <memory>

char const *const AMD::PMFixedQMLItem::trStrings[] = {
    QT_TRANSLATE_NOOP("ControlModeQMLItem", "AMD_PM_FIXED"),
    QT_TRANSLATE_NOOP("AMD::PMFixedQMLItem", "low"),
    QT_TRANSLATE_NOOP("AMD::PMFixedQMLItem", "mid"),
    QT_TRANSLATE_NOOP("AMD::PMFixedQMLItem", "high"),
};

class AMD::PMFixedQMLItem::Initializer final
: public QMLItem::Initializer
, public AMD::PMFixed::Exporter
{
 public:
  Initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine,
              AMD::PMFixedQMLItem &qmlItem) noexcept
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

  void takePMFixedMode(std::string const &mode) override;
  void takePMFixedModes(std::vector<std::string> const &modes) override;

 private:
  AMD::PMFixedQMLItem &outer_;
};

void AMD::PMFixedQMLItem::Initializer::takeActive(bool active)
{
  outer_.takeActive(active);
}

void AMD::PMFixedQMLItem::Initializer::takePMFixedMode(std::string const &mode)
{
  outer_.takePMFixedMode(mode);
}

void AMD::PMFixedQMLItem::Initializer::takePMFixedModes(
    std::vector<std::string> const &modes)
{
  outer_.takePMFixedModes(modes);
}

AMD::PMFixedQMLItem::PMFixedQMLItem() noexcept
{
  setName(tr(AMD::PMFixed::ItemID.data()));
}

void AMD::PMFixedQMLItem::changeMode(QString const &mode)
{
  auto newMode = mode.toStdString();
  if (mode_ != newMode) {
    std::swap(mode_, newMode);
    emit modeChanged(mode);
    emit settingsChanged();
  }
}

void AMD::PMFixedQMLItem::activate(bool active)
{
  takeActive(active);
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMFixedQMLItem::provideImporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
AMD::PMFixedQMLItem::provideExporter(Item const &)
{
  return {};
}

bool AMD::PMFixedQMLItem::provideActive() const
{
  return active_;
}

std::string const &AMD::PMFixedQMLItem::providePMFixedMode() const
{
  return mode_;
}

void AMD::PMFixedQMLItem::takeActive(bool active)
{
  active_ = active;
  setVisible(active);
}

void AMD::PMFixedQMLItem::takePMFixedMode(std::string const &mode)
{
  if (mode_ != mode) {
    mode_ = mode;
    emit modeChanged(QString::fromStdString(mode_));
  }
}

std::unique_ptr<Exportable::Exporter>
AMD::PMFixedQMLItem::initializer(IQMLComponentFactory const &qmlComponentFactory,
                                 QQmlApplicationEngine &qmlEngine)
{
  return std::make_unique<AMD::PMFixedQMLItem::Initializer>(qmlComponentFactory,
                                                            qmlEngine, *this);
}

void AMD::PMFixedQMLItem::takePMFixedModes(std::vector<std::string> const &modes)
{
  QList<QString> modeTextVector;
  for (auto mode : modes) {
    modeTextVector.push_back(QString::fromStdString(mode));
    modeTextVector.push_back(tr(mode.data()));
  }
  emit modesChanged(modeTextVector);
}

bool AMD::PMFixedQMLItem::register_()
{
  QMLComponentRegistry::addQMLTypeRegisterer([]() {
    qmlRegisterType<AMD::PMFixedQMLItem>("CoreCtrl.UIComponents", 1, 0,
                                         AMD::PMFixed::ItemID.data());
  });

  QMLComponentRegistry::addQMLItemProvider(
      AMD::PMFixed::ItemID, [](QQmlApplicationEngine &engine) {
        QQmlComponent component(&engine,
                                QStringLiteral("qrc:/qml/AMDPMFixedForm.qml"));
        return qobject_cast<QMLItem *>(component.create());
      });

  return true;
}

bool const AMD::PMFixedQMLItem::registered_ = AMD::PMFixedQMLItem::register_();
