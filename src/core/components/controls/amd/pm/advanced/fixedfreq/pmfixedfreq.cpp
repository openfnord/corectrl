// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "pmfixedfreq.h"

#include "core/components/controls/amd/pm/handlers/ippdpmhandler.h"
#include "core/icommandqueue.h"
#include "core/idatasource.h"

AMD::PMFixedFreq::PMFixedFreq(std::unique_ptr<IPpDpmHandler> &&ppDpmSclkHandler,
                              std::unique_ptr<IPpDpmHandler> &&ppDpmMclkHandler) noexcept
: Control(true)
, id_(AMD::PMFixedFreq::ItemID)
, ppDpmSclkHandler_(std::move(ppDpmSclkHandler))
, ppDpmMclkHandler_(std::move(ppDpmMclkHandler))
{
  auto &sclkStates = ppDpmSclkHandler_->states();
  if (!sclkStates.empty())
    ppDpmSclkHandler_->activate({sclkStates.front().first});

  auto &mclkStates = ppDpmMclkHandler_->states();
  if (!mclkStates.empty())
    ppDpmMclkHandler_->activate({mclkStates.front().first});
}

void AMD::PMFixedFreq::preInit(ICommandQueue &)
{
}

void AMD::PMFixedFreq::postInit(ICommandQueue &)
{
}

void AMD::PMFixedFreq::init()
{
}

std::string const &AMD::PMFixedFreq::ID() const
{
  return id_;
}

void AMD::PMFixedFreq::importControl(IControl::Importer &i)
{
  auto &pmFixedFreqImporter = dynamic_cast<AMD::PMFixedFreq::Importer &>(i);

  ppDpmSclkHandler_->activate(
      {pmFixedFreqImporter.providePMFixedFreqSclkIndex()});
  ppDpmMclkHandler_->activate(
      {pmFixedFreqImporter.providePMFixedFreqMclkIndex()});
}

void AMD::PMFixedFreq::exportControl(IControl::Exporter &e) const
{
  auto &pmFixedFreqExporter = dynamic_cast<AMD::PMFixedFreq::Exporter &>(e);

  pmFixedFreqExporter.takePMFixedFreqSclkStates(ppDpmSclkHandler_->states());
  pmFixedFreqExporter.takePMFixedFreqSclkIndex(
      ppDpmSclkHandler_->active().front());

  pmFixedFreqExporter.takePMFixedFreqMclkStates(ppDpmMclkHandler_->states());
  pmFixedFreqExporter.takePMFixedFreqMclkIndex(
      ppDpmMclkHandler_->active().front());
}

void AMD::PMFixedFreq::cleanControl(ICommandQueue &ctlCmds)
{
  ppDpmSclkHandler_->reset(ctlCmds);
  ppDpmMclkHandler_->reset(ctlCmds);
}

void AMD::PMFixedFreq::syncControl(ICommandQueue &ctlCmds)
{
  ppDpmSclkHandler_->sync(ctlCmds);
  ppDpmMclkHandler_->sync(ctlCmds);
}
