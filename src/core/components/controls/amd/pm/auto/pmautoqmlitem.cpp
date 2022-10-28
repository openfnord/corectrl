// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmautoqmlitem.h"

#include "core/qmlcomponentregistry.h"
#include "pmauto.h"
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QString>
#include <QtGlobal>
#include <QtQml>
#include <memory>

char const *const AMD::PMAutoQMLItem::trStrings[] = {
    QT_TRANSLATE_NOOP("ControlModeQMLItem", "AMD_PM_AUTO"),
};

class AMD::PMAutoQMLItem::Initializer final
: public QMLItem::Initializer
, public IControl::Exporter
{
 public:
  Initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine,
              AMD::PMAutoQMLItem &qmlItem) noexcept
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
  AMD::PMAutoQMLItem &outer_;
};

void AMD::PMAutoQMLItem::Initializer::takeActive(bool active)
{
  outer_.takeActive(active);
}

AMD::PMAutoQMLItem::PMAutoQMLItem() noexcept
{
  setName(tr(PMAuto::ItemID.data()));
}

void AMD::PMAutoQMLItem::activate(bool active)
{
  takeActive(active);
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMAutoQMLItem::provideImporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
AMD::PMAutoQMLItem::provideExporter(Item const &)
{
  return {};
}

bool AMD::PMAutoQMLItem::provideActive() const
{
  return active_;
}

void AMD::PMAutoQMLItem::takeActive(bool active)
{
  active_ = active;
  setVisible(active);
}

std::unique_ptr<Exportable::Exporter>
AMD::PMAutoQMLItem::initializer(IQMLComponentFactory const &qmlComponentFactory,
                                QQmlApplicationEngine &qmlEngine)
{
  return std::make_unique<AMD::PMAutoQMLItem::Initializer>(qmlComponentFactory,
                                                           qmlEngine, *this);
}

bool AMD::PMAutoQMLItem::register_()
{
  QMLComponentRegistry::addQMLTypeRegisterer([]() {
    qmlRegisterType<AMD::PMAutoQMLItem>("CoreCtrl.UIComponents", 1, 0,
                                        AMD::PMAuto::ItemID.data());
  });

  QMLComponentRegistry::addQMLItemProvider(
      AMD::PMAuto::ItemID, [](QQmlApplicationEngine &engine) {
        QQmlComponent component(&engine,
                                QStringLiteral("qrc:/qml/AMDPMAutoForm.qml"));
        return qobject_cast<QMLItem *>(component.create());
      });

  return true;
}

bool const AMD::PMAutoQMLItem::registered_ = AMD::PMAutoQMLItem::register_();
