// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "profilexmlparser.h"

#include "easyloggingpp/easylogging++.h"
#include "fmt/format.h"
#include "iprofile.h"
#include "isyscomponentprofilepart.h"
#include "pugixml/pugixml.hpp"
#include <algorithm>
#include <cctype>
#include <utility>

ProfileXMLParser::Factory::Factory(
    IProfilePartXMLParserProvider const &profilePartParserProvider,
    ProfileXMLParser &outer) noexcept
: ProfilePartXMLParser::Factory(profilePartParserProvider)
, outer_(outer)
{
}

void ProfileXMLParser::Factory::takePartParser(
    Item const &i, std::unique_ptr<IProfilePartXMLParser> &&part)
{
  auto &key = dynamic_cast<ISysComponentProfilePart const &>(i).key();
  outer_.parsers_.emplace(key, std::move(part));
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
ProfileXMLParser::Factory::provideExporter(Item const &i)
{
  if (i.ID() == IProfile::ItemID)
    return *this;
  else
    return factory(i);
}

class ProfileXMLParser::Initializer final : public IProfile::Exporter
{
 public:
  Initializer(ProfileXMLParser &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &i) override;

  void takeActive(bool active) override;
  void takeInfo(IProfile::Info const &info) override;

 private:
  ProfileXMLParser &outer_;
  std::unordered_map<std::string, std::unique_ptr<Exportable::Exporter>> initializers_;
};

std::optional<std::reference_wrapper<Exportable::Exporter>>
ProfileXMLParser::Initializer::provideExporter(Item const &i)
{
  auto &id = i.ID();
  if (id == IProfile::ItemID)
    return *this;
  else {
    auto &key = dynamic_cast<ISysComponentProfilePart const &>(i).key();
    if (initializers_.count(key) > 0)
      return *initializers_.at(key);
    else if (outer_.parsers_.count(key) > 0) {
      auto initializer = outer_.parsers_.at(key)->initializer();
      if (initializer != nullptr) {
        initializers_.emplace(key, std::move(initializer));
        return *initializers_.at(key);
      }
    }
  }

  return {};
}

void ProfileXMLParser::Initializer::takeActive(bool active)
{
  outer_.active_ = outer_.activeDefault_ = active;
}

void ProfileXMLParser::Initializer::takeInfo(IProfile::Info const &info)
{
  outer_.info_ = outer_.infoDefault_ = info;
}

ProfileXMLParser::ProfileXMLParser() noexcept
: format_("xml")
{
  profileNodeName_ = IProfile::ItemID;
  std::transform(profileNodeName_.cbegin(), profileNodeName_.cend(),
                 profileNodeName_.begin(), ::toupper);
}

std::string const &ProfileXMLParser::format()
{
  return format_;
}

std::unique_ptr<Exportable::Exporter> ProfileXMLParser::initializer()
{
  return std::make_unique<ProfileXMLParser::Initializer>(*this);
}

bool ProfileXMLParser::load(std::vector<char> const &data, IProfile &profile)
{
  pugi::xml_document doc;
  auto status = doc.load_buffer(data.data(), data.size());
  if (status) {
    auto profileNode = doc.child(profileNodeName_.c_str());
    if (!profileNode.empty()) {

      auto active = profileNode.attribute("active");
      auto name = profileNode.attribute("name");
      auto exe = profileNode.attribute("exe");

      active_ = active.as_bool(activeDefault_);
      info_.name = name.as_string(infoDefault_.name.c_str());
      info_.exe = exe.as_string(infoDefault_.exe.c_str());

      for (auto &[key, component] : parsers_)
        component->loadFrom(profileNode);

      profile.importWith(*this);
      return true;
    }
  }

  LOG(ERROR) << fmt::format("Cannot parse xml data for profile {}.\nError: {}",
                            profile.info().name, status.description());

  return false;
}

class PugiXMLWriter final : public pugi::xml_writer
{
 public:
  PugiXMLWriter(std::vector<char> &data)
  : data_(data)
  {
    data.clear();
  }
  void write(const void *data, size_t size) override
  {
    auto start = data_.size();
    data_.resize(start + size);
    memcpy(&data_[start], data, size * sizeof(char));
  }

 private:
  std::vector<char> &data_;
};

bool ProfileXMLParser::save(std::vector<char> &data, IProfile const &profile)
{
  profile.exportWith(*this);

  pugi::xml_document doc;
  auto profileNode = doc.append_child(profileNodeName_.c_str());
  profileNode.append_attribute("active") = active_;
  profileNode.append_attribute("name") = info_.name.c_str();
  profileNode.append_attribute("exe") = info_.exe.c_str();

  for (auto &[key, component] : parsers_)
    component->appendTo(profileNode);

  PugiXMLWriter writer(data);
  doc.save(writer);

  return true;
}

std::optional<std::reference_wrapper<Exportable::Exporter>>
ProfileXMLParser::provideExporter(Item const &i)
{
  if (i.ID() == IProfile::ItemID)
    return *this;

  auto &key = dynamic_cast<ISysComponentProfilePart const &>(i).key();
  auto const iter = parsers_.find(key);
  if (iter != parsers_.cend())
    return iter->second->profilePartExporter();

  return {};
}

std::optional<std::reference_wrapper<Importable::Importer>>
ProfileXMLParser::provideImporter(Item const &i)
{
  if (i.ID() == IProfile::ItemID)
    return *this;

  auto &key = dynamic_cast<ISysComponentProfilePart const &>(i).key();
  auto const iter = parsers_.find(key);
  if (iter != parsers_.cend())
    return iter->second->profilePartImporter();

  return {};
}

void ProfileXMLParser::takeActive(bool active)
{
  active_ = active;
}

void ProfileXMLParser::takeInfo(IProfile::Info const &info)
{
  info_ = info;
}

bool ProfileXMLParser::provideActive() const
{
  return active_;
}

IProfile::Info const &ProfileXMLParser::provideInfo() const
{
  return info_;
}
