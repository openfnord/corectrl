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

class ICommandQueue;

class IDataSourceHandler
{
 public:
  /// Save the state of the data source.
  virtual void saveState() = 0;

  /// Queue commands to restore the saved data surce state.
  virtual void restoreState(ICommandQueue &ctlCmds) = 0;

  /// Queue commands to reset the data source.
  virtual void reset(ICommandQueue &ctlCmds) = 0;

  /// Queue commands to apply the current handler state to the data source.
  virtual void apply(ICommandQueue &ctlCmds) = 0;

  /// Queue commands to sync the handler state with the data source.
  virtual void sync(ICommandQueue &ctlCmds) = 0;

  virtual ~IDataSourceHandler() = default;
};
