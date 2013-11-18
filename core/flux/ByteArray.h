/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_BYTEARRAY_H
#define FLUX_BYTEARRAY_H

#include "containers.h"
#include "strings.h"
#include "Default.h"

namespace flux
{

class String;
template<class T> class List;
typedef List<String> StringList;

namespace syntax { class Definition; }
typedef syntax::Definition SyntaxDefinition;

class File;

class ByteArray: public Object
{
public:
	typedef char Item;

	inline static Ref<ByteArray> create(int size = 0) { return new ByteArray(size); }
	inline static Ref<ByteArray> create(int size, char zero) { return new ByteArray(size, zero); }
	inline static Ref<ByteArray> copy(const char *data, int size = -1) { return new ByteArray(data, size); }
	inline static Ref<ByteArray> wrap(void *data, int size) { return new ByteArray(data, size); }
	static Ref<ByteArray> join(const StringList *parts, const char *sep = "");
	static Ref<ByteArray> join(const StringList *parts, char sep);
	static Ref<ByteArray> join(const StringList *parts, String sep);

	~ByteArray();

	inline static ByteArray *empty() { return Default<ByteArray>::instance(); }

	void resize(int newSize);

	ByteArray &operator=(const ByteArray &b);
	ByteArray &operator^=(const ByteArray &b);

	void clear(char zero = '\0');

	inline int size() const { return size_; }

	inline int first() const { return 0; }
	inline int last() const { return size_ - 1; }

	inline bool has(int i) const {
		return (0 <= i) && (i < size_);
	}

	inline char *pointerAt(int i) const {
		FLUX_ASSERT(has(i));
		return data_ + i;
	}

	inline char &at(int i) const {
		FLUX_ASSERT(has(i));
		return data_[i];
	}
	inline uint8_t &byteAt(int i) const {
		FLUX_ASSERT(has(i));
		return bytes_[i];
	}
	inline uint32_t &wordAt(int j) const {
		FLUX_ASSERT(has(j * 4) && has(j * 4 + 3));
		return words_[j];
	}

	inline char *data() const { return data_; }
	inline uint8_t *bytes() const { return reinterpret_cast<uint8_t *>(data_); }
	inline const char *constData() const { return data_; }
	inline operator char*() const { return data_; }

	inline void read(int i, char *data, int size) {
		if (size == 0) return;
		FLUX_ASSERT(has(i));
		FLUX_ASSERT(has(i + size - 1));
		memcpy(data, data_ + i, size);
	}

	inline void write(int i, const char *data, int size) {
		if (size == 0) return;
		FLUX_ASSERT(has(i));
		FLUX_ASSERT(has(i + size - 1));
		memcpy(data_ + i, data, size);
	}

	inline Ref<ByteArray> copy() const { return new ByteArray(*this); }

	inline Ref<ByteArray> copy(int i0, int i1) const {
		if (i0 < 0) i0 = 0;
		if (i0 > size_) i0 = size_;
		if (i1 < 0) i1 = 0;
		if (i1 > size_) i1 = size_;
		return (i0 < i1) ? new ByteArray(data_ + i0, i1 - i0) : new ByteArray();
	}

	template<class Range>
	inline Ref<ByteArray> copy(Range *range) const {
		return copy(range->i0(), range->i1());
	}

	inline Ref<ByteArray> head(int n) const { return copy(0, n); }
	inline Ref<ByteArray> tail(int n) const { return copy(size_ - n, size_); }

	inline int find(char ch) const { return find(ch, 0); }
	inline int find(char ch, int i) const {
		if (i < 0) i = 0;
		while (i < size_) {
			if (data_[i] == ch) break;
			++i;
		}
		return i;
	}

	inline bool contains(char ch) const { return find(ch) < size_; }
	inline int count(char ch) const {
		int n = 0;
		for (char *p = data_; *p; ++p) n += (*p == ch);
		return n;
	}

	inline int replace(char oldItem, char newItem) {
		int n = 0;
		for (int i = 0; i < size_; ++i) {
			if (data_[i] == oldItem) {
				data_[i] = newItem;
				++n;
			}
		}
		return n;
	}

	int find(const char *pattern, int i = 0) const;
	int find(String pattern, int i = 0) const;
	int find(SyntaxDefinition *pattern, int i = 0) const;

	inline bool contains(const char *pattern) const { return find(pattern) != size_; }
	bool contains(String pattern) const;

	Ref<StringList> split(char sep) const;
	Ref<StringList> split(const char *sep) const;
	Ref<StringList> split(SyntaxDefinition *pattern) const;
	Ref<StringList> breakUp(int chunkSize) const;

	ByteArray *replaceInsitu(const char *pattern, const char *replacement);
	Ref<ByteArray> replace(const char *pattern, const char *replacement) const;
	Ref<ByteArray> replace(String pattern, String replacement) const;

	int toInt(bool *ok = 0) const;
	double toFloat(bool *ok = 0) const;
	int64_t toInt64(bool *ok = 0) const;
	uint64_t toUInt64(bool *ok = 0) const;
	float64_t toFloat64(bool *ok = 0) const;

	int scanString(String *x, const char *term = " \t\n", int i0 = 0, int i1 = -1) const;

	template<class T>
	int scanInt(T *x, int base = 10, int i0 = 0, int i1 = -1) const;

