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
#include "profilepartview.h"

#include <utility>

ProfilePartView::ProfilePartView(std::string const &profile,
                                 std::shared_ptr<IProfilePart> part) noexcept
: profile_(profile)
, part_(std::move(part))
{
}

std::string const &ProfilePartView::profile() const
{
  return profile_;
}

std::shared_ptr<IProfilePart> const &ProfilePartView::part() const
{
  return part_;
}
