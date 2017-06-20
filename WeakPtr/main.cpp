#include <unordered_map>
#include <map>
#include <memory>

using namespace std;
struct Key
{

};
using KeyPtr = shared_ptr<Key>;
struct Value
{

};
using ValuePtr = shared_ptr<Value>;

struct SimpleCache
{
	ValuePtr GetValueForKey(const KeyPtr& key)
	{
		auto it = m_items.find(key);
		if (it != m_items.end())
		{
			return it->second;
		}
		else
		{
			return nullptr;
		}
	}
private:
	using ValueWeakPtr = weak_ptr<Value>;
	using KeyWeakPtr = weak_ptr<Key>;

	map<KeyWeakPtr, ValuePtr, std::owner_less<KeyWeakPtr>> m_items;
};

int main()
{

}