/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXNODE_NODECONFIG_H
#define FLUXNODE_NODECONFIG_H

#include <flux/SocketAddress.h>
#include <flux/String.h>
#include <flux/Map.h>
#include "ServiceDefinition.h"
#include "LogConfig.h"

namespace fnode
{

using namespace flux;

class NodeConfig: public Object, public Singleton<NodeConfig>
{
public:
	void load(int argc, char **argv);

	inline String canoncialName() const { return canonicalName_; }
	inline SocketAddressList *address() const { return address_; }
	inline String user() const { return user_; }
	inline String version() const { return version_; }
	inline bool daemon() const { return daemon_; }

	inline LogConfig *errorLogConfig() const { return errorLogConfig_; }
	inline LogConfig *accessLogConfig() const { return accessLogConfig_; }

	inline ServiceInstances *serviceInstances() const { return serviceInstances_; }

private:
	friend class Singleton<NodeConfig>;

	NodeConfig();

	String canonicalName_;
	Ref<SocketAddressList> address_;
	String user_;
	String version_;
	bool daemon_;

	Ref<LogConfig> errorLogConfig_;
	Ref<LogConfig> accessLogConfig_;

	Ref<ServiceInstances> serviceInstances_;
};

inline NodeConfig *nodeConfig() { return NodeConfig::instance(); }

} // namespace fnode

#endif // FLUXNODE_NODECONFIG_H
