// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "fanfixedqmlitem.h"

#include "core/qmlcomponentregistry.h"
#include "fanfixed.h"
#include <QQmlApplicationEngine>
#include <QString>
#include <memory>

char const *const AMD::FanFixedQMLItem::trStrings[] = {
    QT_TRANSLATE_NOOP("ControlModeQMLItem", "AMD_FAN_FIXED"),
};

class AMD::FanFixedQMLItem::Initializer final
: public QMLItem::Initializer
, public AMD::FanFixed::Exporter
{
 public:
  Initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine,
              AMD::FanFixedQMLItem &qmlItem) noexcept
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
  void takeFanFixedValue(units::concentration::percent_t value) override;
  void takeFanFixedFanStop(bool enabled) override;
  void takeFanFixedFanStartValue(units::concentration::percent_t value) override;

 private:
  AMD::FanFixedQMLItem &outer_;
};

void AMD::FanFixedQMLItem::Initializer::takeActive(bool active)
{
  outer_.takeActive(active);
}

void AMD::FanFixedQMLItem::Initializer::takeFanFixedValue(
    units::concentration::percent_t value)
{
  outer_.takeFanFixedValue(value);
}

void AMD::FanFixedQMLItem::Initializer::takeFanFixedFanStop(bool enabled)
{
  outer_.takeFanFixedFanStop(enabled);
}

void AMD::FanFixedQMLItem::Initializer::takeFanFixedFanStartValue(
    units::concentration::percent_t value)
{
  outer_.takeFanFixedFanStartValue(value);
}

AMD::FanFixedQMLItem::FanFixedQMLItem() noexcept
{
  setName(tr(AMD::FanFixed::ItemID.data()));
}

void AMD::FanFixedQMLItem::changeValue(int value)
{
  if (value_ != value) {
    value_ = value;
    emit valueChanged(value_);
    emit settingsChanged();
  }
}

void AMD::FanFixedQMLItem::enableFanStop(bool enabled)
{
  if (fanStop_ != enabled) {
    fanStop_ = enabled;
    emit fanStopChanged(fanStop_);
    emit settingsChanged();
  }
}

void AMD::FanFixedQMLItem::changeFanStartValue(int value)
{
  if (fanStartValue_ != value) {
    fanStartValue_ = value;
    emit fanStartValueChanged(fanStartValue_);
    emit settingsChanged();
  }
}

void AMD::FanFixedQMLItem::activate(bool active)
{
  takeActive(active);
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::FanFixedQMLItem::provideImporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
AMD::FanFixedQMLItem::provideExporter(Item const &)
{
  return {};
}

bool AMD::FanFixedQMLItem::provideActive() const
{
  return active_;
}

units::concentration::percent_t AMD::FanFixedQMLItem::provideFanFixedValue() const
{
  return value_;
}

bool AMD::FanFixedQMLItem::provideFanFixedFanStop() const
{
  return fanStop_;
}

units::concentration::percent_t
AMD::FanFixedQMLItem::provideFanFixedFanStartValue() const
{
  return fanStartValue_;
}

void AMD::FanFixedQMLItem::takeActive(bool active)
{
  active_ = active;
  setVisible(active);
}

void AMD::FanFixedQMLItem::takeFanFixedValue(units::concentration::percent_t value)
{
  auto newValue = static_cast<int>(value * 100);
  if (value_ != newValue) {
    value_ = newValue;
    emit valueChanged(value_);
  }
}

void AMD::FanFixedQMLItem::takeFanFixedFanStop(bool active)
{
  if (fanStop_ != active) {
    fanStop_ = active;
    emit fanStopChanged(fanStop_);
  }
}

void AMD::FanFixedQMLItem::takeFanFixedFanStartValue(
    units::concentration::percent_t value)
{
  auto newValue = static_cast<int>(value * 100);
  if (fanStartValue_ != newValue) {
    fanStartValue_ = newValue;
    emit fanStartValueChanged(fanStartValue_);
  }
}

std::unique_ptr<Exportable::Exporter>
AMD::FanFixedQMLItem::initializer(IQMLComponentFactory const &qmlComponentFactory,
                                  QQmlApplicationEngine &qmlEngine)
{
  return std::make_unique<AMD::FanFixedQMLItem::Initializer>(
      qmlComponentFactory, qmlEngine, *this);
}

bool AMD::FanFixedQMLItem::register_()
{
  QMLComponentRegistry::addQMLTypeRegisterer([]() {
    qmlRegisterType<AMD::FanFixedQMLItem>("CoreCtrl.UIComponents", 1, 0,
                                          AMD::FanFixed::ItemID.data());
  });

  QMLComponentRegistry::addQMLItemProvider(
      AMD::FanFixed::ItemID, [](QQmlApplicationEngine &engine) {
        QQmlComponent component(&engine,
                                QStringLiteral("qrc:/qml/AMDFanFixedForm.qml"));
        return qobject_cast<QMLItem *>(component.create());
      });

  return true;
}

bool const AMD::FanFixedQMLItem::registered_ = AMD::FanFixedQMLItem::register_();
