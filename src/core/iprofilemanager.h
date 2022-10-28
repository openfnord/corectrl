// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "importable.h"
#include "iprofile.h"
#include <filesystem>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

class ISysModel;

class IProfileManager
{
 public:
  /// Profile manager observers must implement this interface.
  class Observer
  {
   public:
    /// A new profile was added.
    /// @param profileName name of the added profile
    virtual void profileAdded(std::string const &profileName) = 0;

    /// A profile was removed.
    /// @param profileName name of the removed profile
    virtual void profileRemoved(std::string const &profileName) = 0;

    /// The settings of a profile changed.
    /// @param profileName name of the profile that changed
    virtual void profileChanged(std::string const &profileName) = 0;

    /// The active state of a profile changed.
    /// @param profileName name of the profile that changed
    /// @param active new active state
    virtual void profileActiveChanged(std::string const &profileName,
                                      bool active) = 0;

    /// A profile was saved.
    /// @param profileName name of the saved profile
    virtual void profileSaved(std::string const &profileName) = 0;

    /// The info of a profile changed.
    /// @param oldInfo old profile info
    /// @param newInfo new profile info
    virtual void profileInfoChanged(IProfile::Info const &oldInfo,
                                    IProfile::Info const &newInfo) = 0;

    virtual ~Observer() = default;
  };

  /// Adds a new profile manager observer.
  /// @param observer new observer to be added
  virtual void
  addObserver(std::shared_ptr<IProfileManager::Observer> observer) = 0;

  /// Removes a observer from the profile manager observers
  /// @param observer observer to be removed
  virtual void
  removeObserver(std::shared_ptr<IProfileManager::Observer> const &observer) = 0;

  /// Initialization method.
  /// @param model system model to initialize from
  virtual void init(ISysModel const &model) = 0;

  /// Available profiles.
  /// @return names of the available profiles
  virtual std::vector<std::string> profiles() const = 0;

  /// Request a profile from the avaiable profiles.
  /// @param profileName name of the requested profile
  /// @return the named profile when is available
  virtual std::optional<std::reference_wrapper<IProfile const>>
  profile(std::string const &profileName) const = 0;

  /// Request the list of not saved profiles.
  /// @return names of the unsaved profiles
  virtual std::vector<std::string> unsavedProfiles() const = 0;

  /// Checks whether a profile is not saved.
  /// @param profileName profile name to be checked
  /// @return true when the profile is unsaved
  virtual bool unsaved(std::string const &profileName) const = 0;

  /// Adds a new profile with the supplied info.
  /// @param info info of the profile to be created
  virtual void add(IProfile::Info const &info) = 0;

  /// Adds a clone of a profile with the supplied info.
  /// @param cloneInfo info of the resulting profile
  /// @param baseProfileName name of the profile to be cloned
  virtual void clone(IProfile::Info const &cloneInfo,
                     std::string const &baseProfileName) = 0;

  /// Removes a profile.
  /// @param profileName name of the profile to be removed
  virtual void remove(std::string const &profileName) = 0;

  /// Changes the active state of a profile.
  /// @param profileName name of the profile to be changed
  /// @param active new active state
  virtual void activate(std::string const &profileName, bool active) = 0;

  /// Reset a profile settings to its defaults values.
  /// @param profileName name of the profile to be reseted
  virtual void reset(std::string const &profileName) = 0;

  /// Restore a profile from the last saved state.
  /// @param profileName name of the profile to be restored
  virtual void restore(std::string const &profileName) = 0;

  /// Updates the info of a profile.
  /// @param profileName name of the profile to be updated
  /// @param newInfo profile's new info
  virtual void update(std::string const &profileName,
                      IProfile::Info const &newInfo) = 0;

  /// Updates a profile settings from a profile importer.
  /// @param profileName name of the profile to be updated
  /// @param importer profile importer
  virtual void update(std::string const &profileName,
                      Importable::Importer &importer) = 0;

  /// Loads a profile settings from a profile file.
  /// @param profileName name of the profile to be loaded
  /// @param path path to the profile file
  /// @return true when the loading was successful
  virtual bool loadFrom(std::string const &profileName,
                        std::filesystem::path const &path) = 0;

  /// Saves a profile.
  /// @param profileName name of the profile to be saved
  virtual void save(std::string const &profileName) = 0;

  /// Exports a profile to a profile file.
  /// @param profileName name of the profile to be exported
  /// @param path destination path of the exported profile file
  /// @return true when the exporting was successful
  virtual bool exportTo(std::string const &profileName,
                        std::filesystem::path const &path) = 0;

  virtual ~IProfileManager() = default;
};
