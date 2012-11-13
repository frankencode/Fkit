/*
 * Config.hpp -- application configuration
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_CONFIG_HPP
#define FTL_CONFIG_HPP

#include "Wire.hpp"

namespace ftl
{

FTL_STD_EXCEPTION(ConfigException);

class Config: public WireObject
{
public:
	static Ref<Config, Owner> create();

	void read(String path);
	void read(int argc, char **argv);

	Ref<StringList> options() const;
	Ref<StringList> arguments() const;
	String path() const;

	inline bool flag(const char *name) {
		bool h = false;
		lookup(name, &h);
		return h;
	}

private:
	Config() {}

	Ref<WireObject, Owner> object_;
	Ref<StringList, Owner> options_;
	Ref<StringList, Owner> arguments_;
	String path_;
};

} // namespace ftl

#endif // FTL_CONFIG_HPP
