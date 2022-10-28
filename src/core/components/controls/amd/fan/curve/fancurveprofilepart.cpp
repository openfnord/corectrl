// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "fancurveprofilepart.h"

#include "core/components/commonutils.h"
#include "core/profilepartprovider.h"
#include <algorithm>
#include <memory>

class AMD::FanCurveProfilePart::Initializer final : public AMD::FanCurve::Exporter
{
 public:
  Initializer(AMD::FanCurveProfilePart &outer) noexcept
  : outer_(outer)
  {
  }

  std::optional<std::reference_wrapper<Exportable::Exporter>>
  provideExporter(Item const &) override
  {
    return {};
  }

  void takeActive(bool active) override;
  void
  takeFanCurvePoints(std::vector<AMD::FanCurve::Point> const &points) override;
  void takeFanCurveFanStop(bool enabled) override;
  void takeFanCurveFanStartValue(units::concentration::percent_t value) override;
  void takeFanCurveTemperatureRange(units::temperature::celsius_t min,
                                    units::temperature::celsius_t max) override;

 private:
  AMD::FanCurveProfilePart &outer_;
};

void AMD::FanCurveProfilePart::Initializer::takeActive(bool active)
{
  outer_.activate(active);
}

void AMD::FanCurveProfilePart::Initializer::takeFanCurvePoints(
    std::vector<AMD::FanCurve::Point> const &points)
{
  outer_.points_ = points;
}

void AMD::FanCurveProfilePart::Initializer::takeFanCurveFanStop(bool enabled)
{
  outer_.fanStop_ = enabled;
}

void AMD::FanCurveProfilePart::Initializer::takeFanCurveFanStartValue(
    units::concentration::percent_t value)
{
  outer_.fanStartValue_ = value;
}

void AMD::FanCurveProfilePart::Initializer::takeFanCurveTemperatureRange(
    units::temperature::celsius_t min, units::temperature::celsius_t max)
{
  outer_.tempRange_ = std::make_pair(min, max);
}

AMD::FanCurveProfilePart::FanCurveProfilePart() noexcept
: id_(AMD::FanCurve::ItemID)
{
}

std::unique_ptr<Exportable::Exporter>
AMD::FanCurveProfilePart::factory(IProfilePartProvider const &)
{
  return nullptr;
}

std::unique_ptr<Exportable::Exporter> AMD::FanCurveProfilePart::initializer()
{
  return std::make_unique<AMD::FanCurveProfilePart::Initializer>(*this);
}

std::string const &AMD::FanCurveProfilePart::ID() const
{
  return id_;
}

std::optional<std::reference_wrapper<Importable::Importer>>
AMD::FanCurveProfilePart::provideImporter(Item const &)
{
  return {};
}

bool AMD::FanCurveProfilePart::provideActive() const
{
  return active();
}

std::vector<AMD::FanCurve::Point> const &
AMD::FanCurveProfilePart::provideFanCurvePoints() const
{
  return points_;
}

bool AMD::FanCurveProfilePart::provideFanCurveFanStop() const
{
  return fanStop_;
}

units::concentration::percent_t
AMD::FanCurveProfilePart::provideFanCurveFanStartValue() const
{
  return fanStartValue_;
}

void AMD::FanCurveProfilePart::importProfilePart(IProfilePart::Importer &i)
{
  auto &pmfImporter = dynamic_cast<AMD::FanCurveProfilePart::Importer &>(i);
  points(pmfImporter.provideFanCurvePoints());
  fanStop_ = pmfImporter.provideFanCurveFanStop();
  startValue(pmfImporter.provideFanCurveFanStartValue());
}

void AMD::FanCurveProfilePart::exportProfilePart(IProfilePart::Exporter &e) const
{
  auto &pmfExporter = dynamic_cast<AMD::FanCurveProfilePart::Exporter &>(e);
  pmfExporter.takeFanCurvePoints(points_);
  pmfExporter.takeFanCurveFanStop(fanStop_);
  pmfExporter.takeFanCurveFanStartValue(fanStartValue_);
}

std::unique_ptr<IProfilePart> AMD::FanCurveProfilePart::cloneProfilePart() const
{
  auto clone = std::make_unique<AMD::FanCurveProfilePart>();
  clone->tempRange_ = tempRange_;
  clone->points_ = points_;
  clone->fanStop_ = fanStop_;
  clone->fanStartValue_ = fanStartValue_;

  return std::move(clone);
}

void AMD::FanCurveProfilePart::points(std::vector<FanCurve::Point> const &points)
{
  points_ = points;
  Utils::Common::normalizePoints(points_, tempRange_);
}

void AMD::FanCurveProfilePart::startValue(units::concentration::percent_t value)
{
  fanStartValue_ = std::clamp(value, units::concentration::percent_t(0),
                              units::concentration::percent_t(100));
}

bool const AMD::FanCurveProfilePart::registered_ =
    ProfilePartProvider::registerProvider(AMD::FanCurve::ItemID, []() {
      return std::make_unique<AMD::FanCurveProfilePart>();
    });
