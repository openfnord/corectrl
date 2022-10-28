// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include <memory>
#include <string>

class IProfileView;
class IProfileManager;
class ISysModel;

class ISession
{
 public:
  class ManualProfileObserver
  {
   public:
    virtual void toggled(std::string const &profileName, bool active) = 0;
    virtual ~ManualProfileObserver() = default;
  };

  virtual void addManualProfileObserver(
      std::shared_ptr<ISession::ManualProfileObserver> observer) = 0;
  virtual void removeManualProfileObserver(
      std::shared_ptr<ISession::ManualProfileObserver> observer) = 0;

  virtual void init(ISysModel const &model) = 0;
  virtual void toggleManualProfile(std::string const &profileName) = 0;
  virtual IProfileManager &profileManager() const = 0;

  virtual ~ISession() = default;
};
