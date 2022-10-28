// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

class QQuickItem;
class QQmlApplicationEngine;
class QMLItem;

class IQMLComponentRegistry
{
 public:
  virtual std::vector<std::function<void(void)>> const &
  qmlTypeRegisterers() const = 0;

  virtual std::unordered_map<
      std::string, std::function<QMLItem *(QQmlApplicationEngine &)>> const &
  qmlItemProviders() const = 0;

  virtual std::unordered_map<std::string, std::function<QQuickItem *(void)>> const &
  quickItemProviders() const = 0;

  virtual ~IQMLComponentRegistry() = default;
};
