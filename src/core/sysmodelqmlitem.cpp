// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "sysmodelqmlitem.h"

#include "components/igpu.h"
#include "components/igpuprofilepart.h"
#include "info/igpuinfo.h"
#include "iprofilepart.h"
#include "isyscomponent.h"
#include "isyscomponentprofilepart.h"
#include "isysmodel.h"
#include "item.h"
#include "qmlcomponentregistry.h"
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QString>
#include <QtQml>
#include <utility>

class SysModelQMLItem::Initializer final
: public QMLItem::Initializer
, public ISysModel::Exporter
{
 public:
  Initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine, SysModelQMLItem &qmlItem) noexcept
  : QMLItem::Initializer(qmlComponentFactory, qmlEngine)
  , outer_(qmlItem)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;

 private:
  SysModelQMLItem &outer_;
};

std::optional<std::reference_wrapper<Exportable::Exporter>>
SysModelQMLItem::Initializer::provideExporter(Item const &i)
{
  auto &itemID = i.ID();
  if (itemID == ISysModel::ItemID)
    return *this;
  else {
    auto [componentInitializer, qmlItemPtr] = initializer(itemID, &outer_);
    if (componentInitializer.has_value()) {
      auto &key = dynamic_cast<ISysComponent const &>(i).key();
      outer_.components_.emplace(key, qmlItemPtr);
      return componentInitializer;
    }
  }

  return {};
}

void SysModelQMLItem::activate(bool)
{
}

std::optional<std::reference_wrapper<Importable::Importer>>
SysModelQMLItem::provideImporter(Item const &i)
{
  if (i.ID() == IProfile::ItemID)
    return *this;

  auto &key = dynamic_cast<ISysComponentProfilePart const &>(i).key();
  auto const iter = components_.find(key);
  if (iter != components_.cend())
    return dynamic_cast<Importable::Importer &>(*iter->second);

  return {};
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
SysModelQMLItem::provideExporter(Item const &i)
{
  if (i.ID() == IProfile::ItemID)
    return *this;

  auto &key = dynamic_cast<ISysComponentProfilePart const &>(i).key();
  auto const iter = components_.find(key);
  if (iter != components_.cend())
    return dynamic_cast<Exportable::Exporter &>(*iter->second);

  return {};
}

bool SysModelQMLItem::provideActive() const
{
  return profileActive_;
}

IProfile::Info const &SysModelQMLItem::provideInfo() const
{
  return profileInfo_;
}

void SysModelQMLItem::takeActive(bool active)
{
  profileActive_ = active;
}

void SysModelQMLItem::takeInfo(IProfile::Info const &info)
{
  profileInfo_ = info;
}

std::unique_ptr<Exportable::Exporter>
SysModelQMLItem::initializer(IQMLComponentFactory const &qmlComponentFactory,
                             QQmlApplicationEngine &qmlEngine)
{
  return std::make_unique<SysModelQMLItem::Initializer>(qmlComponentFactory,
                                                        qmlEngine, *this);
}

bool SysModelQMLItem::register_()
{
  QMLComponentRegistry::addQMLTypeRegisterer([]() {
    qmlRegisterType<SysModelQMLItem>("CoreCtrl.UIComponents", 1, 0,
                                     ISysModel::ItemID.data());
  });

  QMLComponentRegistry::addQMLItemProvider(
      ISysModel::ItemID, [](QQmlApplicationEngine &engine) {
        QQmlComponent component(&engine,
                                QStringLiteral("qrc:/qml/SysModelForm.qml"));
        return qobject_cast<QMLItem *>(component.create());
      });

  return true;
}

bool const SysModelQMLItem::registered_ = SysModelQMLItem::register_();
