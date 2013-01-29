 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_PROCESSSTATUS_HPP
#define FTL_PROCESSSTATUS_HPP

#include <sys/types.h>
#include "atoms"
#include "string.hpp"

namespace ftl
{

class ProcessStatus: public Instance
{
public:
	ProcessStatus(pid_t processId);
	
	pid_t processId() const;
	pid_t parentProcessId() const;
	gid_t processGroupId() const;
	gid_t foregroundProcessGroupId() const;
	// string terminalName() const;
	string loginName() const;
	string commandName() const;
	char processStatus() const;
	
private:
	pid_t processId_;
	pid_t parentProcessId_;
	gid_t processGroupId_;
	gid_t foregroundProcessGroupId_;
	// string terminalName_;
	string loginName_;
	string commandName_;
	char processStatus_;
};

} // namespace ftl

#endif // FTL_PROCESSSTATUS_HPP
