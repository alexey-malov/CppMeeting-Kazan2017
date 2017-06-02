#pragma once
#include <memory>
#include <functional>
using namespace std;

struct IProduct {
	virtual ~IProduct() = default;
	virtual void Foo() = 0;
};
struct ConcreteProduct : IProduct {
	ConcreteProduct(int data) :m_data(data) {}
	void Foo() override {}
private:
	int m_data;
};

struct IFactory {
	virtual ~IFactory() = default;
	virtual unique_ptr<IProduct> CreateProduct() = 0;
};
void Client(IFactory& factory) {
	auto product = factory.CreateProduct();
	product->Foo();
}
struct ConcreteFactory : IFactory {
	ConcreteFactory(int data) :m_data(data) {}
	unique_ptr<IProduct> CreateProduct() override {
		return make_unique<ConcreteProduct>(m_data);
	}
private:
	int m_data;
};
void TestClassicFactory() {
	ConcreteFactory factory(42);
	Client(factory);
}

using Factory = function<unique_ptr<IProduct>()>;
void Client(const Factory& factory) {
	auto product = factory();
	product->Foo();
}
void TestFunctionalFactory() {
	Client([] {return make_unique<ConcreteProduct>(42); });
}
