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
    qmlRegisterType<AMD::PMAutoQMLItem>("Radman.UIComponents", 1, 0,
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
