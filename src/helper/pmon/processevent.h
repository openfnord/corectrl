// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

struct ProcessEvent
{
  enum class Type {
    IGNORE,
    EXEC,
    EXIT,
  } event;

  int pid;
};
