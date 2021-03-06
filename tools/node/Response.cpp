/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/System>
#include <flux/Date>
#include <flux/stream/TransferMeter>
#include "utils.h"
#include "NodeConfig.h"
#include "ClientConnection.h"
#include "ChunkedSink.h"
#include "Response.h"

namespace fluxnode {

using namespace flux::stream;

Ref<Response> Response::create(ClientConnection *client)
{
    return new Response(client);
}

Response::Response(ClientConnection *client):
    client_(client),
    headerWritten_(false),
    statusCode_(200),
    contentLength_(-1),
    bytesWritten_(0),
    reasonPhrase_("OK")
{}

void Response::status(int statusCode, String reasonPhrase)
{
    statusCode_ = statusCode;
    reasonPhrase_ = reasonPhrase;
    if (reasonPhrase_ == "") reasonPhrase_ = reasonPhraseByStatusCode(statusCode_);
}

void Response::header(String name, String value)
{
    insert(name, value);
}

void Response::writeHeader()
{
    String now = formatDate(Date::create(System::now()));
    insert("Server", nodeConfig()->version());
    insert("Date", now);
    // insert("Connection", "keep-alive");
    if (statusCode_ != 304) {
        if (contentLength_ >= 0) {
            remove("Transfer-Encoding");
            establish("Content-Length", str(contentLength_));
        }
        else {
            establish("Transfer-Encoding", "chunked");
        }
        insert("Last-Modified", now);
    }

    Format header(client_->stream());
    header << "HTTP/1.1 " << statusCode_ << " " << reasonPhrase_ << "\r\n";
    for (int i = 0; i < count(); ++i)
        header << keyAt(i) << ":" << valueAt(i) << "\r\n";
    header << "\r\n";

    headerWritten_ = true;
}

void Response::begin(ssize_t contentLength)
{
    if (!headerWritten_) {
        contentLength_ = contentLength;
        writeHeader();
    }
}

Stream *Response::payload()
{
    if (!payload_) {
        if (!headerWritten_) writeHeader();
        Ref<Stream> stream = client_->stream();
        if (contentLength_ < 0) {
            stream = ChunkedSink::open(stream);
        }
        payload_ = TransferMeter::open(stream);
    }
    return payload_;
}

size_t Response::bytesWritten() const
{
    return (payload_) ? payload_->totalWritten() : bytesWritten_;
}

void Response::write(String bytes)
{
    payload()->write(bytes);
}

Format Response::chunk(String pattern)
{
    return Format(pattern, payload());
}

Format Response::chunk()
{
    return Format(payload());
}

void Response::end()
{
    if (payload_) {
        bytesWritten_ = payload_->totalWritten();
        payload_ = 0;
    }
}

} // namespace fluxnode
