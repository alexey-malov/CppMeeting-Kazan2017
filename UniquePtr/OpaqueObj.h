#pragma once
#include <memory>

class OpaqueObj
{
public:
	OpaqueObj(int data);

	// Explicitly defined destructor is needed to work with incomplete type Impl
	~OpaqueObj();

	void Foo();
private:
	struct Impl;
	std::unique_ptr<Impl> m_impl;
};

