// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "isysmodel.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>

class ISWInfo;
class ISysComponent;

class SysModel final : public ISysModel
{
 public:
  SysModel(std::shared_ptr<ISWInfo> swInfo,
           std::vector<std::unique_ptr<ISysComponent>> &&components) noexcept;

  void init() override;
  void preInit(ICommandQueue &ctlCmds) override;
  void postInit(ICommandQueue &ctlCmds) override;
  void sync(ICommandQueue &ctlCmds) override;
  void updateSensors(
      std::unordered_map<std::string, std::unordered_set<std::string>> const
          &ignored) override;

  std::vector<std::pair<std::string, std::vector<std::pair<std::string, std::string>>>>
  info() const override;

  std::string const &ID() const override;
  void importWith(Importable::Importer &i) override;
  void exportWith(Exportable::Exporter &e) const override;

 private:
  std::pair<std::string, std::vector<std::pair<std::string, std::string>>>
  softwareInfo() const;

  std::string const id_;
  std::shared_ptr<ISWInfo> const swInfo_;
  std::vector<std::unique_ptr<ISysComponent>> const components_;
};
