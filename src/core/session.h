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

#include "iprofile.h"
#include "isession.h"
#include <deque>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

class IProfileApplicator;
class IProfileViewFactory;
class IHelperMonitor;

class Session final : public ISession
{
 public:
  Session(std::shared_ptr<IProfileApplicator> profileApplicator,
          std::unique_ptr<IProfileManager> &&profileManager,
          std::unique_ptr<IProfileViewFactory> &&profileViewFactory,
          std::unique_ptr<IHelperMonitor> &&helperMonitor) noexcept;

  void init(ISysModel const &model) override;
  IProfileView const &profileView() const override;
  IProfileManager &profileManager() const override;

 private:
  void profileAdded(std::string const &profileName);
  void profileRemoved(std::string const &profileName);
  void profileChanged(std::string const &profileName);
  void profileActiveChanged(std::string const &profileName, bool active);
  void profileSaved(std::string const &profileName);
  void profileInfoChanged(IProfile::Info const &oldInfo,
                          IProfile::Info const &newInfo);

  void queueProfileViewForExecutable(std::string const &executableName);
  void dequeueProfileViewForExecutable(std::string const &executableName);

  void populateProfileIndex();
  void watchProfiles();
  void removeProfileIndexMapping(std::string const &profileName);
  void createProfileViews(std::vector<std::string> const &profileNames);
  void queueProfileView(std::string const &profileName);
  void dequeueProfileView(std::string const &profileName);

  std::shared_ptr<IProfileApplicator> profileApplicator_;
  std::unique_ptr<IProfileManager> profileManager_;
  std::unique_ptr<IProfileViewFactory> profileViewFactory_;
  std::unique_ptr<IHelperMonitor> helperMonitor_;

  class ProfileManagerObserver;
  std::shared_ptr<ProfileManagerObserver> const profileManagerObserver_;

  class HelperMonitorObserver;
  std::shared_ptr<HelperMonitorObserver> const helperMonitorObserver_;

  std::deque<std::unique_ptr<IProfileView>> pViews_;
  std::mutex pViewsMutex_;

  using executableName = std::string;
  using profileName = std::string;
  std::unordered_map<executableName, profileName> profileExeIndex_;
  std::mutex profileExeIndexMutex_;
};
