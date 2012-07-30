#include <ftl/stdio>
#include <ftl/Config.hpp>

using namespace ftl;

int main(int argc, char** argv)
{
	Ref<Config, Owner> config = new Config("Mach");
	config->init(argc, argv);
	Ref<WireObject> wire = config->object();
	Ref<WireList> header = wire->member("header");
	for (int i = 0, n = header->length(); i < n; ++i) {
		print("%%:\n", header->at(i));
		Ref<DirEntry, Owner> entry = new DirEntry;
		Ref<Glob, Owner> glob = new Glob(header->at(i));
		while (glob->read(entry)) print("  %%\n", entry->name());
	}
	return 0;
}
