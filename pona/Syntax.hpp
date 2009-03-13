/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstraction Library.
**
** Copyright (C) 2007-2009  Frank Mertens
**
** This file is part of a free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License as published
** by the Free Software Foundation, either version 3 of the License,
** or (at your option) any later version.
**
** The library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this libary.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef PONA_SYNTAX_HPP
#define PONA_SYNTAX_HPP

#include "atoms"
#include "Vector.hpp"
#include "PrefixTree.hpp"
#include "Token.hpp"
#include "TokenFactory.hpp"
#include "SyntaxState.hpp"

namespace pona
{

PONA_EXCEPTION(SyntaxException, Exception);

template<class Media>
class Syntax: public Instance
{
protected:
	typedef typename Media::Element Char;
	typedef Syntax Node;
	typedef PrefixTree<Char,int> KeywordMap;
	
	Syntax(Ref<Node> next = 0)
		: next_(next)
	{}
	
	Ref<Node, Owner> next_;
	
public:
	typedef SyntaxState<Char> State;
	
protected:
	virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state) = 0;
	
	class CharNode: public Node
	{
	public:
		CharNode(Char ch, int invert)
			: ch_(ch),
			  invert_(invert)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			if (i < media->length())
			{
				Char ch = media->get(i++);
				if ((ch != ch_) ^ invert_)
					i = -1;
			}
			else
				i = -1;
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
		
	private:
		Char ch_;
		int invert_;
	};
	
	class AnyNode: public Node
	{
	public:
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			if (i < media->length())
			{
				++i;
			}
			else
				i = -1;
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
	};
	
	class RangeNode: public Node
	{
	public:
		RangeNode(Char a, Char b, int invert)
			: a_(a),
			  b_(b),
			  s_(0),
			  invert_(invert)
		{}
		
		template<class Char2>
		RangeNode(const Char2* s, int len, int invert)
			: s_(s, len),
			  invert_(invert)
		{}
		
		template<class Char2>
		RangeNode(const Char2* s, int invert)
			: s_(s),
			  invert_(invert)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			if (i < media->length())
			{
				Char ch = media->get(i++);
				if (s_) {
					int k = 0, len = s_.length();
					while (k < len) {
						if (s_.get(k) == ch) break;
						++k;
					}
					if ((k == len) ^ invert_)
						i = -1;
				}
				else {
					if (((ch < a_) || (b_ < ch)) ^ invert_)
						i = -1;
				}
			}
			else
				i = -1;
				
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
	
	private:
		Char a_, b_;
		Vector<Char> s_;
		int invert_;
	};
	
	class StringNode: public Node
	{
	public:
		template<class Char2>
		StringNode(const Char2* s, int len)
			: s_(s, len)
		{}
		
		template<class Char2>
		StringNode(const Char2* s)
			: s_(s)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			int n = media->length();
			
			if (i < n) {
				int k = 0, len = s_.length();
				while ((k < len) && (i < n)) {
					Char ch = media->get(i++);
					if (s_.get(k) != ch) break;
					++k;
				}
				if (k != len)
					i = -1;
			}
			else
				i = -1;
				
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
	
	private:
		Vector<Char> s_;
	};
	
	class KeywordNode: public Node
	{
	public:
		KeywordNode(Ref<KeywordMap> map)
			: map_(map)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			if (i < media->length()) {
				int h = 0;
				int tokenType = -1;
				if (map_->lookup(media, i, &h, &tokenType)) {
					if (parentToken)
						parentToken->setType(tokenType);
					i = h;
				}
				else
					i = -1;
			}
			else
				i = -1;
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
	
	private:
		Ref<KeywordMap, Owner> map_;
	};
	
