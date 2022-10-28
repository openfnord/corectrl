// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmfreqod.h"

#include "core/components/amdutils.h"
#include "core/icommandqueue.h"
#include "core/idatasource.h"
#include <algorithm>
#include <cmath>

AMD::PMFreqOd::PMFreqOd(
    std::unique_ptr<IDataSource<unsigned int>> &&sclkOdDataSource,
    std::unique_ptr<IDataSource<unsigned int>> &&mclkOdDataSource,
    std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
        &sclkStates,
    std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const
        &mclkStates) noexcept
: Control(true)
, id_(AMD::PMFreqOd::ItemID)
, sclkOdDataSource_(std::move(sclkOdDataSource))
, mclkOdDataSource_(std::move(mclkOdDataSource))
, baseSclk_(0)
, baseMclk_(0)
, sclkOd_(0)
, mclkOd_(0)
{
  if (sclkOdDataSource_->read(sclkOdDataSourceEntry_) &&
      mclkOdDataSource_->read(mclkOdDataSourceEntry_)) {

    baseSclk_ = sclkStates.back().second;
    if (sclkOdDataSourceEntry_ > 0) {
      baseSclk_ = units::frequency::megahertz_t(std::round(
          baseSclk_.to<double>() * (100.0 / (sclkOdDataSourceEntry_ + 100))));
    }

    baseMclk_ = mclkStates.back().second;
    if (mclkOdDataSourceEntry_ > 0) {
      baseMclk_ = units::frequency::megahertz_t(std::round(
          baseMclk_.to<double>() * (100.0 / (mclkOdDataSourceEntry_ + 100))));
    }
  }
}

void AMD::PMFreqOd::preInit(ICommandQueue &)
{
}

void AMD::PMFreqOd::postInit(ICommandQueue &)
{
}

void AMD::PMFreqOd::init()
{
}

std::string const &AMD::PMFreqOd::ID() const
{
  return id_;
}

void AMD::PMFreqOd::importControl(IControl::Importer &i)
{
  auto &pmFreqOdImporter = dynamic_cast<AMD::PMFreqOd::Importer &>(i);

  sclkOd(pmFreqOdImporter.providePMFreqOdSclkOd());
  mclkOd(pmFreqOdImporter.providePMFreqOdMclkOd());
}

void AMD::PMFreqOd::exportControl(IControl::Exporter &e) const
{
  auto &pmFreqOdExporter = dynamic_cast<AMD::PMFreqOd::Exporter &>(e);

  pmFreqOdExporter.takePMFreqOdBaseSclk(baseSclk());
  pmFreqOdExporter.takePMFreqOdBaseMclk(baseMclk());

  pmFreqOdExporter.takePMFreqOdSclkOd(sclkOd());
  pmFreqOdExporter.takePMFreqOdMclkOd(mclkOd());
}

void AMD::PMFreqOd::cleanControl(ICommandQueue &ctlCmds)
{
  ctlCmds.add({sclkOdDataSource_->source(), std::to_string(0)});
  ctlCmds.add({mclkOdDataSource_->source(), std::to_string(0)});
}

void AMD::PMFreqOd::syncControl(ICommandQueue &ctlCmds)
{
  if (sclkOdDataSource_->read(sclkOdDataSourceEntry_) &&
      mclkOdDataSource_->read(mclkOdDataSourceEntry_)) {

    if (sclkOdDataSourceEntry_ != sclkOd_)
      ctlCmds.add({sclkOdDataSource_->source(), std::to_string(sclkOd())});

    if (mclkOdDataSourceEntry_ != mclkOd_)
      ctlCmds.add({mclkOdDataSource_->source(), std::to_string(mclkOd())});
  }
}

unsigned int AMD::PMFreqOd::sclkOd() const
{
  return sclkOd_;
}

void AMD::PMFreqOd::sclkOd(unsigned int value)
{
  sclkOd_ = std::clamp(value, 0u, 20u);
}

unsigned int AMD::PMFreqOd::mclkOd() const
{
  return mclkOd_;
}

void AMD::PMFreqOd::mclkOd(unsigned int value)
{
  mclkOd_ = std::clamp(value, 0u, 20u);
}

units::frequency::megahertz_t AMD::PMFreqOd::baseSclk() const
{
  return baseSclk_;
}

units::frequency::megahertz_t AMD::PMFreqOd::baseMclk() const
{
  return baseMclk_;
}
