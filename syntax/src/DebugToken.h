/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXSYNTAX_DEBUGTOKEN_H
#define FLUXSYNTAX_DEBUGTOKEN_H

#include <flux/syntax/Token>

namespace flux { class Stream; }

namespace flux {
namespace syntax {

class DebugTokenFactory;

class DebugToken: public Token
{
    friend class DebugTokenFactory;

public:
    inline const char *scopeName() const { return scopeName_; }
    inline const char *ruleName() const { return ruleName_; }

    void printTo(Stream *stream, ByteArray *text, int depth = 0, int defaultScope = -1);

protected:
    DebugToken(int scope, int rule, const char *scopeName, const char *ruleName):
        Token(scope, rule),
        scopeName_(scopeName),
        ruleName_(ruleName)
    {}

private:
    const char *scopeName_;
    const char *ruleName_;
};

}} // namespace flux::syntax

#endif // FLUXSYNTAX_DEBUGTOKEN_H
