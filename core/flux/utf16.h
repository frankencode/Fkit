/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_UTF16_H
#define FLUX_UTF16_H

#include "errors.h"

namespace flux
{

namespace utf16
{

class EncodeSurrogatePairError: public EncodingError
{
public:
	~EncodeSurrogatePairError() throw() {}

	virtual String message() const {
		return "UTF-16 error: encoding surrogate pairs is not allowed (0xD800..0xDFFF)";
	}
};

class EncodeByteOrderMarkError: public EncodingError
{
public:
	~EncodeByteOrderMarkError() throw() {}

	virtual String message() const {
		return "UTF-16 error: encoding the Byte Order Mark is not allowed (0xFEFF, 0xFFFE)";
	}

};

class EncodeLargeCodePointError: public EncodingError
{
public:
	~EncodeLargeCodePointError() throw() {}

	virtual String message() const {
		return "UTF-16 error: code point to large (>= 0x110000)";
	}
};

class DecodeError: public EncodingError
{
public:
	~DecodeError() throw() {}

	virtual String message() const {
		return "UTF-16 error: failed to decode input bytes";
	}
};

inline int encodedSize(uchar_t ch)
{
	return 2 * (1 + (0xFFFF < ch));
}

} // namespace utf16

} // namespace flux

#endif // FLUX_UTF16_H
