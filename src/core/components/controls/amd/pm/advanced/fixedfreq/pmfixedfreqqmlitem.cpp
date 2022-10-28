// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmfixedfreqqmlitem.h"

#include "core/qmlcomponentregistry.h"
#include "pmfixedfreq.h"
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QString>
#include <QtGlobal>
#include <QtQml>
#include <memory>
#include <string>

char const *const AMD::PMFixedFreqQMLItem::trStrings[] = {
    QT_TRANSLATE_NOOP("ControlModeQMLItem", "AMD_PM_FIXED_FREQ"),
};

class AMD::PMFixedFreqQMLItem::Initializer final
: public QMLItem::Initializer
, public AMD::PMFixedFreq::Exporter
{
 public:
  Initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine,
              AMD::PMFixedFreqQMLItem &qmlItem) noexcept
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

  void takePMFixedFreqSclkIndex(unsigned int index) override;
  void takePMFixedFreqMclkIndex(unsigned int index) override;

  void takePMFixedFreqSclkStates(
      std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
          &states) override;
  void takePMFixedFreqMclkStates(
      std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
          &states) override;

 private:
  AMD::PMFixedFreqQMLItem &outer_;
};

void AMD::PMFixedFreqQMLItem::Initializer::takeActive(bool active)
{
  outer_.takeActive(active);
}

void AMD::PMFixedFreqQMLItem::Initializer::takePMFixedFreqSclkIndex(
    unsigned int index)
{
  outer_.takePMFixedFreqSclkIndex(index);
}

void AMD::PMFixedFreqQMLItem::Initializer::takePMFixedFreqMclkIndex(
    unsigned int index)
{
  outer_.takePMFixedFreqMclkIndex(index);
}

void AMD::PMFixedFreqQMLItem::Initializer::takePMFixedFreqSclkStates(
    std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const &states)
{
  outer_.takePMFixedFreqSclkStates(states);
}

void AMD::PMFixedFreqQMLItem::Initializer::takePMFixedFreqMclkStates(
    std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const &states)
{
  outer_.takePMFixedFreqMclkStates(states);
}

AMD::PMFixedFreqQMLItem::PMFixedFreqQMLItem() noexcept
{
  setName(tr(AMD::PMFixedFreq::ItemID.data()));
}

void AMD::PMFixedFreqQMLItem::changeSclkIndex(unsigned int index)
{
  if (sclkIndex() != index) {
    sclkIndex(index);
    emit settingsChanged();
  }
}

void AMD::PMFixedFreqQMLItem::changeMclkIndex(unsigned int index)
{
  if (mclkIndex() != index) {
    mclkIndex(index);
    emit settingsChanged();
  }
}

void AMD::PMFixedFreqQMLItem::activate(bool active)
{
  takeActive(active);
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMFixedFreqQMLItem::provideImporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
AMD::PMFixedFreqQMLItem::provideExporter(Item const &)
{
  return {};
}

bool AMD::PMFixedFreqQMLItem::provideActive() const
{
  return active_;
}

void AMD::PMFixedFreqQMLItem::takeActive(bool active)
{
  active_ = active;
  setVisible(active);
}

unsigned int AMD::PMFixedFreqQMLItem::providePMFixedFreqSclkIndex() const
{
  return sclkIndex();
}

unsigned int AMD::PMFixedFreqQMLItem::providePMFixedFreqMclkIndex() const
{
  return mclkIndex();
}

void AMD::PMFixedFreqQMLItem::takePMFixedFreqSclkIndex(unsigned int index)
{
  if (sclkIndex() != index)
    sclkIndex(index);
}

void AMD::PMFixedFreqQMLItem::takePMFixedFreqMclkIndex(unsigned int index)
{
  if (mclkIndex() != index)
    mclkIndex(index);
}

void AMD::PMFixedFreqQMLItem::takePMFixedFreqSclkStates(
    std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const &states)
{
  QVariantList freqStates;
  for (auto &[index, freq] : states) {
    freqStates.push_back(index);
    freqStates.push_back(stateLabel(freq.to<unsigned int>()));
  }

  emit sclkStatesChanged(freqStates);
}

void AMD::PMFixedFreqQMLItem::takePMFixedFreqMclkStates(
    std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const &states)
{
  QVariantList freqStates;
  for (auto &[index, freq] : states) {
    freqStates.push_back(index);
    freqStates.push_back(stateLabel(freq.to<unsigned int>()));
  }

  emit mclkStatesChanged(freqStates);
}

QString AMD::PMFixedFreqQMLItem::stateLabel(unsigned int value)
{
  return QString::fromStdString(std::to_string(value))
      .append(" ")
      .append(units::frequency::megahertz_t().abbreviation());
}

std::unique_ptr<Exportable::Exporter> AMD::PMFixedFreqQMLItem::initializer(
    IQMLComponentFactory const &qmlComponentFactory,
    QQmlApplicationEngine &qmlEngine)
{
  return std::make_unique<AMD::PMFixedFreqQMLItem::Initializer>(
      qmlComponentFactory, qmlEngine, *this);
}

unsigned int AMD::PMFixedFreqQMLItem::sclkIndex() const
{
  return sclkIndex_;
}

void AMD::PMFixedFreqQMLItem::sclkIndex(unsigned int index)
{
  sclkIndex_ = index;
  emit sclkIndexChanged(index);
}

unsigned int AMD::PMFixedFreqQMLItem::mclkIndex() const
{
  return mclkIndex_;
}

void AMD::PMFixedFreqQMLItem::mclkIndex(unsigned int index)
{
  mclkIndex_ = index;
  emit mclkIndexChanged(index);
}

bool AMD::PMFixedFreqQMLItem::register_()
{
  QMLComponentRegistry::addQMLTypeRegisterer([]() {
    qmlRegisterType<AMD::PMFixedFreqQMLItem>("CoreCtrl.UIComponents", 1, 0,
                                             AMD::PMFixedFreq::ItemID.data());
  });

  QMLComponentRegistry::addQMLItemProvider(
      AMD::PMFixedFreq::ItemID, [](QQmlApplicationEngine &engine) {
        QQmlComponent component(
            &engine, QStringLiteral("qrc:/qml/AMDPMFixedFreqForm.qml"));
        return qobject_cast<QMLItem *>(component.create());
      });

  return true;
}

bool const AMD::PMFixedFreqQMLItem::registered_ =
    AMD::PMFixedFreqQMLItem::register_();
