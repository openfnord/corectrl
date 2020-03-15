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
#pragma once

#include "ihelpercontrol.h"
#include "units/units.h"
#include <QByteArray>
#include <QObject>
#include <QTimer>
#include <memory>

class ICryptoLayer;
class QDBusInterface;

class HelperControl final
: public QObject
, public IHelperControl
{
  Q_OBJECT

 public:
  HelperControl(std::shared_ptr<ICryptoLayer> cryptoLayer,
                QObject *parent = nullptr) noexcept;

  units::time::millisecond_t minExitTimeout() const override;
  void init(units::time::millisecond_t autoExitTimeout) override;
  void stop() override;

 private slots:
  void helperExitDeferrerTimeout();

 private:
  bool isHelperRunning() const;
  void createHelperInterface();
  QByteArray startHelper(units::time::millisecond_t autoExitTimeout,
                         units::time::millisecond_t deferAutoExitSignalInterval);
  void stopHelper();
  bool killOtherHelper() const;

  std::shared_ptr<ICryptoLayer> cryptoLayer_;
  QTimer deferHelperAutoExitSignalTimer_;
  std::unique_ptr<QDBusInterface> helperInterface_;
};
