// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmdynamicfreqqmlitem.h"

#include "core/qmlcomponentregistry.h"
#include "pmdynamicfreq.h"
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QString>
#include <QtGlobal>
#include <QtQml>
#include <memory>

char const *const AMD::PMDynamicFreqQMLItem::trStrings[] = {
    QT_TRANSLATE_NOOP("ControlModeQMLItem", "AMD_PM_DYNAMIC_FREQ"),
};

class AMD::PMDynamicFreqQMLItem::Initializer final
: public QMLItem::Initializer
, public IControl::Exporter
{
 public:
  Initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine,
              AMD::PMDynamicFreqQMLItem &qmlItem) noexcept
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
  AMD::PMDynamicFreqQMLItem &outer_;
};

void AMD::PMDynamicFreqQMLItem::Initializer::takeActive(bool active)
{
  outer_.takeActive(active);
}

AMD::PMDynamicFreqQMLItem::PMDynamicFreqQMLItem() noexcept
{
  setName(tr(PMDynamicFreq::ItemID.data()));
}

void AMD::PMDynamicFreqQMLItem::activate(bool active)
{
  takeActive(active);
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMDynamicFreqQMLItem::provideImporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
AMD::PMDynamicFreqQMLItem::provideExporter(Item const &)
{
  return {};
}

bool AMD::PMDynamicFreqQMLItem::provideActive() const
{
  return active_;
}

void AMD::PMDynamicFreqQMLItem::takeActive(bool active)
{
  active_ = active;
  setVisible(active);
}

std::unique_ptr<Exportable::Exporter> AMD::PMDynamicFreqQMLItem::initializer(
    IQMLComponentFactory const &qmlComponentFactory,
    QQmlApplicationEngine &qmlEngine)
{
  return std::make_unique<AMD::PMDynamicFreqQMLItem::Initializer>(
      qmlComponentFactory, qmlEngine, *this);
}

bool AMD::PMDynamicFreqQMLItem::register_()
{
  QMLComponentRegistry::addQMLTypeRegisterer([]() {
    qmlRegisterType<AMD::PMDynamicFreqQMLItem>(
        "CoreCtrl.UIComponents", 1, 0, AMD::PMDynamicFreq::ItemID.data());
  });

  QMLComponentRegistry::addQMLItemProvider(
      AMD::PMDynamicFreq::ItemID, [](QQmlApplicationEngine &engine) {
        QQmlComponent component(
            &engine, QStringLiteral("qrc:/qml/AMDPMDynamicFreqForm.qml"));
        return qobject_cast<QMLItem *>(component.create());
      });

  return true;
}

bool const AMD::PMDynamicFreqQMLItem::registered_ =
    AMD::PMDynamicFreqQMLItem::register_();