	class RepeatNode: public Node
	{
	public:
		RepeatNode(int minRepeat, int maxRepeat, Ref<Node> entry)
			: minRepeat_(minRepeat),
			  maxRepeat_(maxRepeat),
			  entry_(entry)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			int repeatCount = 0;
			int h = i;
			while ((repeatCount < maxRepeat_) && (h != -1))
			{
				h = entry_->matchNext(media, h, tokenFactory, parentToken, state);
				if (h != -1) {
					i = h;
					++repeatCount;
				}
			}
			if ((repeatCount < minRepeat_) || (maxRepeat_ < repeatCount))
				i = -1;
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
	
	private:
		int minRepeat_;
		int maxRepeat_;
		Ref<Node, Owner> entry_;
	};
	
	class BoiNode: public Node
	{
	public:
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			if (i != 0)
				i = -1;
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
	};
	
	class EoiNode: public Node
	{
	public:
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			if (i != media->length())
				i = -1;
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
	};
	
	class FindNode: public Node
	{
	public:
		FindNode(Ref<Node> entry)
			: entry_(entry)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			bool found = false;
			int n = media->length();
			while (i <= n) {
				int h = entry_->matchNext(media, i, tokenFactory, parentToken, state);
				if (h != -1) {
					found = true;
					i = h;
					break;
				}
				++i;
			}
			if (!found)
				i = -1;
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
	
	private:
		Ref<Node, Owner> entry_;
	};
	
	class OrNode: public Node
	{
	public:
		OrNode(Ref<Node> firstChoice, Ref<Node> secondChoice)
			: firstChoice_(firstChoice),
			  secondChoice_(secondChoice)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			int h = (firstChoice_) ? firstChoice_->matchNext(media, i, tokenFactory, parentToken, state) : i;
			if (h == -1)
				h = (secondChoice_) ? secondChoice_->matchNext(media, i, tokenFactory, parentToken, state) : i;
			i = h;
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
	
	private:
		Ref<Node, Owner> firstChoice_;
		Ref<Node, Owner> secondChoice_;
	};
	
	class AheadNode: public Node
	{
	public:
		AheadNode(Ref<Node> entry, int invert)
			: entry_(entry),
			  invert_(invert)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			int h = i;
			if (entry_)
				h = entry_->matchNext(media, i, 0, parentToken, state);
			
			if ((h == -1) ^ invert_)
				i = -1;
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
	
	private:
		Ref<Node, Owner> entry_;
		int invert_;
	};
	
	class LengthNode: public Node
	{
	public:
		LengthNode(int minLength, int maxLength, Ref<Node> entry)
			: minLength_(minLength),
			  maxLength_(maxLength),
			  entry_(entry)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			int h = entry_->matchNext(media, i, tokenFactory, parentToken, state);
			if (h != -1) {
				int d = h - i;
				if ((d < minLength_) || (maxLength_ < d))
					h = -1;
			}
			i = h;
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
	
	private:
		int minLength_;
		int maxLength_;
		Ref<Node, Owner> entry_;
	};
	
public:
	class Definition;
	
private:
	class InlineNode;
	
	class RuleNode: public Node
	{
	public:
		RuleNode(Definition* definition)
			: definition_(definition),
			  name_("undefined"),
			  id_(-1),
			  isVoid_(false)
		{}
		
		RuleNode(Definition* definition, const char* name, int ruleId, Ref<Node> entry, bool isVoid = false)
			: definition_(definition),
			  name_(name),
			  id_(ruleId),
			  entry_(entry),
			  isVoid_(isVoid)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			Ref<Token, Owner> token;
			if (tokenFactory) {
				token = tokenFactory->produce();
				token->init(name_, definition_->language(), id_);
				if (parentToken)
					parentToken->appendChild(token);
			}
			
			int i0 = i;
			i = (entry_) ? entry_->matchNext(media, i, tokenFactory, token, state) : i;
		
			if (token) {
				if (i != -1) {
					if (isVoid_) {
						if (parentToken) {
							token->unlink();
							parentToken->appendAllChildrenOf(token);
						}
					}
					else {
						token->setRange(i0, i);
					}
				}
				else {
					token->unlink();
				}
			}
			
			return i;
		}
		
