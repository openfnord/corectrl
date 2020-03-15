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
#include "cpufreqqmlitem.h"

#include "core/qmlcomponentregistry.h"
#include "cpufreq.h"
#include <QQmlApplicationEngine>
#include <QtGlobal>
#include <QtQml>
#include <memory>

char const *const CPUFreqQMLItem::trStrings[] = {
    // XXX add cpufreq scaling governors here
    QT_TRANSLATE_NOOP("CPUFreqQMLItem", "performance"),
    QT_TRANSLATE_NOOP("CPUFreqQMLItem", "powersave"),
    QT_TRANSLATE_NOOP("CPUFreqQMLItem", "userspace"),
    QT_TRANSLATE_NOOP("CPUFreqQMLItem", "schedutil"),
    QT_TRANSLATE_NOOP("CPUFreqQMLItem", "ondemand"),
    QT_TRANSLATE_NOOP("CPUFreqQMLItem", "conservative"),
};

class CPUFreqQMLItem::Initializer final
: public QMLItem::Initializer
, public CPUFreq::Exporter
{
 public:
  Initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine, CPUFreqQMLItem &qmlItem) noexcept
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

  void takeCPUFreqScalingGovernor(std::string const &governor) override;
  void
  takeCPUFreqScalingGovernors(std::vector<std::string> const &governors) override;

 private:
  CPUFreqQMLItem &outer_;
};

void CPUFreqQMLItem::Initializer::takeActive(bool active)
{
  outer_.takeActive(active);
}

void CPUFreqQMLItem::Initializer::takeCPUFreqScalingGovernor(
    std::string const &governor)
{
  outer_.takeCPUFreqScalingGovernor(governor);
}

void CPUFreqQMLItem::Initializer::takeCPUFreqScalingGovernors(
    std::vector<std::string> const &governors)
{
  outer_.takeCPUFreqScalingGovernors(governors);
}

CPUFreqQMLItem::CPUFreqQMLItem() noexcept
{
  setName(tr(CPUFreq::ItemID.data()));
}

void CPUFreqQMLItem::changeScalingGovernor(QString const &governor)
{
  auto newScalingGovernor = governor.toStdString();
  if (scalingGovernor_ != newScalingGovernor) {
    std::swap(scalingGovernor_, newScalingGovernor);
    emit scalingGovernorChanged(governor);
    emit settingsChanged();
  }
}

void CPUFreqQMLItem::activate(bool active)
{
  takeActive(active);
}

std::optional<std::reference_wrapper<Importable::Importer>>
CPUFreqQMLItem::provideImporter(Item const &)
{
  return {};
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
CPUFreqQMLItem::provideExporter(Item const &)
{
  return {};
}

bool CPUFreqQMLItem::provideActive() const
{
  return active_;
}

std::string const &CPUFreqQMLItem::provideCPUFreqScalingGovernor() const
{
  return scalingGovernor_;
}

void CPUFreqQMLItem::takeActive(bool active)
{
  active_ = active;
  setVisible(active);
}

void CPUFreqQMLItem::takeCPUFreqScalingGovernor(std::string const &governor)
{
  if (scalingGovernor_ != governor) {
    scalingGovernor_ = governor;
    emit scalingGovernorChanged(QString::fromStdString(scalingGovernor_));
  }
}

std::unique_ptr<Exportable::Exporter>
CPUFreqQMLItem::initializer(IQMLComponentFactory const &qmlComponentFactory,
                            QQmlApplicationEngine &qmlEngine)
{
  return std::make_unique<CPUFreqQMLItem::Initializer>(qmlComponentFactory,
                                                       qmlEngine, *this);
}

void CPUFreqQMLItem::takeCPUFreqScalingGovernors(
    std::vector<std::string> const &governors)
{
  QList<QString> governorTextVector;
  for (auto governor : governors) {
    governorTextVector.push_back(QString::fromStdString(governor));
    governorTextVector.push_back(tr(governor.data()));
  }
  emit scalingGovernorsChanged(governorTextVector);
}

bool CPUFreqQMLItem::register_()
{
  QMLComponentRegistry::addQMLTypeRegisterer([]() {
    qmlRegisterType<CPUFreqQMLItem>("Radman.UIComponents", 1, 0,
                                    CPUFreq::ItemID.data());
  });

  QMLComponentRegistry::addQMLItemProvider(
      CPUFreq::ItemID, [](QQmlApplicationEngine &engine) {
        QQmlComponent component(&engine,
                                QStringLiteral("qrc:/qml/CPUFreqForm.qml"));
        return qobject_cast<QMLItem *>(component.create());
      });

  return true;
}

bool const CPUFreqQMLItem::registered_ = CPUFreqQMLItem::register_();
