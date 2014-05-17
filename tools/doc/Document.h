/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXDOC_DOCUMENT_H
#define FLUXDOC_DOCUMENT_H

#include "fragments.h"

namespace fluxdoc
{

using namespace flux;

class Document;
typedef List< Ref<Document> > DocumentList;
typedef List< Ref<Author> > AuthorList;

class Document: public Object
{
public:
	static Ref<Document> load(String path, Document *parent = 0);
	static Ref<Document> parse(String text, String path = "", Document *parent = 0);

	inline String path() const { return path_; }
	inline FragmentList *fragments() const { return fragments_; }

	inline Title *title() const { return title_; }
	inline AuthorList *authors() const { return authors_; }

	inline Document *parent() const { return parent_; }
	inline DocumentList *parts() const { return parts_; }

	int depth() const;

private:
	Document(FragmentList *fragments, String path, Document *parent = 0);

	String path_;
	Ref<FragmentList> fragments_;
	Ref<Title> title_;
	Ref<AuthorList> authors_;

	Document *parent_;
	Ref<DocumentList> parts_;
};

} // namespace fluxdoc

#endif // FLUXDOC_DOCUMENT_H