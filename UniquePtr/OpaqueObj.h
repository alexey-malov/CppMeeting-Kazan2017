#pragma once
#include <memory>

class OpaqueObj1 {
public:
	OpaqueObj1(int data);
	void Foo();
	~OpaqueObj1();
private:
	struct Impl;
	Impl* m_impl;
};

class OpaqueObj {
public:
  OpaqueObj(int data);

  // Explicitly defined destructor is needed to work with incomplete type Impl
  ~OpaqueObj();

  void Foo();

private:
  struct Impl;
  std::unique_ptr<Impl> m_impl;
};