		inline int id() const { return id_; }
		inline const char* name() const { return name_; }
		
	protected:
		friend class InlineNode;
		
		Definition* definition_;
		const char* name_;
		int id_;
		Ref<Node, Owner> entry_;
		bool isVoid_;
	};
	
	class LinkNode: public Node
	{
	public:
		LinkNode(Definition* definition, const char* ruleName)
			: defintion_(definition),
			  ruleName_(ruleName)
		{}
		
		inline const char* ruleName() const { return ruleName_; }
		inline Ref<RuleNode> rule() const { return rule_; }
		
	protected:
		friend class Definition;
		
		Definition* defintion_;
		const char* ruleName_;
		Ref<RuleNode> rule_;
		Ref<LinkNode> nextLink_;
	};
	
	class RefNode: public LinkNode
	{
	public:
		RefNode(Definition* definition, const char* ruleName)
			: LinkNode(definition, ruleName)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			Ref<Token> lastChildSaved = parentToken->lastChild();
			
			i = LinkNode::rule_->matchNext(media, i, tokenFactory, parentToken, state);
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			if (i == -1) {
				while (parentToken->lastChild() != lastChildSaved)
					parentToken->lastChild()->unlink();
			}
		
			return i;
		}
	};
	
	class InlineNode: public LinkNode
	{
	public:
		InlineNode(Definition* definition, const char* ruleName)
			: LinkNode(definition, ruleName)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			i = LinkNode::rule_->entry_->matchNext(media, i, tokenFactory, parentToken, state);
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
	};
	
	class BeforeNode: public LinkNode
	{
	public:
		BeforeNode(Definition* definition, const char* ruleName)
			: LinkNode(definition, ruleName)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			int h = -1;
			if (parentToken) {
				Ref<Token> sibling = parentToken->previousSibling();
				if (sibling)
					if (sibling->rule() == LinkNode::rule_->id())
						h = i;
			}
			i = h;
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
	};
	
public:
	typedef int (*CallBack) (Ref<Instance> self, Media* media, int i, State* state);
	
