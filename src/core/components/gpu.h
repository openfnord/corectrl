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

#include "igpu.h"
#include <memory>
#include <string>
#include <vector>

class IControl;
class ISensor;

class GPU final : public IGPU
{
 public:
  GPU(std::unique_ptr<IGPUInfo> &&info,
      std::vector<std::unique_ptr<IControl>> &&controls,
      std::vector<std::unique_ptr<ISensor>> &&sensors)
  noexcept;

  bool active() const override;
  void activate(bool active) override;

  std::string const &key() const override;

  std::pair<std::string, std::vector<std::pair<std::string, std::string>>>
  componentInfo() const override;

  void init() override;
  void preInit(ICommandQueue &ctlCmds) override;
  void postInit(ICommandQueue &ctlCmds) override;
  void sync(ICommandQueue &ctlCmds) override;

  void updateSensors(
      std::unordered_map<std::string, std::unordered_set<std::string>> const
          &ignored) override;

  IGPUInfo const &info() const override;

  std::string const &ID() const override;
  void importWith(Importable::Importer &i) override;
  void exportWith(Exportable::Exporter &e) const override;

 private:
  std::string const id_;
  std::unique_ptr<IGPUInfo> const info_;
  std::vector<std::unique_ptr<IControl>> const controls_;
  std::vector<std::unique_ptr<ISensor>> const sensors_;

  std::string key_;
  bool active_{true};
};
