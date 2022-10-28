// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/info/vendor.h"
#include <filesystem>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

class App;

class AppFactory final
{
 public:
  AppFactory() noexcept;

  std::unique_ptr<App> build() const;

 private:
  std::tuple<std::filesystem::path, std::filesystem::path>
  standardDirectories() const;
  void createAppDirectories(std::string const &appDirectory,
                            std::filesystem::path const &config,
                            std::filesystem::path const &cache) const;

  std::vector<Vendor> gpuVendors_;
};
