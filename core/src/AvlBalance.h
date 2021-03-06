/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_AVLBALANCE_H
#define FLUX_AVLBALANCE_H

#include <flux/BinaryNode>

namespace flux {

class AvlBalance
{
protected:
    virtual void setRoot(BinaryNode *k) = 0;

    inline virtual void rotated(BinaryNode *k1, bool left) {}

    void restore(BinaryNode *kp, BinaryNode*, bool left, bool attached);

private:
    BinaryNode *rebalance(BinaryNode *k1);
    void rotate(BinaryNode *k1, bool left);

public:
#ifndef NDEBUG
    static int height(BinaryNode *k);
    static bool testBalance(BinaryNode *k);
#endif
};

} // namespace flux

#endif // FLUX_AVLBALANCE_H
