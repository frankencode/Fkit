/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_BUNDLE_H
#define FLUX_BUNDLE_H

#include <flux/File>

namespace flux {

#define FLUX_QUOTE(name) #name

#define FLUX_BUNDLE_LOOKUP(relPath) \
    File::lookup(relPath, StringList::create() << FLUX_QUOTE(FLUXMAKE_BUNDLE_PREFIX) << String(__FILE__)->reducePath(), File::Exists)

} // namespace flux

#endif // FLUX_BUNDLE_H