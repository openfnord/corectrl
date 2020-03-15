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
#include "qmlcomponentregistry.h"

#include <utility>

std::vector<std::function<void()>> const &
QMLComponentRegistry::qmlTypeRegisterers() const
{
  return qmlTypeRegisterers_();
}

std::unordered_map<std::string,
                   std::function<QMLItem *(QQmlApplicationEngine &)>> const &
QMLComponentRegistry::qmlItemProviders() const
{
  return qmlItemProviders_();
}

std::unordered_map<std::string, std::function<QQuickItem *(void)>> const &
QMLComponentRegistry::quickItemProviders() const
{
  return quickItemProviders_();
}

bool QMLComponentRegistry::addQMLTypeRegisterer(std::function<void()> &&registerer)
{
  qmlTypeRegisterers_().emplace_back(std::move(registerer));
  return true;
}

bool QMLComponentRegistry::addQMLItemProvider(
    std::string_view componentID,
    std::function<QMLItem *(QQmlApplicationEngine &)> &&provider)
{
  qmlItemProviders_().emplace(std::string(componentID), std::move(provider));
  return true;
}

bool QMLComponentRegistry::addQuickItemProvider(
    std::string_view itemID, std::function<QQuickItem *(void)> &&provider)
{
  quickItemProviders_().emplace(std::string(itemID), std::move(provider));
  return true;
}

std::vector<std::function<void(void)>> &QMLComponentRegistry::qmlTypeRegisterers_()
{
  static std::vector<std::function<void(void)>> registerers;
  return registerers;
}

std::unordered_map<std::string, std::function<QMLItem *(QQmlApplicationEngine &)>> &
QMLComponentRegistry::qmlItemProviders_()
{
  static std::unordered_map<std::string,
                            std::function<QMLItem *(QQmlApplicationEngine &)>>
      providers;
  return providers;
}

std::unordered_map<std::string, std::function<QQuickItem *(void)>> &
QMLComponentRegistry::quickItemProviders_()
{
  static std::unordered_map<std::string, std::function<QQuickItem *(void)>> providers;
  return providers;
}
