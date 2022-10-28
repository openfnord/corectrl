// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include <optional>
#include <string>
#include <utility>

class QByteArray;

class ICommandQueue
{
 public:
  /// Activates or deactivates command packing.
  /// When command packing is activated, all subsequent queued
  /// commands writing to the same file will be packed together.
  /// Activation only have effect the first time it's used.
  /// @param activate whether to activate command packing
  virtual void pack(bool activate) = 0;

  /// When command packing is active, returns whether the pack
  /// contains any command that writes to the specified file.
  /// If packing mode is not active, the optional will be null.
  /// @param file file to search for in the pack
  virtual std::optional<bool> packWritesTo(std::string const &file) = 0;

  /// Adds a command to the end of the queue.
  /// If the command was already queued, there is no effect in the queue.
  /// @param cmd command to queue
  virtual void add(std::pair<std::string, std::string> &&cmd) = 0;

  /// Transform all commands into raw data, cleaning the command queue.
  virtual QByteArray toRawData() = 0;

  virtual ~ICommandQueue() = default;
};
