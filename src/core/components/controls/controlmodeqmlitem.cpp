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
#include "controlmodeqmlitem.h"

#include "controlmode.h"
#include "core/item.h"
#include <QQuickItem>
#include <memory>
#include <utility>

class ControlModeQMLItem::Initializer final
: public QMLItem::Initializer
, public ControlMode::Exporter
{
 public:
  Initializer(IQMLComponentFactory const &qmlComponentFactory,
              QQmlApplicationEngine &qmlEngine,
              ControlModeQMLItem &qmlItem) noexcept
  : QMLItem::Initializer(qmlComponentFactory, qmlEngine)
  , outer_(qmlItem)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;

  void takeActive(bool active) override;

  void takeMode(std::string const &mode) override;
  void takeModes(std::vector<std::string> const &modes) override;

 private:
  ControlModeQMLItem &outer_;
};

std::optional<std::reference_wrapper<Exportable::Exporter>>
ControlModeQMLItem::Initializer::provideExporter(Item const &i)
{
  return initializer(i.ID(), &outer_).first;
}

void ControlModeQMLItem::Initializer::takeActive(bool active)
{
  outer_.takeActive(active);
}

void ControlModeQMLItem::Initializer::takeMode(std::string const &mode)
{
  outer_.takeMode(mode);
}

void ControlModeQMLItem::Initializer::takeModes(
    std::vector<std::string> const &modes)
{
  outer_.takeModes(modes);
}

ControlModeQMLItem::ControlModeQMLItem(std::string_view id) noexcept
{
  setName(tr(id.data()));
}

void ControlModeQMLItem::changeMode(QString const &mode)
{
  auto newMode = mode.toStdString();
  if (mode_ != newMode) {
    std::swap(mode_, newMode);
    emit modeChanged(mode);
    emit settingsChanged();
  }
}

std::optional<std::reference_wrapper<Importable::Importer>>
ControlModeQMLItem::provideImporter(Item const &i)
{
  auto item = this->findChild<QQuickItem *>(QString::fromStdString(i.ID()));
  if (item == nullptr)
    return {};

  return dynamic_cast<Importable::Importer &>(*item);
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
ControlModeQMLItem::provideExporter(Item const &i)
{
  auto item = this->findChild<QQuickItem *>(QString::fromStdString(i.ID()));
  if (item == nullptr)
    return {};

  return dynamic_cast<Exportable::Exporter &>(*item);
}

bool ControlModeQMLItem::provideActive() const
{
  return active_;
}

std::string const &ControlModeQMLItem::provideMode() const
{
  return mode_;
}

void ControlModeQMLItem::takeActive(bool active)
{
  active_ = active;
  setVisible(active);
}

void ControlModeQMLItem::takeMode(std::string const &mode)
{
  if (mode_ != mode) {
    mode_ = mode;
    emit modeChanged(QString::fromUtf8(mode_.data()));
  }
}

std::unique_ptr<Exportable::Exporter>
ControlModeQMLItem::initializer(IQMLComponentFactory const &qmlComponentFactory,
                                QQmlApplicationEngine &qmlEngine)
{
  return std::make_unique<ControlModeQMLItem::Initializer>(qmlComponentFactory,
                                                           qmlEngine, *this);
}

void ControlModeQMLItem::activate(bool active)
{
  takeActive(active);
}

void ControlModeQMLItem::takeModes(std::vector<std::string> const &modes)
{
  QList<QString> modeTextVector;
  for (auto mode : modes) {
    modeTextVector.push_back(QString::fromStdString(mode));
    modeTextVector.push_back(tr(mode.data()));
  }
  emit modesChanged(modeTextVector);
}
