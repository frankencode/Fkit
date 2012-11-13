/*
 * Utf16Decoder.hpp -- UTF-16 decoder
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_UTF16DECODER_HPP
#define FTL_UTF16DECODER_HPP

#include "ByteDecoder.hpp"

namespace ftl
{

/** \brief UTF-16 decoder
  *
  * A Byte-Order-Mark (BOM) at the start of the stream automatically sets
  * endianess. The decoder is designed to be as strict as possible. It checks
  * that the input streams delivers full 16-bit words and it requires high
  * surrogates to be followed by low surrogates.
  *
  * A 7-bit ASCII text is be decoded as UTF-16 without error if stream length is
  * a multiple of 2. Means there is a 50% chance of reading an 7-bit ASCII text
  * as UTF-16 without throwing an EncodingException.
  */
class Utf16Decoder: public Source<uchar_t>
{
public:
	inline static Ref<Utf16Decoder, Owner> open(Ref<Stream> stream, int bufCapa, int endian = BigEndian) {
		return new Utf16Decoder(stream, bufCapa, endian);
	}
	inline static Ref<Utf16Decoder, Owner> open(const void *buf, int bufCapa, int endian = BigEndian) {
		return new Utf16Decoder(buf, bufCapa, endian);
	}

	inline bool read(uchar_t *ch)
	{
		bool more = byteDecoder_->hasMore();
		if (more) {
			uint16_t w = byteDecoder_->readUInt16();
			if ((w == 0xFFFE) && (byteDecoder_->numBytesRead() == 0)) {
				byteDecoder_->setEndian((byteDecoder_->endian() == BigEndian) ? LittleEndian : BigEndian);
				w = byteDecoder_->readUInt16();
			}
			*ch = w;
			if ((0xD800 <= *ch) && (*ch < 0xDC00)) {
				uint16_t w = byteDecoder_->readUInt16();
				if (!((0xDC00 <= w) && (w < 0xE000)))
					FTL_THROW(EncodingException, "Input data is not conforming to UTF-16 encoding");
				*ch = 0x10000 + (((*ch - 0xD800) << 10) | (w - 0xDC00));
			}
			else if (0x10FFFF < *ch) {
				FTL_THROW(EncodingException, "Input data is not conforming to UTF-16 encoding");
			}
		}
		return more;
	}

	inline Ref<ByteDecoder> byteDecoder() const { return byteDecoder_; }

private:
	Utf16Decoder(Ref<Stream> stream, int bufCapa, int endian)
		: byteDecoder_(ByteDecoder::open(stream, bufCapa, endian))
	{}

	Utf16Decoder(const void *buf, int bufCapa, int endian)
		: byteDecoder_(ByteDecoder::open(buf, bufCapa, endian))
	{}

	Ref<ByteDecoder, Owner> byteDecoder_;
};

} // namespace ftl

#endif // FTL_UTF16DECODER_HPP
