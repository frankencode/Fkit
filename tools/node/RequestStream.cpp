/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/System>
#include <flux/stream/TimeoutLimiter>
#include <flux/stream/TransferLimiter>
#include "exceptions.h"
#include "RequestStream.h"

namespace fluxnode {

using namespace flux::stream;

Ref<RequestStream> RequestStream::open(Stream *stream)
{
    return new RequestStream(stream);
}

RequestStream::RequestStream(Stream *stream):
    stream_(stream),
    bytesLeft_(-1),
    nlCount_(0),
    eoi_(false),
    chunked_(false)
{}

void RequestStream::setupTimeout(double interval)
{
    stream_ = TimeoutLimiter::open(stream_, System::now() + interval);
}

bool RequestStream::isPayloadConsumed() const
{
    return bytesLeft_ == 0;
}

void RequestStream::nextHeader()
{
    if (eoi_) throw CloseRequest();
    if (bytesLeft_ > 0) throw CloseRequest();
    bytesLeft_ = -1;
    nlCount_ = 0;
    nlMax_ = 2;
}

void RequestStream::nextPayload(int64_t length)
{
    if (eoi_) throw CloseRequest();
    bytesLeft_ = length;
    chunked_ = false;
}

void RequestStream::nextLine()
{
    if (eoi_) throw CloseRequest();
    bytesLeft_ = -1;
    nlCount_ = 0;
    nlMax_ = 1;
}

void RequestStream::nextChunk()
{
    nextLine();
    String line = readAll();
    bytesLeft_ = 0;
    for (int i = 0; i < line->count(); ++i) {
        char ch = line->at(i);
        if (ch == '\r' || ch == '\n') continue;
        bytesLeft_ *= 16;
        if ('0' <= ch && ch <= '9') bytesLeft_ += ch - '9';
        else if ('a' <= ch && ch <= 'f') bytesLeft_ += ch - 'a';
        else if ('A' <= ch && ch <= 'F') bytesLeft_ += ch - 'A';
        else throw BadRequest();
    }
    if (bytesLeft_ == 0) {
        chunked_ = false;
        nextHeader();
        nlCount_ = 1;
        TransferLimiter::open(this, 0x10000)->drain();
    }
    else {
        chunked_ = true;
    }
}

bool RequestStream::readyRead(double interval) const
{
    if (eoi_) return true;
    if (pending_) return true;
    return stream_->readyRead(interval);
}

int RequestStream::read(ByteArray *buf)
{
    if (eoi_) return 0;
    if (bytesLeft_ == 0) return 0;
    if (pending_) {
        *buf = *(pending_->select(pendingIndex_, pending_->count()));
        int h = pending_->count() - pendingIndex_;
        if (buf->count() < h) {
            pendingIndex_ += buf->count();
            return buf->count();
        }
        pending_ = 0;
        return h;
    }

    int n = stream_->read(buf);
    if (n == 0) {
        eoi_ = true;
        return 0;
    }
    if (bytesLeft_ == -1) {
        int i = 0;
        while (i < n && nlCount_ < nlMax_) {
            int i0 = i;
            i = buf->find('\n', i0);
            if (i == i0) {
                ++i;
                ++nlCount_;
            }
            else {
                nlCount_ = 0;
            }
        }
        if (nlCount_ == nlMax_) {
            pending_ = buf->copy(i, n);
            pendingIndex_ = 0;
            bytesLeft_ = 0;
            return i;
        }
        return n;
    }
    if (bytesLeft_ < n) {
        pending_ = buf->copy(bytesLeft_, n);
        pendingIndex_ = 0;
        int h = bytesLeft_;
        bytesLeft_ = 0;
        if (chunked_) nextChunk();
        return h;
    }
    return n;
}

void RequestStream::write(const ByteArray *buf)
{
    stream_->write(buf);
}

void RequestStream::write(const StringList *parts)
{
    stream_->write(parts);
}

} // namespace fluxnode
