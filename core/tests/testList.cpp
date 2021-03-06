/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/testing/TestSuite>
#include <flux/stdio>
#include <flux/Random>
#include <flux/List>

using namespace flux;
using namespace flux::testing;

template<class T>
void print(Ref< List<T> > list) {
    fout("[");
    for (int i = 0; i < list->count(); ++i) {
        fout() << list->at(i);
        if (i + 1 < list->count()) fout(", ");
    }
    fout("]\n");
}

typedef List<int> IntList;

class InsertionIteration: public TestCase
{
    void run() {
        Ref<IntList> list = IntList::create();
        for (int i = 0; i < 10; ++i)
            list->append(i);
        print(list);
        for (int i = 0; i < 10; ++i)
            FLUX_VERIFY(list->at(i) == i);
    }
};

class InsertionRemoval: public TestCase
{
    void run() {
        Ref<IntList> list = IntList::create();
        for (int i = 0; i < 10; ++i)
            list->append(i);
        print(list);
        for (int i = 0; i < list->count(); ++i)
            if (list->at(i) % 2 != 0) list->remove(i);
        print(list);
        for (int i = 0; i < 10; ++i) {
            if (i % 2 == 0)
                FLUX_VERIFY(list->popFront() == i);
        }
    }
};

class Sorting: public TestCase
{
    void run() {
        Ref<IntList> list = IntList::create();
        Ref<Random> random = Random::open();
        for (int i = 0; i < 10; ++i)
            list->append(random->get(0, 99));
        print(list);
        Ref<IntList> list2 = list->sort();
        print(list2);
        FLUX_VERIFY(list2->count() == list->count());
        for (int i = 0; i < list2->count() - 1; ++i)
            FLUX_VERIFY(list2->at(i) <= list2->at(i + 1));
        Ref<IntList> list3 = list->unique();
        print(list3);
        for (int i = 0; i < list3->count() - 1; ++i)
            FLUX_VERIFY(list3->at(i) < list3->at(i + 1));
    }
};

class Cloning: public TestCase
{
    void run() {
        Ref<Random> random = Random::open();
        Ref<IntList> a = IntList::create();
        for (int i = 0; i < 10; ++i)
            a->append(random->get(0, 99));
        Ref<IntList> b = IntList::clone(a);
        print(a);
        print(b);
        FLUX_VERIFY(*a == *b);
    }
};

class Preallocation: public TestCase
{
    void run() {
        Ref<IntList> a = IntList::create(11);
        print(a);
        FLUX_VERIFY(a->count() == 11);
    }
};

int main(int argc, char **argv)
{
    FLUX_TESTSUITE_ADD(InsertionIteration);
    FLUX_TESTSUITE_ADD(InsertionRemoval);
    FLUX_TESTSUITE_ADD(Sorting);
    FLUX_TESTSUITE_ADD(Cloning);
    FLUX_TESTSUITE_ADD(Preallocation);

    return testSuite()->run(argc, argv);
}
