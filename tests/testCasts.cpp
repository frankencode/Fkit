#include <ftl/stdio>
#include <ftl/time>

namespace ftl
{

class B;

class A: public Instance
{
public:
	A(): m1_(1), m2_(7) {}
	void doSmth() { if (m1_ > m2_) m2_ += m2_; else m1_ += m2_; }
	virtual B* toB() { return 0; }
private:
	int m1_, m2_;
};

class B: public A
{
public:
	B(): m1_(1), m2_(2) {}
	void doSmth() { if (m1_ < m2_) m2_ -= m2_; else m1_ -= m2_; }
	virtual B* toB() { return this; }
private:
	int m1_, m2_;
};

int main()
{
	const int n = 1000000;
	{
		A* b = new B;
		Time t = now();
		for (int i = 0; i < n; ++i)
			b->toB()->doSmth();
		t = now() - t;
		print("%% vtable casting, casting upwards, cost %% us\n", n, t.us());
	}
	{
		A* b = new B;
		Time t = now();
		for (int i = 0; i < n; ++i)
			dynamic_cast<B*>(b)->doSmth();
		t = now() - t;
		print("%% dynamic_cast, casting upwards, cost %% us\n", n, t.us());
	}
	{
		B* b = new B;
		Time t = now();
		for (int i = 0; i < n; ++i)
			dynamic_cast<A*>(b)->doSmth();
		t = now() - t;
		print("%% dynamic_cast, casting downwards, cost %% us\n", n, t.us());
	}
	{
		B* b = new B;
		Time t = now();
		for (int i = 0; i < n; ++i)
			dynamic_cast<B*>(b)->doSmth();
		t = now() - t;
		print("%% dynamic_cast, identity cast, cost %% us\n", n, t.us());
	}
	{
		B* b = new B;
		Time t = now();
		for (int i = 0; i < n; ++i)
			b->doSmth();
		t = now() - t;
		print("%% no cast, cost %% us\n", n, t.us());
	}
	{
		B* b = new B;
		Time t = now();
		for (int i = 0; i < n; ++i)
			static_cast<A*>(b)->doSmth();
		t = now() - t;
		print("%% static_cast, casting downwards, cost %% us\n", n, t.us());
	}
	return 0;
}

} // namespace ftl

int main()
{
	return ftl::main();
}
