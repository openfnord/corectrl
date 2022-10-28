// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2022 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include <string>

namespace Polkit {

enum class AuthResult {
  Yes,
  No,
  Error,
};

struct BusNameSubject
{
  std::string bus;
};

/// Checks whether a subject, identified by its bus name, is authorized to
/// perform an action. This operation requires user interaction.
AuthResult checkAuthorizationSync(std::string const &actionId,
                                  BusNameSubject const &busNameSubject);

} // namespace Polkit
