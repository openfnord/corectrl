// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2019 Juan Palacios <jpalaciosdev@gmail.com>

#include "ppdpmhandler.h"

#include "core/components/amdutils.h"
#include "core/icommandqueue.h"
#include "core/idatasource.h"
#include <algorithm>
#include <iterator>

AMD::PpDpmHandler::PpDpmHandler(
    std::unique_ptr<IDataSource<std::string>> &&perfLevelDataSource,
    std::unique_ptr<IDataSource<std::vector<std::string>>> &&ppDpmDataSource) noexcept
: perfLevelDataSource_(std::move(perfLevelDataSource))
, ppDpmDataSource_(std::move(ppDpmDataSource))
, resync_(true)
{
  if (ppDpmDataSource_->read(ppDpmLines_)) {
    auto states = Utils::AMD::parseDPMStates(ppDpmLines_);
    if (states.has_value())
      std::swap(states_, states.value());

    active_.reserve(states_.size());
    std::transform(states_.cbegin(), states_.cend(), std::back_inserter(active_),
                   [](auto &state) { return state.first; });
  }
}

std::vector<std::pair<unsigned int, units::frequency::megahertz_t>> const &
AMD::PpDpmHandler::states() const
{
  return states_;
}

std::vector<unsigned int> const &AMD::PpDpmHandler::active() const
{
  return active_;
}

void AMD::PpDpmHandler::activate(std::vector<unsigned int> const &states)
{
  std::vector<unsigned int> active;
  std::copy_if(states.cbegin(), states.cend(), std::back_inserter(active),
               [&](unsigned int index) {
                 // skip unknown state indices
                 return std::find_if(states_.cbegin(), states_.cend(),
                                     [&](auto &state) {
                                       return state.first == index;
                                     }) != states_.cend();
               });

  if (!active.empty()) { // cannot deactivate all states!
    std::swap(active, active_);
    resync_ = true;
  }
}

void AMD::PpDpmHandler::saveState()
{
  // NOTE At the moment, there is no way to get the active states
  // from pp_dpm_* files. The data source state cannot be saved.
}

void AMD::PpDpmHandler::restoreState(ICommandQueue &)
{
  // NOTE At the moment, there is no way to get the active states
  // from pp_dpm_* files. The data source state cannot be restored.
}

void AMD::PpDpmHandler::reset(ICommandQueue &ctlCmds)
{
  std::string activeStatesStr;
  for (auto &state : states_)
    activeStatesStr.append(std::to_string(state.first)).append(" ");
  activeStatesStr.pop_back(); // remove trailing space

  if (perfLevelDataSource_->read(perfLevelValue_) &&
      perfLevelValue_ != "manual")
    ctlCmds.add({perfLevelDataSource_->source(), "manual"});
  ctlCmds.add({ppDpmDataSource_->source(), activeStatesStr});

  resync_ = false;
}

void AMD::PpDpmHandler::sync(ICommandQueue &ctlCmds)
{
  // NOTE As there is no way to obtain a list of active states from
  // pp_dpm_* files, we cannot control external changes of the file
  // when the external change selected one or more states within the
  // current active states group of this handler.
  // Also, we need to keep track manually when is necessary a re-sync.

  if (perfLevelDataSource_->read(perfLevelValue_) &&
      ppDpmDataSource_->read(ppDpmLines_)) {

    if (perfLevelValue_ != "manual") {
      apply(ctlCmds); // apply it unconditionally
    }
    else {
      auto currentIndex = Utils::AMD::parseDPMCurrentStateIndex(ppDpmLines_);
      if (currentIndex.has_value()) {
        if (resync_ || // modified: needs syncing
            std::find( // current index is not an active index
                active_.cbegin(), active_.cend(), *currentIndex) ==
                active_.cend())
          apply(ctlCmds);
      }
    }
  }
}

void AMD::PpDpmHandler::apply(ICommandQueue &ctlCmds)
{
  std::string activeStatesStr;
  for (auto &index : active_)
    activeStatesStr.append(std::to_string(index)).append(" ");
  activeStatesStr.pop_back(); // remove trailing space

  if (perfLevelValue_ != "manual")
    ctlCmds.add({perfLevelDataSource_->source(), "manual"});
  ctlCmds.add({ppDpmDataSource_->source(), activeStatesStr});

  resync_ = false;
}
