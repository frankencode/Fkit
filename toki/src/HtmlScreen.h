/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXTOKI_HTMLSCREEN_H
#define FLUXTOKI_HTMLSCREEN_H

#include <flux/Format>
#include <flux/syntax/Token>

namespace flux {
namespace toki {

using namespace flux::syntax;

class Theme;

class HtmlScreen: public Object, public TokenScreen
{
public:
    static Ref<HtmlScreen> create(String text, Format sink = Format()) { return new HtmlScreen(text, sink); }

    virtual bool project(Token *token, int i0, int i1);

    static void writeCss(Theme *theme, Stream *sink = 0);

private:
    HtmlScreen(String text, Format sink);
    ~HtmlScreen();

    void writeLineNumbers();

    String text_;
    Format sink_;
};

}} // namespace flux::toki

#endif // FLUXTOKI_HTMLSCREEN_H
