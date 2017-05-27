#include <optional>
#include <vector>
#include <string>
#include <iterator>
#include <iostream>
#define BOOST_RESULT_OF_USE_DECLTYPE
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/algorithm/cxx11/copy_if.hpp>
#include <boost/range/algorithm/transform.hpp>
#include <boost/range/algorithm/copy.hpp>

using namespace std;

struct Person
{
	string name;
	int age;
};
ostream& operator<<(ostream& out, const Person& person)
{
	out << person.name << "(" << person.age << ")";
	return out;
}

auto FindPeople(const vector<Person>& people, optional<string_view> name, optional<int> minAge, optional<int> maxAge)
{
	using namespace boost::adaptors;
	using boost::copy;

	auto byNameAndAge = [&](auto& person) {
		return (!name || person.name == *name)
			&& (!minAge || person.age >= *minAge)
			&& (!maxAge || person.age < maxAge);
	};
	auto intoRef = [](auto && x) { return ref(x); };

	vector<reference_wrapper<const Person>> results;
	results.reserve(people.size());

	copy(people 
			| filtered(byNameAndAge) 
			| transformed(intoRef), 
		back_inserter(results));

	return results;
}

int main()
{
	vector<Person> people = { 
		{ "Ivan", 25}, 
		{ "Peter", 35},
		{ "Ivan", 37 },
		{ "Vladimir", 16},
		{ "Ivan", 20 },
		{ "Constantine", 23}
	};

	auto foundPeople = FindPeople(people, nullopt, nullopt, nullopt);
	boost::copy(foundPeople, ostream_iterator<Person>(cout, "\n"));
}