#define CATCH_CONFIG_MAIN
#include "../unit_testing/catch.hpp"

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdio.h>
#include <cassert>
#include "UniversalPtr.h"

SCENARIO("UniversalPtr can be constructed") {
	WHEN("default constructed")	{
		UniversalPtr<int> p;
		THEN("converts to false") {
			REQUIRE(!p);
		}
	}
	
	WHEN("constructed from nullptr") {
		UniversalPtr<int> p(nullptr);
		THEN("converts to false") {
			REQUIRE(!p);
		}
		THEN("retrieves null pointer") {
			REQUIRE(p.get() == nullptr);
		}
	}

	WHEN("constructed from non-empty unique_ptr") {
		auto src = std::make_unique<int>(42);
		auto srcPtr = src.get();
		UniversalPtr<int> p(std::move(src));

		THEN("takes ownership from unique_ptr") {
			CHECK(!src);
		}
		THEN("can be converted to true") {
			CHECK(p);
		}
		THEN("returns pointer to value") {
			CHECK(p.get() == srcPtr);
		}
		THEN("can retrieve its value") {
			CHECK(*p == 42);
		}
	}

	WHEN("constructed from non-empty shared_ptr") {
		auto src = std::make_shared<int>(42);
		UniversalPtr<int> p(src);

		THEN("shares ownership with shared_ptr") {
			auto oldCount = src.use_count();
			UniversalPtr<int> p1(src);
			CHECK(src.use_count() == oldCount + 1);
		}
		THEN("can be converted to true") {
			REQUIRE(p);
		}
		THEN("returns pointer to value") {
			CHECK(p.get() == src.get());
		}
		THEN("can retrieve its value") {
			CHECK(*p == 42);
		}
	}
}

SCENARIO("UniversalPtr comparison") {
	GIVEN("non empty UniversalPtr") {
		int value = 0;
		UniversalPtr<int> p1(value);

		THEN("is equal to itself") {
			CHECK(p1 == p1);
			CHECK(!(p1 != p1));
		}
		THEN("is not equal to nullptr") {
			CHECK(nullptr != p1);
			CHECK(p1 != nullptr);
			CHECK(!(nullptr == p1));
			CHECK(!(p1 == nullptr));
		}
		THEN("is equal to pointers to the same object") {
			{
				UniversalPtr<int> p2(value);
				CHECK(p1 == p2);
				CHECK(!(p1 != p2));
			}
			{
				auto p1Copy(p1);
				CHECK(p1 == p1Copy);
				CHECK(!(p1Copy != p1));
			}
		}
	}
}

/*
struct Foo
{
	virtual ~Foo() = default;
	virtual void Bar()
	{
		std::cout << "Foo::Bar\n";
	}
	void ConstBar()const
	{
		std::cout << "Foo::ConstBar\n";
	}
};
struct FooEx : public Foo
{
	void Bar() override
	{
		std::cout << "FooEx::Bar\n";
	}
};

struct Client
{
	Client(UniversalPtr<Foo> foo)
		: m_foo(std::move(foo))
	{
	}
	void DoSomething()
	{
		m_foo->Bar();
	}
private:
	UniversalPtr<Foo> m_foo;
};

struct Client2
{
	Client2(UniversalPtr<Foo> foo)
		: m_foo(std::move(foo))
	{
	}
	void DoSomethingElse()
	{
		m_foo->Bar();
		m_foo->Bar();
	}
private:
	UniversalPtr<Foo> m_foo;
};

struct Client3
{
	Client3(UniversalPtr<FILE> file)
		: m_file(std::move(file))
	{
	}
	void DoSomething()
	{
		if (m_file)
		{
			fgetc(m_file.get());
		}
	}
private:
	UniversalPtr<FILE> m_file;
};

int main(int, char* argv[])
{
	// Отсутствие владения (ссылка)
	{
		Foo foo;
		Client c(foo);
		c.DoSomething();
	}
	// Отсутствие владения (указатель)
	{
		Foo foo;
		Client c(&foo);
		c.DoSomething();
	}
	// Единоличное владение
	{
		Client c(std::make_unique<Foo>());
		c.DoSomething();
	}
	// Единоличное владение (пользовательский deleter)
	{
		struct FileCloser
		{
			void operator()(FILE *file)
			{ 
				if (file)
					fclose(file);
			}
		};
		std::unique_ptr<FILE, FileCloser> foo(fopen(argv[0], "rb"));
		Client3 c(std::move(foo));
		c.DoSomething();
	}
	// Совместное владение
	{
		auto foo = std::make_shared<Foo>();
		Client c1(foo);
		Client2 c2(foo);
		c1.DoSomething();
		c2.DoSomethingElse();
	}
	{
		auto sp = std::make_shared<Foo>();
		UniversalPtr<Foo> p1(sp);
		UniversalPtr<Foo> p2(sp);
		assert(p1 == p2);
		UniversalPtr<Foo> p3;
		assert(p1 != p3);
	}
	{
		static_assert(std::is_nothrow_default_constructible<UniversalPtr<Foo>>::value, "UniversalPtr is nothrow default constructible");
		static_assert(std::is_nothrow_copy_constructible<UniversalPtr<Foo>>::value, "UniversalPtr is nothrow copy constructible");
		static_assert(std::is_nothrow_move_constructible<UniversalPtr<Foo>>::value, "UniversalPtr is nothrow move constructible");
		static_assert(std::is_nothrow_copy_assignable<UniversalPtr<Foo>>::value, "UniversalPtr is nothrow copy asignable");
		static_assert(std::is_nothrow_move_assignable<UniversalPtr<Foo>>::value, "UniversalPtr is nothrow move assignable");
	}
	{
		UniversalPtr<const Foo> p1;
		p1 = std::make_unique<FooEx>();
		p1 = std::make_shared<FooEx>();
		p1->ConstBar();
	}
	return 0;
}
*/