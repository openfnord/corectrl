// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/icommandqueue.h"
#include <QByteArray>
#include <vector>

class CommandQueueStub final : public ICommandQueue
{
 public:
  void pack(bool) override
  {
  }

  std::optional<bool> packWritesTo(std::string const &) override
  {
    return std::nullopt;
  }

  void add(std::pair<std::string, std::string> &&cmd) override
  {
    commands_.emplace_back(std::move(cmd));
  }

  QByteArray toRawData() override
  {
    return QByteArray();
  }

  std::vector<std::pair<std::string, std::string>> const &commands()
  {
    return commands_;
  }

  void clear()
  {
    commands_.clear();
  }

 private:
  std::vector<std::pair<std::string, std::string>> commands_;
};
