#include "C-Library.h"
#include "OpaqueObj.h"
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

struct DataDeleter {
  void operator()(Data *data) const noexcept { DeallocateData(data); }
};
using DataPtr = std::unique_ptr<Data, DataDeleter>;

DataPtr SafeAllocateData() {
  if (DataPtr p{AllocateData()})
    return p;
  throw std::runtime_error("Failed to allocate data");
}

using namespace std;
int CalculateX(int value) {
  if (value < 0) {
    throw std::invalid_argument("Invalid argument");
  }
  return 42;
}

bool CStyleCode() {
  Data *p = AllocateData();
  if (!p) {
    cout << "Failed to allocated data\n";
    return false;
  } else {
    if (p->value == 42) {
      DeallocateData(p);
      return true;
    }
    int x = CalculateX(p->value); // Если CalculateX выбросит исключение,
                                  // DeallocateData не будет вызван
    DoSomethingWithData(p, x);
    DeallocateData(p);
    return false;
  }
}

bool CppStyleCode() {
  try {
    auto p = SafeAllocateData();
    if (p->value == 42)
      return true;
    DoSomethingWithData(p.get(), CalculateX(p->value));
  } catch (std::exception const &e) {
    cout << e.what() << "\n";
  }
  return false;
}

int main() {
  OpaqueObj obj(42);
  obj.Foo();

  {}

  try {
    auto p = SafeAllocateData();
    if (p->value == 42) {
      return 42;
    }
  } // при выходе из блока p будет освобожден при помощи DeallocateData
  catch (const exception &e) {
    // Unknown exception
    cout << e.what() << "\n";
  }
}
