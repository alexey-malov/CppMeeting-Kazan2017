#pragma once
#include <windows.h>

namespace detail {
template <HANDLE INVALID>
class Handle {
public:
  Handle() = default;
  explicit Handle(HANDLE handle = INVALID) : m_handle(handle) {}
  Handle(Handle&& x) : m_handle(x.m_handle) { x.m_handle = INVALID; }
  Handle(const Handle &&) = delete; // no copy construction

  Handle &operator=(Handle&& x) { // move assignment
    std::swap(m_handle, x.m_handle);
    return *this;
  }
  Handle &operator=(const Handle&) = delete; // no copy assignment

  ~Handle() {
    if (m_handle != INVALID) {
      CloseHandle(m_handle);
    }
  }

  explicit operator bool() const { return m_handle != INVALID; }
  operator HANDLE() const { return m_handle; }

  void Close() {
    if (!CloseHandle(m_handle)) {
      m_handle = INVALID;
      throw std::runtime_error("Failed to close handle");
    }
    m_handle = INVALID;
  }

private:
  HANDLE m_handle = INVALID;
};
} // namespace detail

using FileHandle = detail::Handle<INVALID_HANDLE_VALUE>;
