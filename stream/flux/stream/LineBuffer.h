/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXSTREAM_LINEBUFFER_H
#define FLUXSTREAM_LINEBUFFER_H

#include <flux/Stream.h>

namespace flux {
namespace stream {

class LineBuffer: public Stream
{
public:
	static Ref<LineBuffer> open(Stream *stream, String prefix = "");

	inline Stream *stream() const { return stream_; }
	virtual String prefix() const;

	virtual bool readyRead(double interval) const;
	virtual int read(ByteArray *buf);

	virtual void write(const ByteArray *buf);
	virtual void write(const StringList *parts);

protected:
	LineBuffer(Stream *stream, String prefix = "");

private:
	Ref<Stream> stream_;
	String prefix_;
	Ref<StringList> backlog_;
};

}} // namespace flux::stream

#endif // FLUXSTREAM_LINEBUFFER_H