	inline Ref<ByteArray> downcase() const { return copy()->downcaseInsitu(); }
	inline Ref<ByteArray> upcase() const { return copy()->upcaseInsitu(); }
	ByteArray *downcaseInsitu();
	ByteArray *upcaseInsitu();

	inline Ref<ByteArray> expand() const { return copy()->expandInsitu(); }
	Ref<ByteArray> escape() const;
	ByteArray *expandInsitu();

	ByteArray *truncate(int i1);
	ByteArray *truncate(int i0, int i1);

	inline Ref<ByteArray> trim(const char *space = " \t\n\r") const { return copy()->trimInsitu(space); }
	ByteArray *trimInsitu(const char *space = " \t\n\r");
	Ref<ByteArray> stripTags() const;
	Ref<ByteArray> simplify() const;
	Ref<ByteArray> normalize(bool nameCase = true) const;

	bool offsetToLinePos(int offset, int *line = 0, int *pos = 0) const;
	bool linePosToOffset(int line, int pos, int *offset = 0) const;

	void checkUtf8() const;

	static Ref<ByteArray> fromUtf16(ByteArray *utf16, int endian = localEndian());
	bool toUtf16(void *buf, int *size);
	Ref<ByteArray> toUtf16(int endian = localEndian());

	Ref<ByteArray> hex() const;

	bool isRootPath() const;
	bool isRelativePath() const;
	bool isAbsolutePath() const;

	Ref<ByteArray> absolutePathRelativeTo(String currentDir) const;
	Ref<ByteArray> absolutePath() const;
	Ref<ByteArray> fileName() const;
	Ref<ByteArray> baseName(bool complete = true) const;
	Ref<ByteArray> suffix(bool complete = false) const;
	Ref<ByteArray> reducePath() const;
	Ref<ByteArray> expandPath(String component) const;
	Ref<ByteArray> canonicalPath() const;

private:
	friend class Singleton<ByteArray>;
	friend class File;
	friend class ByteRange;

	explicit ByteArray(int size = 0, char zero = 0);
	ByteArray(const char *data, int size);
	ByteArray(char *data, int size, size_t mapSize);
	ByteArray(void *data, int size);
	ByteArray(const ByteArray &b);
	ByteArray(ByteArray *b, int size);
	void destroy();

	int size_, origSize_;
	union {
		char *data_;
		uint8_t *bytes_;
		uint32_t *words_;
	};
	char *origData_;
	size_t mapSize_;
	bool wrapped_;
	#ifndef NDEBUG
	int rangeCount_;
	#endif
};

class ByteRange
{
public:
	ByteRange(ByteArray *array, int i0, int i1)
		: array_(array),
		  origData_(array->data_),
		  origSize_(array->size_),
		  origEnd_(0)
	{
		if (i0 < 0) i0 = 0;
		if (i0 > origSize_) i0 = origSize_;
		if (i1 < i0) i1 = i0;
		if (i1 > origSize_) i1 = origSize_;
		origEnd_ = origData_[i1];
		origData_[i1] = 0;
		array_->data_ = origData_ + i0;
		array_->size_ = i1 - i0;
		#ifndef NDEBUG
		++array_->rangeCount_;
		#endif
	}

	~ByteRange() {
		array_->data_[array_->size_] = origEnd_;
		array_->data_ = origData_;
		array_->size_ = origSize_;
		#ifndef NDEBUG
		--array_->rangeCount_;
		#endif
		array_ = 0;
	}

	inline operator ByteArray*() const { return array_; }
	inline ByteArray *operator*() const { return array_; } // FIXME, better return "ByteArray &" ?
	inline ByteArray *operator->() const { return array_; }
	operator String() const;

private:
	// ByteRange(const ByteRange&);
	ByteRange &operator=(const ByteRange &b);
	void *operator new(size_t size);
	void operator delete(void *data, size_t size);

	Ref<ByteArray> array_;
	char *origData_;
	int origSize_;
	char origEnd_;
};

template<class T>
int ByteArray::scanInt(T *x, int base, int i0, int i1) const
{
	int i = i0;
	if (i1 < 0) i1 = size_;
	if (i > i1) i = i1;
	*x = T();
	bool minus = false;
	if (T(-1) < T() && i < i1) {
		if (at(i) == '-') {
			minus = true;
			++i;
		}
		else if (at(i) == '+') {
			minus = false;
			++i;
		}
	}
	while (i < i1) {
		char ch = at(i);
		int z = -1;
		if ('0' <= ch && ch <= '9') z = ch - '0';
		else if ('a' <= ch && ch <= 'z') z = ch - 'a';
		else if ('A' <= ch && ch <= 'Z') z = ch - 'A';
		if (z < 0 || base <= z) break;
		*x *= base;
		*x += z;
		++i;
	}
	return i;
}

inline bool operator==(const ByteArray &a, const ByteArray &b) { return container::compare(a, b) == 0; }
inline bool operator!=(const ByteArray &a, const ByteArray &b) { return container::compare(a, b) != 0; }
inline bool operator< (const ByteArray &a, const ByteArray &b) { return container::compare(a, b) <  0; }
inline bool operator> (const ByteArray &a, const ByteArray &b) { return container::compare(a, b) >  0; }
inline bool operator<=(const ByteArray &a, const ByteArray &b) { return container::compare(a, b) <= 0; }
inline bool operator>=(const ByteArray &a, const ByteArray &b) { return container::compare(a, b) >= 0; }

} // namespace flux

#endif // FLUX_BYTEARRAY_H