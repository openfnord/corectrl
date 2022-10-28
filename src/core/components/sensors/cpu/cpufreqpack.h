// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include <string_view>

/// A sensor that monitors the operation frequency of a physical cpu package.
/// The reported frequency is computed as the highest operation frequency
/// of all its execution units at the time.
namespace CPUFreqPack {

static constexpr std::string_view ItemID{"CPU_FREQ_PACK"};

} // namespace CPUFreqPack
