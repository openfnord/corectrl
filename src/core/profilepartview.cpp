// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "profilepartview.h"

#include <utility>

ProfilePartView::ProfilePartView(std::string const &profile,
                                 std::shared_ptr<IProfilePart> part) noexcept
: profile_(profile)
, part_(std::move(part))
{
}

std::string const &ProfilePartView::profile() const
{
  return profile_;
}

std::shared_ptr<IProfilePart> const &ProfilePartView::part() const
{
  return part_;
}
