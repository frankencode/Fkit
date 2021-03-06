/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include "BuildPlan.h"
#include "InstallStage.h"

namespace fluxmake {

bool InstallStage::run()
{
    if (complete_) return success_;
    complete_ = true;

    if (outOfScope()) return success_ = true;
    if (plan()->options() & BuildPlan::Test) return success_ = true;

    for (int i = 0; i < plan()->prerequisites()->count(); ++i) {
        if (!plan()->prerequisites()->at(i)->installStage()->run())
            return success_ = false;
    }

    if (plan()->options() & BuildPlan::Package) return success_ = true;

    if (plan()->options() & BuildPlan::Tools) {
        for (int i = 0; i < plan()->modules()->count(); ++i) {
            if (!toolChain()->install(plan(), plan()->modules()->at(i)))
                return success_ = false;
        }
        return success_ = true;
    }

    return success_ = toolChain()->install(plan());
}

} // namespace fluxmake
