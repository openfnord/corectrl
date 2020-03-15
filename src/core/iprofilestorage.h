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
#include <filesystem>
#include <memory>
#include <vector>

/// Stores profiles into a location.
class IProfileStorage
{
 public:
  /// Initializes the profile storage.
  /// @param defaultProfile default profile of the application
  virtual void init(IProfile const &defaultProfile) = 0;

  /// Reads the profiles on the storage.
  /// @param baseProfile profile used as creation base
  /// @return loaded profiles
  virtual std::vector<std::unique_ptr<IProfile>>
  profiles(IProfile const &baseProfile) = 0;

  /// Loads a profile from the storage.
  /// @param profile profile to be loaded
  /// @return true on success
  virtual bool load(IProfile &profile) = 0;

  /// Saves a profile into the storage.
  /// @param profile profile to be saved
  /// @return true on success
  virtual bool save(IProfile &profile) = 0;

  /// Loads a profile configuration from a file.
  /// @param profile profile to be loaded
  /// @param path path to the profile file
  /// @return true on success
  virtual bool loadFrom(IProfile &profile,
                        std::filesystem::path const &path) const = 0;

  /// Exports a profile to a profile file.
  /// @param profile profile to be exported
  /// @param path path to the target profile file
  /// @return true on success
  virtual bool exportTo(IProfile const &profile,
                        std::filesystem::path const &path) const = 0;

  /// Updates the info of a stored profile with new info.
  /// @param profile profile of the stored profile to be updated
  /// @param newInfo info to be updated
  /// @return true on success
  virtual bool update(IProfile const &profile, IProfile::Info &newInfo) = 0;

  /// Removes a stored profile.
  /// @param info info of the stored profile to be removed
  virtual void remove(IProfile::Info &info) = 0;

  virtual ~IProfileStorage() = default;
};
