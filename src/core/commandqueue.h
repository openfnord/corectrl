// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "icommandqueue.h"
#include <string>
#include <utility>
#include <vector>

class CommandQueue : public ICommandQueue
{
 public:
  CommandQueue() noexcept;

  void pack(bool activate) override;
  std::optional<bool> packWritesTo(std::string const &file) override;
  void add(std::pair<std::string, std::string> &&cmd) override;
  QByteArray toRawData() override;

 protected:
  std::vector<std::pair<std::string, std::string>> &commands();
  std::optional<unsigned int> const &packIndex() const;

 private:
  std::optional<unsigned int> packIndex_;
  std::vector<std::pair<std::string, std::string>> commands_;
};
