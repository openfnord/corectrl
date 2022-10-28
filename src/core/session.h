// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "iprofile.h"
#include "isession.h"
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class IProfileApplicator;
class IProfileView;
class IProfileViewFactory;
class IHelperMonitor;

class Session final : public ISession
{
 public:
  Session(std::shared_ptr<IProfileApplicator> profileApplicator,
          std::unique_ptr<IProfileManager> &&profileManager,
          std::unique_ptr<IProfileViewFactory> &&profileViewFactory,
          std::unique_ptr<IHelperMonitor> &&helperMonitor) noexcept;

  void addManualProfileObserver(
      std::shared_ptr<ISession::ManualProfileObserver> observer) override;
  void removeManualProfileObserver(
      std::shared_ptr<ISession::ManualProfileObserver> observer) override;

  void init(ISysModel const &model) override;
  void toggleManualProfile(std::string const &profileName) override;
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

  void populateProfileExeIndex();
  void watchProfiles();

  void createProfileViews(
      std::optional<std::reference_wrapper<IProfileView>> baseProfileView,
      std::vector<std::string> const &profileNames);

  std::optional<std::reference_wrapper<IProfileView>>
  getBaseView(std::deque<std::unique_ptr<IProfileView>> const &pViews,
              std::optional<std::string> const &manualProfile) const;

  void queueProfileView(std::string const &profileName);
  void dequeueProfileView(std::string const &profileName);

  void notifyManualProfileToggled(std::string const &profileName, bool active);

  std::shared_ptr<IProfileApplicator> profileApplicator_;
  std::unique_ptr<IProfileManager> profileManager_;
  std::unique_ptr<IProfileViewFactory> profileViewFactory_;
  std::unique_ptr<IHelperMonitor> helperMonitor_;

  class ProfileManagerObserver;
  std::shared_ptr<ProfileManagerObserver> const profileManagerObserver_;

  class HelperMonitorObserver;
  std::shared_ptr<HelperMonitorObserver> const helperMonitorObserver_;

  std::optional<std::string> manualProfile_;
  std::mutex manualProfileMutex_;

  std::deque<std::unique_ptr<IProfileView>> pViews_;
  std::mutex pViewsMutex_;

  using executableName = std::string;
  using profileName = std::string;
  std::unordered_map<executableName, profileName> profileExeIndex_;
  std::mutex profileExeIndexMutex_;

  std::vector<std::shared_ptr<ISession::ManualProfileObserver>> manualProfileObservers_;
  std::mutex manualProfileObserversMutex_;
};
