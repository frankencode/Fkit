/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/Singleton.h>
#include "Registry.h"

namespace fluxclaim
{

Registry::Registry()
	: headerScanners_(HeaderScannerList::create())
{}

const HeaderScannerList *Registry::headerScanners() const
{
	return headerScanners_;
}

void Registry::registerHeaderScanner(HeaderScanner *scanner)
{
	headerScanners_->append(scanner);
}

Registry *registry() { return Singleton<Registry>::instance(); }

} // namespace fluxclaim