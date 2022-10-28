// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "iprofile.h"
#include "iprofilemanager.h"
#include "iprofilestorage.h"
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>

class ProfileManager final : public IProfileManager
{
 public:
  ProfileManager(std::unique_ptr<IProfile> &&defaultProfile,
                 std::unique_ptr<IProfileStorage> &&profileStorage) noexcept;

  void addObserver(std::shared_ptr<IProfileManager::Observer> observer) override;
  void removeObserver(
      std::shared_ptr<IProfileManager::Observer> const &observer) override;

  void init(ISysModel const &model) override;

  std::vector<std::string> profiles() const override;
  std::optional<std::reference_wrapper<IProfile const>>
  profile(std::string const &profileName) const override;

  std::vector<std::string> unsavedProfiles() const override;
  bool unsaved(std::string const &profileName) const override;

  void add(IProfile::Info const &info) override;
  void clone(IProfile::Info const &cloneInfo,
             std::string const &baseProfileName =
                 std::string(IProfile::Info::GlobalID)) override;
  void remove(std::string const &profileName) override;
  void activate(std::string const &profileName, bool active) override;
  void reset(std::string const &profileName) override;
  void restore(std::string const &profileName) override;
  void update(std::string const &profileName,
              IProfile::Info const &newInfo) override;
  void update(std::string const &profileName,
              Importable::Importer &importer) override;
  bool loadFrom(std::string const &profileName,
                std::filesystem::path const &path) override;
  void save(std::string const &profileName) override;
  bool exportTo(std::string const &profileName,
                std::filesystem::path const &path) override;

 private:
  void notifyProfileAdded(std::string const &profileName);
  void notifyProfileRemoved(std::string const &profileName);
  void notifyProfileChanged(std::string const &profileName);
  void notifyProfileActiveChanged(std::string const &profileName, bool active);
  void notifyProfileSaved(std::string const &profileName);
  void notifyProfileInfoChanged(IProfile::Info const &oldInfo,
                                IProfile::Info const &newInfo);

  std::unique_ptr<IProfile> defaultProfile_;
  std::unique_ptr<IProfileStorage> profileStorage_;
  std::unordered_map<std::string, std::unique_ptr<IProfile>> profiles_;
  std::unordered_set<std::string> unsavedProfiles_;

  std::vector<std::shared_ptr<IProfileManager::Observer>> observers_;
  std::mutex obMutex_;
};
