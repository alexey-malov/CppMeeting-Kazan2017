#include "OpaqueObj.h"
#include <iostream>

struct OpaqueObj1::Impl {
  Impl(int data) : m_data(data) {}

  void Foo() { /* do something */
  }
  int m_data = 42;
};

OpaqueObj1::OpaqueObj1(int data) 
  : m_impl(new Impl(data))
{}

void OpaqueObj1::Foo() {
  m_impl->Foo();
}

OpaqueObj1::~OpaqueObj1() {
  delete m_impl;
}

//////////////////////////////////////////////////////////////////////////

struct OpaqueObj::Impl {
  Impl(int data) : m_data(data) {}

  void Foo() { /* do something */
  }
  int m_data = 42;
};

OpaqueObj::OpaqueObj(int data) 
  : m_impl(std::make_unique<Impl>(data))
{}

OpaqueObj::~OpaqueObj() = default;

void OpaqueObj::Foo() {
  m_impl->Foo();
}
