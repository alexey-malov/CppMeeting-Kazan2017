#define CATCH_CONFIG_MAIN
#include "../unit_testing/catch.hpp"

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdio.h>
#include <cassert>
#include "UniversalPtr.h"
#include <string>

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

SCENARIO("UniversalPtr Demo") {
	using namespace std;

	struct IShape {
		virtual ~IShape() = default;
		virtual void Draw()const = 0;
	};
	struct IShapeFactory {
		virtual ~IShapeFactory() = default;
		virtual unique_ptr<IShape> CreateShape(string name) = 0;
	};
	struct ShapeFactory : IShapeFactory {
		struct Rectangle : IShape { 
			void Draw()const override { cout << "Rectangle\n"; }
		};
		struct Circle : IShape {
			void Draw()const override { cout << "Circle\n"; }
		};
		unique_ptr<IShape> CreateShape(string name) override {
			if (name == "rect") return make_unique<Rectangle>();
			else if (name == "circle") return make_unique<Circle>();
			else return nullptr;
		}
	};
	
	struct Client {
		Client(UniversalPtr<IShapeFactory> factory):m_factory(move(factory)) {}
		void WorkWithShapes() {
			if (auto circle = m_factory->CreateShape("circle")) 
				circle->Draw();
			if (auto rect = m_factory->CreateShape("rect"))
				rect->Draw();
		}
	private:
		UniversalPtr<IShapeFactory> m_factory;
	};

	GIVEN("Unowned Shape Factory") {
		ShapeFactory factory;
		THEN("can be used by clients without shorter lifitime") {
			Client c1(factory);
			Client c2(&factory);
			c1.WorkWithShapes();
			c2.WorkWithShapes();
		}
	}

	GIVEN("Unique ShapeFactory") {
		auto uniqueFactory = make_unique<ShapeFactory>();
		THEN("can be passed to a client for exclusive access") {
			Client client(move(uniqueFactory));
			CHECK(!uniqueFactory); // moved to the client
			client.WorkWithShapes();
		}
	}
	GIVEN("Shared ShapeFactory") {
		auto sharedFactory = make_shared<ShapeFactory>();
		weak_ptr<IShapeFactory> weakFactory(sharedFactory);

		WHEN("shared among clients") {
			auto c1 = make_unique<Client>(sharedFactory);
			auto c2 = make_unique<Client>(sharedFactory);
			sharedFactory.reset();
			CHECK(weakFactory.use_count() == 2);

			THEN("increases use count when chlient is copied") {
				auto client1Copy(*c1);
				CHECK(weakFactory.use_count() == 3);
				client1Copy.WorkWithShapes();
			}
			THEN("is deleted when there are no clients") {
				c1.reset();
				CHECK(weakFactory.use_count() == 1);

				// c2 still holds the factory via strong reference
				c2->WorkWithShapes();

				c2.reset();
				CHECK(weakFactory.use_count() == 0);
				CHECK(weakFactory.expired());
			}
		}
	}
}