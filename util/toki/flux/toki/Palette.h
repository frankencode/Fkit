/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXTOKI_PALETTE_H
#define FLUXTOKI_PALETTE_H

#include <flux/SyntaxDefinition.h>
#include <flux/MetaObject.h>
#include "Style.h"

namespace flux {
namespace toki {

using namespace yason;

class Palette: public MetaObject
{
public:
	static Ref<Palette> create() { return new Palette; }
	static Ref<Palette> load(String path);

	inline String scopeName() const { return scopeName_; }
	inline int scope() const { return scope_; }

	static int defaultScope();

	enum DefaultRule {
		Text,
		CurrentLine,
		Cursor,
		Selection,
		Match,
		LineNumber,
		CurrentLineNumber,
		Undefined = -1
	};

	inline Style *styleByRule(int rule) { return styleByRule_->value(rule); }

	inline int styleCount() const { return styleByRule_->count(); }
	inline const Style *styleAt(int i) const { return styleByRule_->valueAt(i); }

private:
	Palette();

	static int defaultRuleByName(String name);

	virtual Ref<MetaObject> produce();
	virtual void define();
	virtual void realize(const ByteArray *text, Token *objectToken);

	String scopeName_;
	int scope_;

	typedef Map<int, Ref<Style> > StyleByRule;
	Ref<StyleByRule> styleByRule_;
};

}} // namespace flux::toki

#endif // FLUXTOKI_PALETTE_H
