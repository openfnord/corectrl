// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "ihelpersysctl.h"
#include <memory>

class ICryptoLayer;
class QDBusInterface;

class HelperSysCtl final : public IHelperSysCtl
{
 public:
  HelperSysCtl(std::shared_ptr<ICryptoLayer> cryptoLayer) noexcept;

  void init() override;
  void apply(ICommandQueue &ctlCmds) override;

 private:
  std::shared_ptr<ICryptoLayer> cryptoLayer_;
  std::unique_ptr<QDBusInterface> sysCtlInterface_;
};
