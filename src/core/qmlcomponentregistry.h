// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
