#ifndef FTL_MACHOBJECT_HPP
#define FTL_MACHOBJECT_HPP

#include "String.hpp"
#include "Format.hpp"

namespace ftl
{

class MachObject: public Instance
{
public:
	MachObject(String objectPath, Ref<StringList> dependencyPaths)
		: objectPath_(objectPath),
		  dependencyPaths_(dependencyPaths)
	{}

	inline String objectPath() const { return objectPath_; }
	inline Ref<StringList> dependencyPaths() const { return dependencyPaths_; }

private:
	String objectPath_;
	Ref<StringList, Owner> dependencyPaths_;
};

typedef List<MachObject> MachObjectList;

} // namespace ftl

#endif // FTL_MACHOBJECT_HPP