/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_DIRWALKER_H
#define FLUX_DIRWALKER_H

#include <flux/String>

namespace flux {

class Dir;

/** \brief Recursive directory tree walker
  * \see Dir
  */
class DirWalker: public Source<String>
{
public:
    static Ref<DirWalker> open(String path);
    static Ref<DirWalker> tryOpen(String path);

    inline int maxDepth() const { return maxDepth_; }
    inline void setMaxDepth(int depth) { maxDepth_ = depth; }

    inline bool ignoreHidden() const { return ignoreHidden_; }
    inline void setIgnoreHidden(bool on) { ignoreHidden_ = on; }

    inline bool followSymlink() const { return followSymlink_; }
    inline void setFollowSymlink(bool on) { followSymlink_ = on; }

    inline bool deleteOrder() const { return deleteOrder_; }
    inline void setDeleteOrder(bool on) { deleteOrder_ = on; }

    bool read(String *path, bool *isDir);
    bool read(String *path) { return read(path, 0); }

private:
    DirWalker(String path, Dir *dir = 0);
    int maxDepth_;
    bool ignoreHidden_;
    bool followSymlink_;
    bool deleteOrder_;
    int depth_;
    Ref<Dir> dir_;
    Ref<DirWalker> child_;
};

} // namespace flux

#endif // FLUX_DIRWALKER_H
