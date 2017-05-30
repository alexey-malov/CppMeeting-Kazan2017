#include <variant>
#include <cmath>
#include <iostream>
#include "variant_utils.h"

using namespace std;

struct NotAQudaraticEquation {};
using QuadraticEquationRoots = variant<
	std::monostate, // no roots
	double,			// one root
	pair<double, double>, // two roots
	NotAQudaraticEquation>; // not a quadratic equation

QuadraticEquationRoots Solve(double a, double b, double c) {
	if (abs(a) < DBL_EPSILON) {
		return NotAQudaraticEquation{};
	}
	else {
		auto disc = b * b - 4 * a * c;
		if (disc < 0) {
			return std::monostate{};
		} else {
			auto sqrtDisc = sqrt(disc);
			return make_pair(
				(-b - sqrtDisc) / (2 * a), 
				(-b + sqrtDisc) / (2 * a)
			);
		}
	}
}

int main()
{
	auto result = Solve(1, 0, -1);

	{
		struct ResultPrinter {
			void operator()(std::monostate) {
				cout << "No real roots\n";
			}
			void operator()(double x) {
				cout << "One root:" << x << "\n";
			}
			void operator()(pair<double, double> twoRoots) {
				cout << "Two roots: " << twoRoots.first << ", " << twoRoots.second << "\n";
			}
			void operator()(NotAQudaraticEquation) {
				cout << "This is not a quadratic equation\n";
			}
		};

		visit(ResultPrinter(), result);
	}
	{
		if (get_if<std::monostate>(&result))
			cout << "No real roots\n";
		else if (auto singleRoot = get_if<double>(&result))
			cout << "One root:" << *singleRoot << "\n";
		else if (auto twoRoots = get_if<pair<double, double>>(&result))
			cout << "Two roots: " << twoRoots->first << ", " << twoRoots->second << "\n";
		else if (get_if<NotAQudaraticEquation>(&result))
			cout << "This is not a quadratic equation\n";
	}
	{
		boost::variant<int, double> bv = 42;
		if (auto pInt = variant_utils::get_if<int>(&bv))
			cout << "Variant holds int value of " << *pInt << "\n";
		else if (auto pDouble = variant_utils::get_if<double>(&bv))
			cout << "Variant holds double value of " << *pDouble << "\n";
	}
}