private:
	class CallNode: public Node
	{
	public:
		CallNode(CallBack callBack, Ref<Instance> self)
			: callBack_(callBack),
			  self_(self)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			i = callBack_(self_, media, i, state);
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
		
	private:
		CallBack callBack_;
		Ref<Instance, SetNull> self_;
	};
	
	class SetNode: public Node
	{
	public:
		SetNode(int flagId, bool value)
			: flagId_(flagId),
			  value_(value)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			*state->flag(flagId_) = value_;
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
	
	private:
		int flagId_;
		bool value_;
	};
	
	class IfNode: public Node
	{
	public:
		IfNode(int flagId, Ref<Node> trueBranch, Ref<Node> falseBranch)
			: flagId_(flagId),
			  trueBranch_(trueBranch),
			  falseBranch_(falseBranch)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			if (*state->flag(flagId_)) {
				if (trueBranch_)
					i = trueBranch_->matchNext(media, i, tokenFactory, parentToken, state);
			}
			else {
				if (falseBranch_)
					i = falseBranch_->matchNext(media, i, tokenFactory, parentToken, state);
			}
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
	
	private:
		int flagId_;
		Ref<Node, Owner> trueBranch_;
		Ref<Node, Owner> falseBranch_;
	};
	
	class GetCharNode: public Node
	{
	public:
		GetCharNode(int charId)
			: charId_(charId)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			if (i < media->length())
				*state->character(charId_) = media->get(i++);
			else
				i = -1;
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
	
	private:
		int charId_;
	};
	
	class SetCharNode: public Node
	{
	public:
		SetCharNode(int charId, Char value)
			: charId_(charId),
			  value_(value)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			*state->character(charId_) = value_;
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
	
	private:
		int charId_;
		Char value_;
	};
	
	class VarCharNode: public Node
	{
	public:
		VarCharNode(int charId, int invert)
			: charId_(charId),
			  invert_(invert)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			if (i < media->length()) {
				Char ch = media->get(i++);
				if ((ch != *state->character(charId_)) ^ invert_)
					i = -1;
			}
			else
				i = -1;
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
	
	private:
		int charId_;
		int invert_;
	};
	
	class GetStringNode: public Node
	{
	public:
		GetStringNode(int stringId, Ref<Node> termination)
			: stringId_(stringId),
			  termination_(termination)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			int i0 = i;
			int n = media->length();
			
			if (termination_)
			{
				while (true) {
					int h = termination_->matchNext(media, i, 0, parentToken, state);
					if ((h != -1) || (i == n)) break;
					++i;
				}
				
				Vector<Char>* string = state->string(stringId_);
				string->reset(i - i0);
				for (int k = i0; k < i; ++k)
					string->set(k - i0, media->get(k));
			}
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
		
	private:
		int stringId_;
		Ref<Node, Owner> termination_;
	};
	
	class VarStringNode: public Node
	{
	public:
		VarStringNode(int stringId)
			: stringId_(stringId)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			Vector<Char>* string = state->string(stringId_);
			int k = 0, m = string->length();
			int n = media->length();
			while ((i < n) && (k < m)) {
				if (media->get(i) != string->get(k))
					break;
				++i;
				++k;
			}
			
			if (k != m)
				i = -1;
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
		
	private:
		int stringId_;
	};
	
	class InvokeNode: public Node
	{
	public:
		InvokeNode(Ref<Definition> definition, Ref<Node> termination)
			: definition_(definition),
			  termination_(termination)
		{}
		
		virtual int matchNext(Media* media, int i, TokenFactory* tokenFactory, Token* parentToken, State* state)
		{
			State* childState = 0;
			if (state) {
				childState = state->child();
				if (childState) {
					if (childState->language() != definition_->language())
						childState = 0;
				}
				if (!childState)
					childState = definition_->newState(state);
			}
			
			if (termination_) {
				int i1 = i;
				while (i1 < media->length()) {
					if (termination_->matchNext(media, i1, 0, parentToken, state) != -1)
						break;
					++i1;
				}
				
				Media range(media, i1);
				i = definition_->matchNext(&range, i, tokenFactory, parentToken, childState);
			}
			else {
				i = definition_->matchNext(media, i, tokenFactory, parentToken, childState);
			}
			
			if ((i != -1) && (next_))
				i = next_->matchNext(media, i, tokenFactory, parentToken, state);
			
			return i;
		}
		
	private:
		Ref<Definition, SetNull> definition_;
		Ref<Node, Owner> termination_;
	};
	
