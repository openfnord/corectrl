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
#include "profilepartxmlparserprovider.h"

#include <utility>

std::unordered_map<std::string,
                   std::function<std::unique_ptr<IProfilePartXMLParser>()>> const &
ProfilePartXMLParserProvider::profilePartParserProviders() const
{
  return profilePartParserProviders_();
}

std::unordered_map<std::string,
                   std::function<std::unique_ptr<IProfilePartXMLParser>()>> &
ProfilePartXMLParserProvider::profilePartParserProviders_()
{
  static std::unordered_map<
      std::string, std::function<std::unique_ptr<IProfilePartXMLParser>()>>
      providers;
  return providers;
}

bool ProfilePartXMLParserProvider::registerProvider(
    std::string_view componentID,
    std::function<std::unique_ptr<IProfilePartXMLParser>()> &&provider)
{
  profilePartParserProviders_().emplace(std::string(componentID),
                                        std::move(provider));
  return true;
}
