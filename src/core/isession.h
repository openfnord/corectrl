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
