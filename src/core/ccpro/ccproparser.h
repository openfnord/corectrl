// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/iprofilefileparser.h"

class CCPROParser final : public IProfileFileParser
{
 public:
  std::string fileExtension() const override;

  std::optional<std::vector<char>>
  load(std::filesystem::path const &path,
       std::string const &internalDataName) override;

  bool save(std::filesystem::path const &path,
            std::vector<std::pair<std::string, std::vector<char>>> const &data)
      override;
};
