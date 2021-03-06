/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/stdio>
#include <flux/FileStatus>
#include <flux/regexp/Glob>
#include "BuildPlan.h"
#include "JobScheduler.h"
#include "PreparationStage.h"

namespace fluxmake {

using namespace flux::regexp;

bool PreparationStage::run()
{
    if (complete_) return success_;
    complete_ = true;

    if (plan()->options() & BuildPlan::Test) {
        if (!(plan()->options() & BuildPlan::BuildTests))
            return success_ = true;
    }

    if (outOfScope()) return success_ = true;

    for (int i = 0; i < plan()->prerequisites()->count(); ++i)
        if (!plan()->prerequisites()->at(i)->preparationStage()->run()) return success_ = false;

    if (!plan()->predicates()) return success_ = true;

    for (int i = 0; i < plan()->predicates()->count(); ++i)
    {
        Predicate *predicate = plan()->predicates()->at(i);
        Ref<JobScheduler> scheduler;

        if (predicate->source()->count() == 0) {
            String targetPath = plan()->sourcePath(predicate->target()->replace("%", ""));
            if (!FileStatus::read(targetPath)->exists()) {
                String command = expand(predicate->create(), "", targetPath);
                if (!scheduler) {
                    scheduler = createScheduler();
                    scheduler->start();
                }
                scheduler->schedule(Job::create(command));
            }
        }

        for (int j = 0; j < predicate->source()->count(); ++j) {
            String sourceExpression =
                plan()->sourcePath(
                    predicate->source()->at(j)->replace("%", "(?@*)")
                );
            RegExp sourcePattern = sourceExpression;
            Ref<Glob> glob = Glob::open(sourceExpression);
            for (String sourcePath; glob->read(&sourcePath);) {
                String name;
                if (predicate->source()->at(j)->contains('%')) {
                    Ref<SyntaxState> state = sourcePattern->match(sourcePath);
                    name = sourcePath->copy(state->capture());
                }
                else {
                    name = sourcePath->baseName();
                }
                String targetPath =
                    plan()->sourcePath(
                        predicate->target()->replace("%", name)
                    );
                if (FileStatus::read(targetPath)->lastModified() < FileStatus::read(sourcePath)->lastModified()) {
                    String command = expand(predicate->update(), sourcePath, targetPath);
                    if (!scheduler) {
                        scheduler = createScheduler();
                        scheduler->start();
                    }
                    scheduler->schedule(Job::create(command));
                }
            }
        }

        if (predicate->remove() != "") {
            String targetExpression =
                plan()->sourcePath(
                    predicate->target()->replace("%", "(?@*)")
                );
            RegExp targetPattern = targetExpression;
            Ref<Glob> glob = Glob::open(targetExpression);
            for (String targetPath; glob->read(&targetPath);) {
                String name;
                if (predicate->target()->contains('%')) {
                    Ref<SyntaxState> state = targetPattern->match(targetPath);
                    name = targetPath->copy(state->capture());
                }
                else {
                    name = targetPath->baseName();
                }
                bool sourceFound = false;
                for (int j = 0; j < predicate->source()->count(); ++j) {
                    String sourcePath =
                        plan()->sourcePath(
                            predicate->source()->at(j)->replace("%", name)
                        );
                    if (FileStatus::read(sourcePath)->exists()) {
                        sourceFound = true;
                        break;
                    }
                }
                if (!sourceFound) {
                    String command = expand(predicate->remove(), "", targetPath);
                    if (!scheduler) {
                        scheduler = createScheduler();
                        scheduler->start();
                    }
                    scheduler->schedule(Job::create(command));
                }
            }
        }

        if (scheduler) {
            for (Ref<Job> job; scheduler->collect(&job);) {
                fout() << shell()->beautify(job->command()) << nl;
                if (job->status() != 0) {
                    ferr() << job->outputText();
                    status_ = job->status();
                    return success_ = false;
                }
            }
        }
    }

    return success_ = true;
}

String PreparationStage::expand(String command, String sourcePath, String targetPath)
{
    return command
        ->replace("$<", sourcePath)
        ->replace("$@", targetPath)
        ->replace("$SOURCE", sourcePath)
        ->replace("$TARGET", targetPath);
}

} // namespace fluxmake