public:
	class Definition: public RuleNode
	{
	public:
		Definition(int language = -1)
			: RuleNode(this),
			  language_(language),
			  numRules_(0),
			  numKeywords_(0),
			  ruleByName_(new RuleByName),
			  tokenTypeByName_(new TokenTypeByName),
			  numStateFlags_(0),
			  numStateChars_(0),
			  numStateStrings_(0),
			  flagIdByName_(new StateIdByName),
			  charIdByName_(new StateIdByName),
			  stringIdByName_(new StateIdByName)
		{}
		
		typedef Ref<Node, Owner> NODE;
		typedef Ref<RuleNode, Owner> RULE;
		
		//-- stateless definition interface
		
		inline static NODE CHAR(Char ch) { return new CharNode(ch, 0); }
		inline static NODE OTHER(Char ch) { return new CharNode(ch, 1); }
		inline static NODE ANY() { return new AnyNode(); }
		inline static NODE RANGE(Char a, Char b) { return new RangeNode(a, b, 0); }
		inline static NODE RANGE(Char* s) { return new RangeNode(s, 0); }
		inline static NODE RANGE(const char* s) { return new RangeNode(s, 0); }
		inline static NODE EXCEPT(Char a, Char b) { return new RangeNode(a, b, 1); }
		inline static NODE EXCEPT(Char* s) { return new RangeNode(s, 1); }
		inline static NODE EXCEPT(const char* s) { return new RangeNode(s, 1); }
		inline static NODE STRING(Char* s, int len) { return new StringNode(s, len); }
		inline static NODE STRING(const char* s) { return new StringNode(s); }
		
		NODE KEYWORD(const char* keys)
		{
			Ref<KeywordMap, Owner> map = new KeywordMap;
			while (*keys) {
				if ((*keys == ' ') || (*keys == '\t')) {
					++keys;
					continue;
				}
				int n = 0;
				while (true) {
					char ch = *(keys + n);
					if ((ch == ' ') || (ch == '\t') || (ch == '\0')) break;
					++n;
				}
				map->insert(keys, n, numKeywords_);
				tokenTypeByName_->insert(keys, n, numKeywords_);
				++numKeywords_;
				keys += n;
			}
			return new KeywordNode(map);
		}
		inline NODE TOKEN(const char* name) {
			int tokenType = -1;
			tokenTypeByName_->lookup(name, &tokenType);
			return CHAR(tokenType);
		}
		
		inline static NODE REPEAT(int minRepeat, int maxRepeat, NODE entry) { return new RepeatNode(minRepeat, maxRepeat, entry); }
		inline static NODE REPEAT(int minRepeat, NODE entry) { return REPEAT(minRepeat, intMax, entry); }
		inline static NODE REPEAT(NODE entry) { return REPEAT(0, intMax, entry); }
		inline static NODE BOI() { return new BoiNode(); }
		inline static NODE EOI() { return new EoiNode(); }
		inline static NODE PASS() { return 0; }
		inline static NODE FAIL() { return NOT(PASS()); }
		inline static NODE FIND(NODE entry) { return new FindNode(entry); }
		inline static NODE AHEAD(NODE entry) { return new AheadNode(entry, 0); }
		inline static NODE NOT(NODE entry) { return new AheadNode(entry, 1); }
		
		inline static NODE LENGTH(int minLength, int maxLength, NODE entry) { return new LengthNode(minLength, maxLength, entry); }
		inline static NODE LENGTH(int minLength, NODE entry) { return LENGTH(minLength, intMax, entry); }
		
		#include "SyntaxSugar.hpp"
		
		inline RULE DEFINE(const char* name, NODE entry) {
			Ref<RuleNode, Owner> rule = new RuleNode(this, name, numRules_++, entry);
			ruleByName_->insert(name, rule);
			return rule;
		}
		inline RULE DEFINE_VOID(const char* name, NODE entry) {
			Ref<RuleNode, Owner> rule = new RuleNode(this, name, numRules_++, entry, true);
			ruleByName_->insert(name, rule);
			return rule;
		}
		inline void DEFINE_SELF(const char* name, Ref<Node> entry) {
			RuleNode::name_ = name;
			RuleNode::id_ = numRules_++;
			RuleNode::entry_ = entry;
			ruleByName_->insert(name, this);
		}
		
		inline NODE REF(const char* ruleName) {
			Ref<RefNode, Owner> link = new RefNode(this, ruleName);
			link->nextLink_ = linkHead_;
			linkHead_ = link;
			return link;
		}
		inline NODE INLINE(const char* ruleName) {
			Ref<InlineNode, Owner> link = new InlineNode(this, ruleName);
			link->nextLink_ = linkHead_;
			linkHead_ = link;
			return link;
		}
		inline NODE BEFORE(const char* ruleName) {
			Ref<BeforeNode, Owner> link =  new BeforeNode(this, ruleName);
			link->nextLink_ = linkHead_;
			linkHead_ = link;
			return link;
		}
		
		inline NODE CALL(CallBack callBack, Ref<Instance> self = 0) {
			return new CallNode(callBack, self);
		}
		
		inline void LINK() {
			while (linkHead_) {
				linkHead_->rule_ = ruleByName(linkHead_->ruleName_);
				linkHead_ = linkHead_->nextLink_;
			}
		}
		
		//-- stateful definition interface
		
		inline void STATE_FLAG(const char* name, bool defaultValue = false) {
			stateFlagHead_ = new StateFlag(stateFlagHead_, defaultValue);
			flagIdByName()->insert(name, numStateFlags_);
			++numStateFlags_;
		}
		inline void STATE_CHAR(const char* name, Char defaultValue = 0) {
			stateCharHead_ = new StateChar(stateCharHead_, defaultValue);
			charIdByName()->insert(name, numStateChars_);
			++numStateChars_;
		}
		inline void STATE_STRING(const char* name, const char* defaultValue = 0) {
			stateStringHead_ = new StateString(stateStringHead_, defaultValue);
			stringIdByName()->insert(name, numStateStrings_);
			++numStateStrings_;
		}
		inline NODE SET(const char* name, bool value) {
			return new SetNode(flagIdByName(name), value);
		}
		inline NODE IF(const char* name, NODE trueBranch, NODE falseBranch = 0) {
			return new IfNode(flagIdByName(name), trueBranch, falseBranch);
		}
		inline NODE GETCHAR(const char* name) {
			return new GetCharNode(charIdByName(name));
		}
		inline NODE SETCHAR(const char* name, Char value) {
			return new SetCharNode(charIdByName(name), value);
		}
		inline NODE VARCHAR(const char* name) {
			return new VarCharNode(charIdByName(name), 0);
		}
		inline NODE VAROTHER(const char* name) {
			return new VarCharNode(charIdByName(name), 1);
		}
		inline NODE GETSTRING(const char* name, NODE termination) {
			return new GetStringNode(stringIdByName(name), termination);
		}
		inline NODE VARSTRING(const char* name) {
			return new VarStringNode(stringIdByName(name));
		}
		inline NODE INVOKE(Definition* definition, NODE termination = 0) {
			if (definition != this)
				definition->LINK();
			return new InvokeNode(definition, termination);
		}
		
		//-- execution interface
		
		inline int language() const { return language_; }
		inline int numRules() const { return numRules_; }
		
		State* newState(State* parent = 0)
		{
			State* state = new State(language_, numStateFlags_, numStateChars_, numStateStrings_, parent);
			
			Ref<StateFlag> stateFlag = stateFlagHead_;
			for (int id = numStateFlags_ - 1; id >= 0; --id) {
				*state->flag(id) = stateFlag->defaultValue_;
				stateFlag = stateFlag->next_;
			}
			
			Ref<StateChar> stateChar = stateCharHead_;
			for (int id = numStateChars_ - 1; id >= 0; --id) {
				*state->character(id) = stateChar->defaultValue_;
				stateChar = stateChar->next_;
			}
			
			Ref<StateString> stateString = stateStringHead_;
			for (int id = numStateStrings_ - 1; id >= 0; --id) {
				*state->string(id) = stateString->defaultValue_;
				stateString = stateString->next_;
			}
			
			return state;
		}
		
		bool find(Media* media, int* i0, int* i1 = 0, Ref<Token, Owner>* rootToken = 0, uint8_t* buf = 0, int bufSize = 0)
		{
			LINK();
			
			int i = *i0;
			int n = media->length();
			bool found = false;
			while (i < n) {
				if (match(media, i, i1, rootToken, 0, buf, bufSize)) {
					found = true;
					break;
				}
				++i;
			}
			*i0 = i;
			return found;
		}
		
		bool match(Media* media, int i0 = 0, int* i1 = 0, Ref<Token, Owner>* rootToken = 0, State* state = 0, uint8_t* buf = 0, int bufSize = 0)
		{
			LINK();
			
			if (!state) {
				if ((stateFlagHead_) || (stateCharHead_))
					state = newState();
			}
			
			TokenFactory tokenFactory(buf, bufSize);
			int h = matchNext(media, i0, &tokenFactory, 0, state);
			if (rootToken)
				*rootToken = tokenFactory.rootToken();
			
			if ((i1 != 0) && (h != -1))
				*i1 = h;
			return (h != -1);
		}
		
		Ref<Token, Owner> match(Media* media, State* state)
		{
			Ref<Token, Owner> rootToken = 0;
			int i0 = 0, i1 = 0;
			match(media, i0, &i1, &rootToken, state);
			return rootToken;
		}
		
		Ref<RuleNode, Owner> ruleByName(const char* name)
		{
			Ref<RuleNode, Owner> node;
			ruleByName_->lookup(name, &node);
			if (!node)
				PONA_THROW(SyntaxException, "Undefined rule referenced");
			return node;
		}
		
		int tokenType(const char* name)
		{
			int tokenType = -1;
			tokenTypeByName_->lookup(name, &tokenType);
			if (tokenType == -1)
				PONA_THROW(SyntaxException, "Undefined token type referenced");
			return tokenType;
		}
		
		int flagIdByName(const char* name)
		{
			int flagId = -1;
			flagIdByName()->lookup(name, &flagId);
			if (flagId == -1)
				PONA_THROW(SyntaxException, "Undefined state flag referenced");
			return flagId;
		}
		
		int charIdByName(const char* name)
		{
			int charId = -1;
			charIdByName()->lookup(name, &charId);
			if (charId == -1)
				PONA_THROW(SyntaxException, "Undefined state char referenced");
			return charId;
		}
		
		int stringIdByName(const char* name)
		{
			int stringId = -1;
			stringIdByName()->lookup(name, &stringId);
			if (stringId == -1)
				PONA_THROW(SyntaxException, "Undefined state string referenced");
			return stringId;
		}
		
	private:
		int language_;
		
		class StateFlag: public Instance {
		public:
			StateFlag(Ref<StateFlag> head, bool defaultValue)
				: next_(head),
				  defaultValue_(defaultValue)
			{}
			Ref<StateFlag, Owner> next_;
			bool defaultValue_;
		};
		
		class StateChar: public Instance {
		public:
			StateChar(Ref<StateChar> head, Char defaultValue)
				: next_(head),
				  defaultValue_(defaultValue)
			{}
			Ref<StateChar, Owner> next_;
			Char defaultValue_;
		};
		
		class StateString: public Instance {
		public:
			StateString(Ref<StateString> head, const char* defaultValue)
				: next_(head),
				  defaultValue_(defaultValue)
			{}
			Ref<StateString, Owner> next_;
			Vector<Char> defaultValue_;
		};
		
		int numRules_;
		int numKeywords_;
		typedef PrefixTree<char, Ref<RuleNode, Owner> > RuleByName;
		typedef PrefixTree<char, int> TokenTypeByName;
		Ref<RuleByName, Owner> ruleByName_;
		Ref<TokenTypeByName, Owner> tokenTypeByName_;
		
		Ref<LinkNode> linkHead_;
		
		int numStateFlags_;
		int numStateChars_;
		int numStateStrings_;
		Ref<StateFlag, Owner> stateFlagHead_;
		Ref<StateChar, Owner> stateCharHead_;
		Ref<StateString, Owner> stateStringHead_;
		
		typedef PrefixTree<char, int> StateIdByName;
		Ref<StateIdByName, Owner> flagIdByName_;
		Ref<StateIdByName, Owner> charIdByName_;
		Ref<StateIdByName, Owner> stringIdByName_;
		
		inline Ref<StateIdByName> flagIdByName() {
			if (!flagIdByName_) flagIdByName_ = new StateIdByName;
			return flagIdByName_;
		}
		inline Ref<StateIdByName> charIdByName() {
			if (!charIdByName_) charIdByName_ = new StateIdByName;
			return charIdByName_;
		}
		inline Ref<StateIdByName> stringIdByName() {
			if (!stringIdByName_) stringIdByName_ = new StateIdByName;
			return stringIdByName_;
		}
	};
};

} // namespace pona

#endif // PONA_SYNTAX_HPP
