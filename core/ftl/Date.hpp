/*
 * Date.hpp -- broken down calendar times
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_DATE_HPP
#define FTL_DATE_HPP

#include "atoms"
#include "Time.hpp"
#include "String.hpp"

namespace ftl
{

typedef struct tm StructTm;

/** \brief broken-down time
  * \see Time
  */
class Date: public StructTm, public Instance
{
public:
	enum Weekday {
		Sun = 0,
		Mon = 1,
		Tue = 2,
		Wed = 3,
		Thu = 4,
		Fri = 5,
		Sat = 6
	};

	inline static Ref<Date, Owner> create() {
		return new Date;
	}
	inline static Ref<Date, Owner> create(Time time) {
		return new Date(time);
	}

	// \todo
	// improve logic (add date validation, bool validated_, etc...)
	inline bool valid() const { return tm_off != -2;}

	static Ref<Date, Owner> localTime();
	static Ref<Date, Owner> localTime(Time time);

	inline int year() const { return tm_year + 1900; }
	inline int month() const { return tm_mon; }
	inline int day() const { return tm_mday; }
	inline int weekday() const { return tm_wday; }
	inline int yearday() const { return tm_yday; }
	inline int hour() const { return tm_hour; }
	inline int minute() const { return tm_min; }
	inline int second() const { return tm_sec; }
	inline int offset() const { return tm_off; }

	Time time() const;

	// \todo
	String toString() const;
	// static Ref<Date, Owner> fromString(String s);

	// \todo
	// local time formatting

private:
	Date();
	Date(Time time);

	// \todo
	// Date(String s);
	// Date(int year, int month, int day, int hour = 0, int min = 0, int sec = 0);

	void clear();
	void init(Time time);

	// \todo: replace by a unixDay(;)
	static int julianDay(int year, int month, int day);
	int tm_off;
	// \todo: fully enable ms resolution (ss.fraction in string output)
	// int tm_ms;
};

} // namespace ftl

#endif // FTL_DATE_HPP