// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmfreqodqmlitem.h"

#include "core/qmlcomponentregistry.h"
#include "pmfreqod.h"
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QString>
#include <QtQml>
#include <memory>
#include <string>

class AMD::PMFreqOdQMLItem::Initializer final
: public QMLItem::Initializer
, public AMD::PMFreqOd::Exporter
{
 public:
  Initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine,
              AMD::PMFreqOdQMLItem &qmlItem) noexcept
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
  void takePMFreqOdBaseSclk(units::frequency::megahertz_t value) override;
  void takePMFreqOdBaseMclk(units::frequency::megahertz_t value) override;
  void takePMFreqOdSclkOd(unsigned int value) override;
  void takePMFreqOdMclkOd(unsigned int value) override;

 private:
  AMD::PMFreqOdQMLItem &outer_;
};

void AMD::PMFreqOdQMLItem::Initializer::takeActive(bool active)
{
  outer_.takeActive(active);
}

void AMD::PMFreqOdQMLItem::Initializer::takePMFreqOdBaseSclk(
    units::frequency::megahertz_t value)
{
  outer_.takePMFreqOdBaseSclk(value);
}

void AMD::PMFreqOdQMLItem::Initializer::takePMFreqOdBaseMclk(
    units::frequency::megahertz_t value)
{
  outer_.takePMFreqOdBaseMclk(value);
}

void AMD::PMFreqOdQMLItem::Initializer::takePMFreqOdSclkOd(unsigned int value)
{
  outer_.takePMFreqOdSclkOd(value);
}

void AMD::PMFreqOdQMLItem::Initializer::takePMFreqOdMclkOd(unsigned int value)
{
  outer_.takePMFreqOdMclkOd(value);
}

AMD::PMFreqOdQMLItem::PMFreqOdQMLItem() noexcept
{
  setName(tr(AMD::PMFreqOd::ItemID.data()));
}

void AMD::PMFreqOdQMLItem::changeSclkOd(unsigned int value)
{
  if (sclkOd() != value) {
    sclkOd(value);
    emit settingsChanged();
  }
}

void AMD::PMFreqOdQMLItem::changeMclkOd(unsigned int value)
{
  if (mclkOd() != value) {
    mclkOd(value);
    emit settingsChanged();
  }
}

void AMD::PMFreqOdQMLItem::activate(bool active)
{
  takeActive(active);
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMFreqOdQMLItem::provideImporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
AMD::PMFreqOdQMLItem::provideExporter(Item const &)
{
  return {};
}

bool AMD::PMFreqOdQMLItem::provideActive() const
{
  return active_;
}

void AMD::PMFreqOdQMLItem::takeActive(bool active)
{
  active_ = active;
  setVisible(active);
}

unsigned int AMD::PMFreqOdQMLItem::providePMFreqOdSclkOd() const
{
  return sclkOd();
}

unsigned int AMD::PMFreqOdQMLItem::providePMFreqOdMclkOd() const
{
  return mclkOd();
}

void AMD::PMFreqOdQMLItem::takePMFreqOdSclkOd(unsigned int value)
{
  if (sclkOd() != value)
    sclkOd(value);
}

void AMD::PMFreqOdQMLItem::takePMFreqOdMclkOd(unsigned int value)
{
  if (mclkOd() != value)
    mclkOd(value);
}

void AMD::PMFreqOdQMLItem::takePMFreqOdBaseSclk(units::frequency::megahertz_t value)
{
  baseSclk_ = value.to<unsigned int>();
  auto scaledValue = sclkOd() > 0 ? static_cast<unsigned int>(std::round(
                                        baseSclk_ * (sclkOd() / 100.0f)))
                                  : baseSclk_;
  emit sclkChanged(stateLabel(scaledValue));
}

void AMD::PMFreqOdQMLItem::takePMFreqOdBaseMclk(units::frequency::megahertz_t value)
{
  baseMclk_ = value.to<unsigned int>();
  auto scaledValue = mclkOd() > 0 ? static_cast<unsigned int>(std::round(
                                        baseMclk_ * (mclkOd() / 100.0f)))
                                  : baseMclk_;
  emit mclkChanged(stateLabel(scaledValue));
}

std::unique_ptr<Exportable::Exporter>
AMD::PMFreqOdQMLItem::initializer(IQMLComponentFactory const &qmlComponentFactory,
                                  QQmlApplicationEngine &qmlEngine)
{
  return std::make_unique<AMD::PMFreqOdQMLItem::Initializer>(
      qmlComponentFactory, qmlEngine, *this);
}

QString AMD::PMFreqOdQMLItem::stateLabel(unsigned int value)
{
  return QString::fromStdString(std::to_string(value))
      .append(" ")
      .append(units::frequency::megahertz_t().abbreviation());
}

unsigned int AMD::PMFreqOdQMLItem::sclkOd() const
{
  return sclkOd_;
}

void AMD::PMFreqOdQMLItem::sclkOd(unsigned int value)
{
  sclkOd_ = value;
  emit sclkOdChanged(sclkOd_);

  if (baseSclk_ > 0) {
    auto scaledValue = sclkOd_ > 0
                           ? static_cast<unsigned int>(std::floor(
                                 baseSclk_ * (1.0f + (sclkOd_ / 100.0f))))
                           : baseSclk_;
    emit sclkChanged(stateLabel(scaledValue));
  }
}

unsigned int AMD::PMFreqOdQMLItem::mclkOd() const
{
  return mclkOd_;
}

void AMD::PMFreqOdQMLItem::mclkOd(unsigned int value)
{
  mclkOd_ = value;
  emit mclkOdChanged(mclkOd_);

  if (baseMclk_ > 0) {
    auto scaledValue = mclkOd_ > 0
                           ? static_cast<unsigned int>(std::floor(
                                 baseMclk_ * (1.0f + (mclkOd_ / 100.0f))))
                           : baseMclk_;
    emit mclkChanged(stateLabel(scaledValue));
  }
}

bool AMD::PMFreqOdQMLItem::register_()
{
  QMLComponentRegistry::addQMLTypeRegisterer([]() {
    qmlRegisterType<AMD::PMFreqOdQMLItem>("CoreCtrl.UIComponents", 1, 0,
                                          AMD::PMFreqOd::ItemID.data());
  });

  QMLComponentRegistry::addQMLItemProvider(
      AMD::PMFreqOd::ItemID, [](QQmlApplicationEngine &engine) {
        QQmlComponent component(&engine,
                                QStringLiteral("qrc:/qml/AMDPMFreqOdForm.qml"));
        return qobject_cast<QMLItem *>(component.create());
      });

  return true;
}

bool const AMD::PMFreqOdQMLItem::registered_ = AMD::PMFreqOdQMLItem::register_();
