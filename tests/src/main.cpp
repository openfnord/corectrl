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
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "trompeloeil.hpp"

namespace trompeloeil {
template<>
void reporter<specialized>::send(severity s, const char *file,
                                 unsigned long line, const char *msg)
{
  std::ostringstream os;
  if (line)
    os << file << ':' << line << '\n';
  os << msg;
  auto failure = os.str();
  if (s == severity::fatal) {
    FAIL(failure);
  }
  else {
    CAPTURE(failure);
    CHECK(failure.empty());
  }
}
} // namespace trompeloeil
