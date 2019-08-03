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
#include "pmfixedfreq.h"

#include "core/components/controls/amd/pm/handlers/ippdpmhandler.h"
#include "core/icommandqueue.h"
#include "core/idatasource.h"

AMD::PMFixedFreq::PMFixedFreq(
    std::unique_ptr<IDataSource<std::string>> &&perfLevelDataSource,
    std::unique_ptr<IPpDpmHandler> &&ppDpmSclkHandler,
    std::unique_ptr<IPpDpmHandler> &&ppDpmMclkHandler) noexcept
: Control(true)
, id_(AMD::PMFixedFreq::ItemID)
, perfLevelDataSource_(std::move(perfLevelDataSource))
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
  ctlCmds.add({perfLevelDataSource_->source(), "manual"});

  ppDpmSclkHandler_->reset(ctlCmds);
  ppDpmMclkHandler_->reset(ctlCmds);
}

void AMD::PMFixedFreq::syncControl(ICommandQueue &ctlCmds)
{
  if (perfLevelDataSource_->read(dataSourceEntry_)) {

    if (dataSourceEntry_ != "manual") {
      ctlCmds.add({perfLevelDataSource_->source(), "manual"});

      ppDpmSclkHandler_->apply(ctlCmds);
      ppDpmMclkHandler_->apply(ctlCmds);
    }
    else {
      ppDpmSclkHandler_->sync(ctlCmds);
      ppDpmMclkHandler_->sync(ctlCmds);
    }
  }
}
