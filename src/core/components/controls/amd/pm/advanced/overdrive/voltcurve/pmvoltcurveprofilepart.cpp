// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmvoltcurveprofilepart.h"

#include "core/profilepartprovider.h"
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <memory>

class AMD::PMVoltCurveProfilePart::Initializer final
: public AMD::PMVoltCurve::Exporter
{
 public:
  Initializer(AMD::PMVoltCurveProfilePart &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takePMVoltCurveModes(std::vector<std::string> const &modes) override;
  void takePMVoltCurvePointsRange(
      std::vector<std::pair<
          std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t>,
          std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t>>> const
          &pointRanges) override;

  void takeActive(bool active) override;
  void takePMVoltCurveMode(std::string const &mode) override;
  void takePMVoltCurvePoints(
      std::vector<std::pair<units::frequency::megahertz_t,
                            units::voltage::millivolt_t>> const &points) override;

 private:
  AMD::PMVoltCurveProfilePart &outer_;
};

void AMD::PMVoltCurveProfilePart::Initializer::takePMVoltCurveModes(
    std::vector<std::string> const &modes)
{
  outer_.modes_ = modes;
}

void AMD::PMVoltCurveProfilePart::Initializer::takePMVoltCurvePointsRange(
    std::vector<std::pair<
        std::pair<units::frequency::megahertz_t, units::frequency::megahertz_t>,
        std::pair<units::voltage::millivolt_t, units::voltage::millivolt_t>>> const
        &pointRanges)
{
  outer_.pointsRange_ = pointRanges;
}

void AMD::PMVoltCurveProfilePart::Initializer::takeActive(bool active)
{
  outer_.activate(active);
}

void AMD::PMVoltCurveProfilePart::Initializer::takePMVoltCurveMode(
    std::string const &mode)
{
  outer_.mode_ = mode;
}

void AMD::PMVoltCurveProfilePart::Initializer::takePMVoltCurvePoints(
    std::vector<std::pair<units::frequency::megahertz_t,
                          units::voltage::millivolt_t>> const &points)
{
  outer_.points_ = points;
}

AMD::PMVoltCurveProfilePart::PMVoltCurveProfilePart() noexcept
: id_(AMD::PMVoltCurve::ItemID)
{
}

std::unique_ptr<Exportable::Exporter>
AMD::PMVoltCurveProfilePart::factory(IProfilePartProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> AMD::PMVoltCurveProfilePart::initializer()
{
  return std::make_unique<AMD::PMVoltCurveProfilePart::Initializer>(*this);
}

std::string const &AMD::PMVoltCurveProfilePart::ID() const
{
  return id_;
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::PMVoltCurveProfilePart::provideImporter(Item const &)
{
  return {};
}

bool AMD::PMVoltCurveProfilePart::provideActive() const
{
  return active();
}

std::string const &AMD::PMVoltCurveProfilePart::providePMVoltCurveMode() const
{
  return mode_;
}

std::pair<units::frequency::megahertz_t, units::voltage::millivolt_t>
AMD::PMVoltCurveProfilePart::providePMVoltCurvePoint(unsigned int index) const
{
  if (index < points_.size())
    return points_.at(index);
  else
    return std::make_pair(units::frequency::megahertz_t(0),
                          units::voltage::millivolt_t(0));
}

void AMD::PMVoltCurveProfilePart::importProfilePart(IProfilePart::Importer &i)
{
  auto &importer = dynamic_cast<AMD::PMVoltCurveProfilePart::Importer &>(i);

  mode(importer.providePMVoltCurveMode());

  for (size_t i = 0; i < points_.size(); ++i) {
    auto [freq, volt] =
        importer.providePMVoltCurvePoint(static_cast<unsigned int>(i));
    point(static_cast<unsigned int>(i), freq, volt);
  }
}

void AMD::PMVoltCurveProfilePart::exportProfilePart(IProfilePart::Exporter &e) const
{
  auto &pmFVVoltCurveExporter =
      dynamic_cast<AMD::PMVoltCurveProfilePart::Exporter &>(e);

  pmFVVoltCurveExporter.takePMVoltCurveMode(mode_);
  pmFVVoltCurveExporter.takePMVoltCurvePoints(points_);
}

std::unique_ptr<IProfilePart> AMD::PMVoltCurveProfilePart::cloneProfilePart() const
{
  auto clone = std::make_unique<AMD::PMVoltCurveProfilePart>();

  clone->modes_ = modes_;
  clone->mode_ = mode_;
  clone->pointsRange_ = pointsRange_;
  clone->points_ = points_;

  return std::move(clone);
}

void AMD::PMVoltCurveProfilePart::mode(std::string const &mode)
{
  auto iter = std::find_if(
      modes_.cbegin(), modes_.cend(),
      [&](auto &availableMode) { return mode == availableMode; });
  if (iter != modes_.cend())
    mode_ = mode;
}

void AMD::PMVoltCurveProfilePart::point(unsigned int index,
                                        units::frequency::megahertz_t freq,
                                        units::voltage::millivolt_t volt)
{
  if (index < points_.size()) {
    auto [freqRange, voltRange] = pointsRange_.at(index);
    auto &[pointFreq, pointVolt] = points_.at(index);
    pointFreq = std::clamp(freq, freqRange.first, freqRange.second);
    pointVolt = std::clamp(volt, voltRange.first, voltRange.second);
  }
}

bool const AMD::PMVoltCurveProfilePart::registered_ =
    ProfilePartProvider::registerProvider(AMD::PMVoltCurve::ItemID, []() {
      return std::make_unique<AMD::PMVoltCurveProfilePart>();
    });
