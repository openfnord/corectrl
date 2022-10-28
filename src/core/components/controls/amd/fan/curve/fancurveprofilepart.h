// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#pragma once

#include "core/profilepart.h"
#include "fancurve.h"
#include <string>
#include <utility>
#include <vector>

namespace AMD {

class FanCurveProfilePart final
: public ProfilePart
, public AMD::FanCurve::Importer
{
 public:
  class Importer : public IProfilePart::Importer
  {
   public:
    virtual std::vector<FanCurve::Point> const &provideFanCurvePoints() const = 0;
    virtual bool provideFanCurveFanStop() const = 0;
    virtual units::concentration::percent_t
    provideFanCurveFanStartValue() const = 0;
  };

  class Exporter : public IProfilePart::Exporter
  {
   public:
    virtual void
    takeFanCurvePoints(std::vector<FanCurve::Point> const &points) = 0;
    virtual void takeFanCurveFanStop(bool enabled) = 0;
    virtual void
    takeFanCurveFanStartValue(units::concentration::percent_t value) = 0;
  };

  FanCurveProfilePart() noexcept;

  std::unique_ptr<Exportable::Exporter>
  factory(IProfilePartProvider const &profilePartProvider) override;
  std::unique_ptr<Exportable::Exporter> initializer() override;

  std::string const &ID() const override;

  std::optional<std::reference_wrapper<Importable::Importer>>
  provideImporter(Item const &i) override;

  bool provideActive() const override;
  std::vector<FanCurve::Point> const &provideFanCurvePoints() const override;
  bool provideFanCurveFanStop() const override;
  units::concentration::percent_t provideFanCurveFanStartValue() const override;

 protected:
  void importProfilePart(IProfilePart::Importer &i) override;
  void exportProfilePart(IProfilePart::Exporter &e) const override;
  std::unique_ptr<IProfilePart> cloneProfilePart() const override;

 private:
  void points(std::vector<FanCurve::Point> const &points);
  void startValue(units::concentration::percent_t value);

  class Initializer;

  std::string const id_;

  std::vector<FanCurve::Point> points_;
  std::pair<units::temperature::celsius_t, units::temperature::celsius_t> tempRange_;
  bool fanStop_;
  units::concentration::percent_t fanStartValue_;

  static bool const registered_;
};

} // namespace AMD
