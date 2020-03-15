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

#include "iqmlcomponentregistry.h"
#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

class QMLItem;
class QQmlApplicationEngine;
class QQuickItem;

class QMLComponentRegistry final : public IQMLComponentRegistry
{
 public:
  std::vector<std::function<void(void)>> const &
  qmlTypeRegisterers() const override;

  std::unordered_map<std::string,
                     std::function<QMLItem *(QQmlApplicationEngine &)>> const &
  qmlItemProviders() const override;

  std::unordered_map<std::string, std::function<QQuickItem *(void)>> const &
  quickItemProviders() const override;

  static bool addQMLTypeRegisterer(std::function<void(void)> &&registerer);

  static bool addQMLItemProvider(
      std::string_view componentID,
      std::function<QMLItem *(QQmlApplicationEngine &)> &&provider);

  static bool addQuickItemProvider(std::string_view itemID,
                                   std::function<QQuickItem *(void)> &&provider);

 private:
  static std::vector<std::function<void(void)>> &qmlTypeRegisterers_();
  static std::unordered_map<std::string,
                            std::function<QMLItem *(QQmlApplicationEngine &)>> &
  qmlItemProviders_();
  static std::unordered_map<std::string, std::function<QQuickItem *(void)>> &
  quickItemProviders_();
};
