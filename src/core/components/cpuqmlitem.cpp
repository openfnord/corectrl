// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "cpuqmlitem.h"

#include "core/info/icpuinfo.h"
#include "core/iqmlcomponentfactory.h"
#include "core/qmlcomponentregistry.h"
#include "icpu.h"
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

class CPUQMLItem::Initializer final
: public QMLItem::Initializer
, public ICPU::Exporter
{
 public:
  Initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine, CPUQMLItem &qmlItem) noexcept
  : QMLItem::Initializer(qmlComponentFactory, qmlEngine)
  , outer_(qmlItem)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;

  void takeActive(bool active) override;
  void takeInfo(ICPUInfo const &info) override;
  void takeSensor(ISensor const &sensor) override;

 private:
  CPUQMLItem &outer_;
};

std::optional<std::reference_wrapper<Exportable::Exporter>>
CPUQMLItem::Initializer::provideExporter(Item const &i)
{
  return initializer(i.ID(), &outer_).first;
}

void CPUQMLItem::Initializer::takeActive(bool active)
{
  outer_.takeActive(active);
}

void CPUQMLItem::Initializer::takeInfo(ICPUInfo const &info)
{
  auto name(info.info(ICPUInfo::Keys::modelName));
  if (!name.empty())
    name.append("\n");
  name.append("[CPU ").append(std::to_string(info.socketId())).append("]");
  outer_.setName(QString::fromStdString(name));

  outer_.takeSocketId(info.socketId());
}

void CPUQMLItem::Initializer::takeSensor(ISensor const &sensor)
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

void CPUQMLItem::activate(bool active)
{
  takeActive(active);
}

std::optional<std::reference_wrapper<Importable::Importer>>
CPUQMLItem::provideImporter(Item const &i)
{
  auto item = this->findChild<QQuickItem *>(QString::fromStdString(i.ID()));
  if (item == nullptr)
    return {};

  return dynamic_cast<Importable::Importer &>(*item);
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
CPUQMLItem::provideExporter(Item const &i)
{
  auto item = this->findChild<QQuickItem *>(QString::fromStdString(i.ID()));
  if (item == nullptr)
    return {};

  return dynamic_cast<Exportable::Exporter &>(*item);
}

bool CPUQMLItem::provideActive() const
{
  return active_;
}

int CPUQMLItem::provideSocketId() const
{
  return socketId_;
}

void CPUQMLItem::takeActive(bool active)
{
  active_ = active;
  setEnabled(active);
}

void CPUQMLItem::takeSocketId(int id)
{
  socketId_ = id;
}

std::unique_ptr<Exportable::Exporter>
CPUQMLItem::initializer(IQMLComponentFactory const &qmlComponentFactory,
                        QQmlApplicationEngine &qmlEngine)
{
  return std::make_unique<CPUQMLItem::Initializer>(qmlComponentFactory,
                                                   qmlEngine, *this);
}

bool CPUQMLItem::register_()
{
  QMLComponentRegistry::addQMLTypeRegisterer([]() {
    qmlRegisterType<CPUQMLItem>("CoreCtrl.UIComponents", 1, 0,
                                ICPU::ItemID.data());
  });

  QMLComponentRegistry::addQMLItemProvider(
      ICPU::ItemID, [](QQmlApplicationEngine &engine) {
        QQmlComponent component(&engine,
                                QStringLiteral("qrc:/qml/CPUForm.qml"));
        return qobject_cast<QMLItem *>(component.create());
      });

  return true;
}

bool const CPUQMLItem::registered_ = CPUQMLItem::register_();
