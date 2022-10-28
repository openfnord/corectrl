// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "fanautoqmlitem.h"

#include "core/qmlcomponentregistry.h"
#include "fanauto.h"
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QString>
#include <QtGlobal>
#include <QtQml>
#include <memory>

char const *const AMD::FanAutoQMLItem::trStrings[] = {
    QT_TRANSLATE_NOOP("ControlModeQMLItem", "AMD_FAN_AUTO"),
};

class AMD::FanAutoQMLItem::Initializer final
: public QMLItem::Initializer
, public IControl::Exporter
{
 public:
  Initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine,
              AMD::FanAutoQMLItem &qmlItem) noexcept
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

 private:
  AMD::FanAutoQMLItem &outer_;
};

void AMD::FanAutoQMLItem::Initializer::takeActive(bool active)
{
  outer_.takeActive(active);
}

AMD::FanAutoQMLItem::FanAutoQMLItem() noexcept
{
  setName(tr(FanAuto::ItemID.data()));
}

void AMD::FanAutoQMLItem::activate(bool active)
{
  takeActive(active);
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::FanAutoQMLItem::provideImporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
AMD::FanAutoQMLItem::provideExporter(Item const &)
{
  return {};
}

bool AMD::FanAutoQMLItem::provideActive() const
{
  return active_;
}

void AMD::FanAutoQMLItem::takeActive(bool active)
{
  active_ = active;
  setVisible(active);
}

std::unique_ptr<Exportable::Exporter>
AMD::FanAutoQMLItem::initializer(IQMLComponentFactory const &qmlComponentFactory,
                                 QQmlApplicationEngine &qmlEngine)
{
  return std::make_unique<AMD::FanAutoQMLItem::Initializer>(qmlComponentFactory,
                                                            qmlEngine, *this);
}

bool AMD::FanAutoQMLItem::register_()
{
  QMLComponentRegistry::addQMLTypeRegisterer([]() {
    qmlRegisterType<AMD::FanAutoQMLItem>("CoreCtrl.UIComponents", 1, 0,
                                         AMD::FanAuto::ItemID.data());
  });

  QMLComponentRegistry::addQMLItemProvider(
      AMD::FanAuto::ItemID, [](QQmlApplicationEngine &engine) {
        QQmlComponent component(&engine,
                                QStringLiteral("qrc:/qml/AMDFanAutoForm.qml"));
        return qobject_cast<QMLItem *>(component.create());
      });

  return true;
}

bool const AMD::FanAutoQMLItem::registered_ = AMD::FanAutoQMLItem::register_();
