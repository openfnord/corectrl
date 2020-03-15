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
#include "profilepartprovider.h"

#include <utility>

std::unordered_map<std::string, std::function<std::unique_ptr<IProfilePart>()>> const &
ProfilePartProvider::profilePartProviders() const
{
  return profilePartProviders_();
}

std::unordered_map<std::string, std::function<std::unique_ptr<IProfilePart>()>> &
ProfilePartProvider::profilePartProviders_()
{
  static std::unordered_map<std::string,
                            std::function<std::unique_ptr<IProfilePart>()>>
      providers;
  return providers;
}

bool ProfilePartProvider::registerProvider(
    std::string_view componentID,
    std::function<std::unique_ptr<IProfilePart>()> &&provider)
{
  profilePartProviders_().emplace(std::string(componentID), std::move(provider));
  return true;
}
