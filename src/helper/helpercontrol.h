// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "ihelpercontrol.h"
#include "units/units.h"
#include <QByteArray>
#include <QObject>
#include <QTimer>
#include <memory>
#include <optional>

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
  void helperHealthCheckTimeout();
  void helperExitDeferrerTimeout();

 private:
  bool helperHasBeenStarted() const;
  void createHelperInterface();
  std::optional<QByteArray> startHelper();
  void stopHelper();
  void killOtherHelperInstance();
  bool startHelperKiller();

  std::shared_ptr<ICryptoLayer> cryptoLayer_;
  QTimer deferHelperHealthCheckTimer_;
  QTimer deferHelperAutoExitSignalTimer_;
  std::unique_ptr<QDBusInterface> helperInterface_;

  units::time::millisecond_t autoExitTimeout_;
  units::time::millisecond_t deferAutoExitSignalInterval_;
};
