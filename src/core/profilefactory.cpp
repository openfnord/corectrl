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
#include "profilefactory.h"

#include "isysmodel.h"
#include "profile.h"
#include <utility>

ProfileFactory::ProfileFactory(
    std::unique_ptr<IProfilePartProvider> &&profilePartProvider) noexcept
: profilePartProvider_(std::move(profilePartProvider))
{
}

std::unique_ptr<IProfile> ProfileFactory::build(ISysModel const &sysModel) const
{
  auto profile = std::make_unique<Profile>();
  Profile::Factory profileFactory(*profilePartProvider_, *profile);
  sysModel.exportWith(profileFactory);
  return std::move(profile);
}
