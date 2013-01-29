#include <ftl/stdio>
#include <ftl/strings>

namespace ftl
{

int main(int argc, char **argv)
{
	{
		string s = "Übertragung";
		print("s = \"%%\"\n", s);
		print("s->chars()->length() = %%\n", s->chars()->length());
		print("s->length() = %%\n", s->length());
		print("s->copy() = \"%%\"\n", s->copy());
		print("string(s->chars()->copy(s->chars()->length()-3, s->chars()->length())) = \"%%\"\n", string(s->chars()->copy(s->chars()->length()-3, s->chars()->length())));
		for (int i = 0; i < s->chars()->length(); ++i)
			print("s[%%] = '%%' (%%)\n", i, s->chars()->copy(i, i + 1), s->chars()->get(i));
		print("s->chars()->copy(s->chars()->length()-3, s->chars()->length()) = \"%%\"\n", s->chars()->copy(s->chars()->length()-3, s->chars()->length()));
		hook<StringList> parts = s->split("a");
		print("s.split(\"a\") = [\n");
		for (int i = 0; i < parts->length(); ++i)
			print("  \"%%\"\n", parts->get(i));
		print("]\n");
	}
	{
		string s = "bin/testPath";
		// print("s = \"%%\"\n", s);
		print("s->find(\"/\") = %%\n", s->find("/"));
		hook<StringList> parts = s->split("/");
		print("s.split(\"/\") = [\n");
		for (int i = 0; i < parts->length(); ++i)
			print("  \"%%\"\n", parts->get(i));
		print("]\n");
	}
	{
		/*string s = "Привет!";
		for (int i = 0; i < s.length(); ++i)
			print("%% (\\u%hex:4:4.:'0'%)\n", s.copy(i, i + 1), s.get(i));*/
		string s = "Hallo!, \n\\u041F\\u0440\\u0438\\u0432\\u0435\\u0442!, \\ud834\\udd22, Hello!";
		print("s = \"%%\", s->chars()->length() = %%\n", s, s->chars()->length());
		string se = s->expand();
		print("se.get(16) = %hex%\n", se->chars()->at(17));
		print("se = \"%%\", s->chars()->length() = %%\n", se, se->chars()->length());
	}
	{
		string s = "..Привет, Привет!";
		s->replaceInsitu("Привет", "Hello");
		print("s = \"%%\"\n", s);
	}
	return 0;
}

} // namespace ftl

int main(int argc, char **argv)
{
	return ftl::main(argc, argv);
}
