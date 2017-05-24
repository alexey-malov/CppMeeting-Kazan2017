#include <memory>
#include <functional>
#include <iostream>
#include <unordered_set>
#include <cassert>
#include <stdexcept>

using namespace std;

struct Bad
{
	~Bad()
	{
		cout << "Bad is shutting down\n";
	}
	void DoAsyncJob(function<void(bool)> callback)
	{
		// somewhere in the future
		callback(true);
		cout << "Data is " << data << "\n";
	}
private:
	int data = 42;
};

struct Good : enable_shared_from_this<Good>
{
	~Good()
	{
		cout << "Good is shutting down\n";
	}
	void DoAsyncJob(function<void(bool)> callback)
	{
		auto self = shared_from_this();
		// somewhere in the future
		callback(true);
		cout << "Data is " << data << "\n";
	}
private:
	int data = 42;
};

using NodePtr = shared_ptr<struct Node>;

struct Node : enable_shared_from_this<Node>
{
	void AddChild(const NodePtr& node)
	{
		auto self = shared_from_this();
		if (node == self || IsDescendantOf(node))
			throw invalid_argument("Can't add self or ancestor");

		if (m_children.emplace(node).second)
		{
			if (auto oldParent = node->GetParent())
			{
				oldParent->m_children.erase(node);
			}
			node->m_parent = self;
		}
	}

	NodePtr GetParent()const { return m_parent.lock(); }

	bool IsDescendantOf(const NodePtr& node)const
	{
		auto self = shared_from_this();
		for (auto x = node->GetParent(); x; x = x->GetParent())
		{
			if (x == self) return true;
		}
		return false;
	}
private:
	weak_ptr<Node> m_parent;
	unordered_set<NodePtr> m_children;
};

int main()
{
#if 0 // will cause UB
	{
		auto bad = make_shared<Bad>();
		bad->DoAsyncJob([&](bool finished) {
			if (finished)
			{
				bad.reset();
			}
		});
	}
	cout << "----\n";
#endif
	{
		auto good = make_shared<Good>();
		good->DoAsyncJob([&](bool finished) {
			if (finished)
			{
				good.reset();
			}
		});
	}

	{
		auto a = make_shared<Node>();
		auto b = make_shared<Node>();
		auto c = make_shared<Node>();
		a->AddChild(b);
		c->AddChild(b); // add to c and detach from a
		b->AddChild(a);
		assert(b->IsDescendantOf(a));
		assert(c->IsDescendantOf(b));
		assert(c->IsDescendantOf(a));
		assert(!a->IsDescendantOf(b));
		assert(!a->IsDescendantOf(c));
		assert(!a->IsDescendantOf(a));
	}
}
