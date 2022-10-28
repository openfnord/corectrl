// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "controlgroupqmlitem.h"

#include "controlgroup.h"
#include "icontrol.h"
#include <QQuickItem>
#include <memory>
#include <utility>

class ControlGroupQMLItem::Initializer final
: public QMLItem::Initializer
, public ControlGroup::Exporter
{
 public:
  Initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine,
              ControlGroupQMLItem &qmlItem) noexcept
  : QMLItem::Initializer(qmlComponentFactory, qmlEngine)
  , outer_(qmlItem)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;

  void takeActive(bool active) override;

 private:
  ControlGroupQMLItem &outer_;
};

std::optional<std::reference_wrapper<Exportable::Exporter>>
ControlGroupQMLItem::Initializer::provideExporter(Item const &i)
{
  return initializer(i.ID(), &outer_).first;
}

void ControlGroupQMLItem::Initializer::takeActive(bool active)
{
  outer_.takeActive(active);
}

ControlGroupQMLItem::ControlGroupQMLItem(std::string_view id) noexcept
{
  setName(tr(id.data()));
}

void ControlGroupQMLItem::activate(bool active)
{
  takeActive(active);
}

std::optional<std::reference_wrapper<Importable::Importer>>
ControlGroupQMLItem::provideImporter(Item const &i)
{
  auto item = findQQuickItem(i);
  if (item == nullptr)
    return {};

  return dynamic_cast<Importable::Importer &>(*item);
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
ControlGroupQMLItem::provideExporter(Item const &i)
{
  auto item = findQQuickItem(i);
  if (item == nullptr)
    return {};

  return dynamic_cast<Exportable::Exporter &>(*item);
}

bool ControlGroupQMLItem::provideActive() const
{
  return active_;
}

void ControlGroupQMLItem::takeActive(bool active)
{
  active_ = active;
  setVisible(active);
}

QQuickItem *ControlGroupQMLItem::findQQuickItem(Item const &i) const
{
  static std::string const instanceIDPropertyName{"instanceID"};

  if (i.ID() != i.instanceID()) {
    // The control has multiple instances
    auto children =
        this->findChildren<QQuickItem *>(QString::fromStdString(i.ID()));
    if (!children.empty()) {

      auto instanceID = QString::fromStdString(i.instanceID());
      for (auto child : children) {

        auto instanceIDProperty = child->property(instanceIDPropertyName.c_str());
        if (instanceIDProperty.isValid() &&
            instanceIDProperty.toString() == instanceID)
          return child;
      }
    }
  }
  else {
    // The control has a single instance
    auto item = this->findChild<QQuickItem *>(QString::fromStdString(i.ID()));
    if (item != nullptr)
      return item;
  }

  return nullptr;
}

std::unique_ptr<Exportable::Exporter>
ControlGroupQMLItem::initializer(IQMLComponentFactory const &qmlComponentFactory,
                                 QQmlApplicationEngine &qmlEngine)
{
  return std::make_unique<ControlGroupQMLItem::Initializer>(qmlComponentFactory,
                                                            qmlEngine, *this);
}
