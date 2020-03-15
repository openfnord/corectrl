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
