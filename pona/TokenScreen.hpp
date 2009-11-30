#ifndef PONA_TOKENSCREEN_HPP
#define PONA_TOKENSCREEN_HPP

#include "atoms"

namespace pona
{

class Token;

class TokenScreen: public virtual Instance
{
public:
	virtual bool project(Ref<Token> token, int i0, int i1) = 0;
};

} // namespace pona

#endif // PONA_TOKENSCREEN_HPP