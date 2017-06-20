#pragma warning (push)
#pragma warning(disable:4996)
#include <algorithm>
#pragma warning(pop)
#include <vector>
#include <string>
#include <iterator>
#include <functional>
#include <iostream>
#include <sstream>
#include <map>
#define BOOST_RESULT_OF_USE_DECLTYPE
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/adaptor/formatted.hpp>
#include <boost/range/adaptor/indexed.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/algorithm/cxx11/any_of.hpp>
#include <boost/range/algorithm/transform.hpp>
#include <boost/range/algorithm/max_element.hpp>
#include <boost/range/algorithm/sort.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/phoenix.hpp>

using namespace std;
using namespace boost::adaptors;
using boost::copy;
using boost::max_element;
using boost::sort;
using boost::transform;
using boost::algorithm::any_of;
using namespace boost::phoenix::arg_names;

enum class Gender : uint8_t { Male, Female };

struct Person {
	string name;
	short age;
	Gender gender;
	int salary;
};

ostream& operator<<(ostream& out, const Person& p)
{
	out << p.name 
		<< " (age " << p.age << ", " 
		<< (p.gender == Gender::Male ? "male" : "female") 
		<< ")";
	return out;
}

const vector<Person> people = {
	{ "Ivan", 25, Gender::Male, 15'000 },
	{ "Peter", 35, Gender::Male, 35'000 },
	{ "Masha", 24, Gender::Female, 20'000 },
	{ "Dasha", 33, Gender::Female, 50'000 },
	{ "Ivan", 37, Gender::Male, 50'000 },
	{ "Sveta", 28, Gender::Female, 45'000 },
	{ "Ivan", 20, Gender::Male, 30'000 },
	{ "Constantine", 23, Gender::Male, 35'000 },
	{ "Eugene", 35, Gender::Male, 40'000 },
};

void WriteToStdout() {
	for (size_t i = 0; i < people.size(); ++i) {
		cout << people[i] << "\n";
	}

	for (auto & person : people) {
		cout << person << "\n";
	}

	copy(people, ostream_iterator<Person>(cout, "\n"));
}

void FindTheOldestOne() {
	// Raw loop version
	{
		if (!people.empty()) {
			size_t oldest = 0;
			for (size_t i = 1; i < people.size(); ++i) {
				if (people[i].age > people[oldest].age) {
					oldest = i;
				}
			}
			cout << "The oldest one is " << people[oldest] << "\n";
		}
		else {
			cout << "No people\n";
		}
	}

	// max_element
	{
		auto orderedByAge = [](auto & lhs, auto & rhs) { 
			return lhs.age < rhs.age; 
		};
		auto oldest = max_element(people, orderedByAge);
		if (oldest != people.end()) 
			cout << "The oldest one is " << *oldest << "\n";
		else
			cout << "No people\n";
	}
}

// Print all women aged between 20 and 30,
// then print all men aged between 25 and 40
void SelectPeopleByAgeAndGender() {
	cout << "---Raw loop---\n";
	{
		for (size_t i = 0; i < people.size(); ++i) {
			if (people[i].gender == Gender::Female &&
				people[i].age >= 20 && people[i].age <= 30) {
				cout << people[i] << "\n";
			}
		}
		for (size_t i = 0; i < people.size(); ++i) {
			if (people[i].gender == Gender::Male &&
				people[i].age >= 25 && people[i].age <= 40) {
				cout << people[i] << "\n";
			}
		}
	}
	cout << "---Range-based for---\n";
	{
		for (auto& person : people) {
			if ((person.gender == Gender::Female) && 
				(person.age >= 20 && person.age <= 30)) {
				cout << person << "\n";
			}
		}
		for (auto& person : people) {
			if ((person.gender == Gender::Male) && 
				(person.age >= 25 && person.age <= 40)) {
				cout << person << "\n";
			}
		}
	}
	cout << "---Range filtering---\n";
	{
		auto ByGenderAndAge = [](Gender g, int minAge, int maxAge) {
			return [=](auto& p) { 
				return (p.gender == g) && p.age >= minAge && p.age <= maxAge; 
			};
		};

		auto ToStdout = ostream_iterator<Person>(cout, "\n");
		copy(people | filtered(ByGenderAndAge(Gender::Female, 20, 30)), ToStdout);
		copy(people | filtered(ByGenderAndAge(Gender::Male, 25, 40)), ToStdout);
	}
}

decltype(auto) ToRef()
{
	return [](auto && value) { return ref(value); };
}

void Indexing()
{
	using ConstPersonRef = reference_wrapper<const Person>;
	cout << "Sorted people with initial indices\n";
	{
		using IndexedPersonRef = pair<ConstPersonRef, size_t>;

		vector<IndexedPersonRef> indexedPeople;
		indexedPeople.reserve(people.size());

		for (size_t i = 0; i < people.size(); ++i) {
			indexedPeople.emplace_back(people[i], i + 1);
		}
		sort(indexedPeople.begin(), indexedPeople.end(), [](auto& lhs, auto & rhs) {
			return lhs.first.get().name < rhs.first.get().name; 
		});
		for (size_t i = 0; i < indexedPeople.size(); ++i) {
			cout << indexedPeople[i].second << ". " << indexedPeople[i].first << "\n";
		}
	}
	cout << "Sorted people with initial indices\n";
	{
		auto indexedPeople = people | transformed(ToRef()) | indexed(1);
		using IndexedPersonRef = decltype(indexedPeople)::value_type;

		vector<IndexedPersonRef> peopleToSort;
		peopleToSort.reserve(people.size());
		peopleToSort.assign(indexedPeople.begin(), indexedPeople.end());

		auto ByName = [](auto & lhs, auto & rhs) { 
			return lhs.value().get().name < rhs.value().get().name; 
		};

		auto ToStringWithIndex = [](auto & person) {
			ostringstream out;
			out << person.index() << ". " << person.value();
			return out.str();
		};

		sort(peopleToSort, ByName);

		boost::transform(peopleToSort, ostream_iterator<string>(cout, "\n"), ToStringWithIndex);
	}
}

void AnyOf()
{
	{
		bool hasWomenOlderThan30 = false;
		for (size_t i = 0; i < people.size(); ++i) {
			if (people[i].age > 30 && people[i].gender == Gender::Female) {
				hasWomenOlderThan30 = true;
				break;
			}
		}
		if (hasWomenOlderThan30)
			cout << "There are women older than 30\n";
		else
			cout << "There are no women above 30\n";
	}
	{
		auto olderThan = [](int age) {
			return [=](auto& person) { return person.age > age; }; 
		};
		auto isFemale = [](auto& person) { return person.gender == Gender::Female; };
		if (any_of(people | filtered(olderThan(30)), isFemale))
			cout << "There are women older than 30\n";
		else
			cout << "There are no women above 30\n";
	}
}

void Transform()
{
	int numbers[] = {1, 0, 5, 3, -4, -3, 2, 5};
	{
		vector<string> strings;
		for (size_t i = 0; i < sizeof(numbers) / sizeof(numbers[0]); ++i) {
			if (numbers[i] > 0)
				strings.push_back(to_string(numbers[i]));
		}
	}
	{
		vector<string> strings;
		for (auto n: numbers) {
			if (n > 0)
				strings.push_back(to_string(n));
		}
	}
	{
		vector<string> strings;
		auto ToString = [](int i) { return to_string(i); };
		auto IsPositive = [](int i) {return i > 0; };
		transform(numbers | filtered(IsPositive), back_inserter(strings), ToString);
	}
	{
		vector<string> strings;
		auto ToString = [](int i) { return to_string(i); };
		boost::transform(numbers | filtered(arg1 > 0), back_inserter(strings), ToString);
	}
}

int main()
{
	WriteToStdout();
	FindTheOldestOne();
	SelectPeopleByAgeAndGender();
	Indexing();
	AnyOf();
	Transform();
}