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
    qmlRegisterType<SysModelQMLItem>("Radman.UIComponents", 1, 0,
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
