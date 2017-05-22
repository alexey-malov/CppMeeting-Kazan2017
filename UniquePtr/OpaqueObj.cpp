#include "OpaqueObj.h"
#include <iostream>

struct OpaqueObj::Impl
{
	Impl(int data) : m_data(data) {}

	void Foo()
	{
		/* do something */
	}
	int m_data = 42;
};

OpaqueObj::OpaqueObj(int data)
	: m_impl(std::make_unique<Impl>(data))
{
}

OpaqueObj::~OpaqueObj() = default;

void OpaqueObj::Foo()
{
	m_impl->Foo();
}

