//
// Copyright 2022 Juan Palacios <jpalaciosdev@gmail.com>
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
#include "polkit.h"

#define QT_NO_KEYWORDS
#include <polkit/polkit.h>

#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"

namespace Polkit {

class Authority
{
 public:
  Authority() noexcept;
  ~Authority();

  PolkitAuthority *get() const;

 private:
  PolkitAuthority *authority_;
};

Authority::Authority() noexcept
: authority_(nullptr)
{
  GError *error = nullptr;

  authority_ = polkit_authority_get_sync(nullptr, &error);
  if (authority_ == nullptr) {
    LOG(ERROR) << fmt::format("Could not get Polkit authority: {}",
                              error->message);
    g_error_free(error);
    return;
  }
}

Authority::~Authority()
{
  g_object_unref(authority_);
}

PolkitAuthority *Authority::get() const
{
  return authority_;
}

class BusNameSubjectImpl
{
 public:
  BusNameSubjectImpl(BusNameSubject const &subject) noexcept;
  ~BusNameSubjectImpl();

  PolkitSubject *get() const;

 private:
  PolkitSubject *subject_;
};

BusNameSubjectImpl::BusNameSubjectImpl(BusNameSubject const &subject) noexcept
: subject_(nullptr)
{
  subject_ = polkit_system_bus_name_new(subject.bus.c_str());
}

BusNameSubjectImpl::~BusNameSubjectImpl()
{
  g_object_unref(subject_);
}

PolkitSubject *BusNameSubjectImpl::get() const
{
  return subject_;
}

AuthResult checkAuthorizationSync(std::string const &actionId,
                                  BusNameSubject const &subject)
{
  auto polkitAuthority = Authority();
  if (polkitAuthority.get() == nullptr)
    return AuthResult::Error;

  auto polkitSubject = BusNameSubjectImpl(subject);
  if (polkitSubject.get() == nullptr)
    return AuthResult::Error;

  GError *error = nullptr;
  auto polkitResult = polkit_authority_check_authorization_sync(
      polkitAuthority.get(), polkitSubject.get(), actionId.c_str(), nullptr,
      POLKIT_CHECK_AUTHORIZATION_FLAGS_ALLOW_USER_INTERACTION, nullptr, &error);
  if (error) {
    LOG(ERROR) << fmt::format("Could not check Polkit authorization: {}",
                              error->message);
    g_error_free(error);
    return AuthResult::Error;
  }

  auto result = polkit_authorization_result_get_is_authorized(polkitResult)
                    ? AuthResult::Yes
                    : AuthResult::No;

  g_object_unref(polkitResult);

  return result;
}

} // namespace Polkit
