// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

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

  /// Queue commands to sync the handler state with the data source.
  virtual void sync(ICommandQueue &ctlCmds) = 0;

  virtual ~IDataSourceHandler() = default;
};
