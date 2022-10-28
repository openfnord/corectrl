// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "noopqmlitem.h"

#include "core/qmlcomponentregistry.h"
#include "noop.h"
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QString>
#include <QtGlobal>
#include <QtQml>
#include <memory>

char const *const NoopQMLItem::trStrings[] = {
    QT_TRANSLATE_NOOP("ControlModeQMLItem", "NOOP"),
};

class NoopQMLItem::Initializer final
: public QMLItem::Initializer
, public IControl::Exporter
{
 public:
  Initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine, NoopQMLItem &qmlItem) noexcept
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
  NoopQMLItem &outer_;
};

void NoopQMLItem::Initializer::takeActive(bool active)
{
  outer_.takeActive(active);
}

NoopQMLItem::NoopQMLItem() noexcept
{
  setName(tr(Noop::ItemID.data()));
}

void NoopQMLItem::activate(bool active)
{
  takeActive(active);
}

std::optional<std::reference_wrapper<Importable::Importer>>
NoopQMLItem::provideImporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
NoopQMLItem::provideExporter(Item const &)
{
  return {};
}

bool NoopQMLItem::provideActive() const
{
  return active_;
}

void NoopQMLItem::takeActive(bool active)
{
  active_ = active;
  setVisible(active);
}

std::unique_ptr<Exportable::Exporter>
NoopQMLItem::initializer(IQMLComponentFactory const &qmlComponentFactory,
                         QQmlApplicationEngine &qmlEngine)
{
  return std::make_unique<NoopQMLItem::Initializer>(qmlComponentFactory,
                                                    qmlEngine, *this);
}

bool NoopQMLItem::register_()
{
  QMLComponentRegistry::addQMLTypeRegisterer([]() {
    qmlRegisterType<NoopQMLItem>("CoreCtrl.UIComponents", 1, 0,
                                 Noop::ItemID.data());
  });

  QMLComponentRegistry::addQMLItemProvider(
      Noop::ItemID, [](QQmlApplicationEngine &engine) {
        QQmlComponent component(&engine,
                                QStringLiteral("qrc:/qml/NoopForm.qml"));
        return qobject_cast<QMLItem *>(component.create());
      });

  return true;
}

bool const NoopQMLItem::registered_ = NoopQMLItem::register_();
