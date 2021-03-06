/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_THREADLOCALSINGLETON_H
#define FLUX_THREADLOCALSINGLETON_H

#include <flux/Object>
#include <flux/Ref>
#include <flux/ThreadLocalRef>
#include <flux/LocalStatic>

namespace flux {

template<class SubClass>
class ThreadLocalSingleton
{
public:
    static SubClass *instance()
    {
        ThreadLocalRef<SubClass> &instance_ = localStatic< ThreadLocalRef<SubClass>, ThreadLocalSingleton<SubClass> >();
        if (!instance_)
            instance_ = new SubClass;
        return instance_;
    }
};

} // namespace flux

#endif // FLUX_THREADLOCALSINGLETON_H
