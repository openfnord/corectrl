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
#include "gpuqmlitem.h"

#include "core/info/igpuinfo.h"
#include "core/iqmlcomponentfactory.h"
#include "core/qmlcomponentregistry.h"
#include "igpu.h"
#include "sensors/graphitem.h"
#include "sensors/isensor.h"
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickItem>
#include <QString>
#include <Qt>
#include <memory>
#include <utility>

class GPUQMLItem::Initializer final
: public QMLItem::Initializer
, public IGPU::Exporter
{
 public:
  Initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine, GPUQMLItem &qmlItem) noexcept
  : QMLItem::Initializer(qmlComponentFactory, qmlEngine)
  , outer_(qmlItem)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;

  void takeActive(bool active) override;
  void takeInfo(IGPUInfo const &info) override;
  void takeSensor(ISensor const &sensor) override;

 private:
  GPUQMLItem &outer_;
};

std::optional<std::reference_wrapper<Exportable::Exporter>>
GPUQMLItem::Initializer::provideExporter(Item const &i)
{
  return initializer(i.ID(), &outer_).first;
}

void GPUQMLItem::Initializer::takeActive(bool active)
{
  outer_.takeActive(active);
}

void GPUQMLItem::Initializer::takeInfo(IGPUInfo const &info)
{
  auto name(info.info(IGPUInfo::Keys::subdeviceName));
  if (!name.empty())
    name.append("\n");
  name.append("[GPU ").append(std::to_string(info.index())).append("]");
  outer_.setName(QString::fromStdString(name));

  outer_.takeIndex(info.index());
}

void GPUQMLItem::Initializer::takeSensor(ISensor const &sensor)
{
  auto graphItem = dynamic_cast<GraphItem *>(
      qmlComponentFactory_.createQuickItem(sensor.ID(), &outer_, "SensorGraph"));
  if (graphItem != nullptr) {
    connect(graphItem, &GraphItem::settingsChanged, &outer_,
            &QMLItem::settingsChanged, Qt::UniqueConnection);
    graphItem->init(&sensor);
    emit outer_.newGraphItem(graphItem);
  }
}

void GPUQMLItem::activate(bool active)
{
  takeActive(active);
}

std::optional<std::reference_wrapper<Importable::Importer>>
GPUQMLItem::provideImporter(Item const &i)
{
  auto item = this->findChild<QQuickItem *>(QString::fromStdString(i.ID()));
  if (item == nullptr)
    return {};

  return dynamic_cast<Importable::Importer &>(*item);
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
GPUQMLItem::provideExporter(Item const &i)
{
  auto item = this->findChild<QQuickItem *>(QString::fromStdString(i.ID()));
  if (item == nullptr)
    return {};

  return dynamic_cast<Exportable::Exporter &>(*item);
}

bool GPUQMLItem::provideActive() const
{
  return active_;
}

int GPUQMLItem::provideIndex() const
{
  return index_;
}

std::string const &GPUQMLItem::provideDeviceID() const
{
  return deviceID_;
}

std::string const &GPUQMLItem::provideRevision() const
{
  return revision_;
}

void GPUQMLItem::takeActive(bool active)
{
  active_ = active;
  setEnabled(active);
}

void GPUQMLItem::takeIndex(int index)
{
  index_ = index;
}

void GPUQMLItem::takeDeviceID(std::string const &deviceID)
{
  deviceID_ = deviceID;
}

void GPUQMLItem::takeRevision(std::string const &revision)
{
  revision_ = revision;
}

std::unique_ptr<Exportable::Exporter>
GPUQMLItem::initializer(IQMLComponentFactory const &qmlComponentFactory,
                        QQmlApplicationEngine &qmlEngine)
{
  return std::make_unique<GPUQMLItem::Initializer>(qmlComponentFactory,
                                                   qmlEngine, *this);
}

bool GPUQMLItem::register_()
{
  QMLComponentRegistry::addQMLTypeRegisterer([]() {
    qmlRegisterType<GPUQMLItem>("Radman.UIComponents", 1, 0, IGPU::ItemID.data());
  });

  QMLComponentRegistry::addQMLItemProvider(
      IGPU::ItemID, [](QQmlApplicationEngine &engine) {
        QQmlComponent component(&engine,
                                QStringLiteral("qrc:/qml/GPUForm.qml"));
        return qobject_cast<QMLItem *>(component.create());
      });

  return true;
}

bool const GPUQMLItem::registered_ = GPUQMLItem::register_();
