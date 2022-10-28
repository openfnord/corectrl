// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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
