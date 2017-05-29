#include <variant>
#include <iostream>
#include <type_traits>
#include <iomanip>
#include <vector>



#include <variant>
#include <cmath>
#include <iostream>

using namespace std;

struct NotAQudaraticEquation {};
using QuadraticEquationRoots = variant<std::monostate, double, pair<double, double>, NotAQudaraticEquation>;

QuadraticEquationRoots Solve(double a, double b, double c) {
	if (abs(a) < DBL_EPSILON) {
		return NotAQudaraticEquation{};
	}
	else {
		auto disc = b * b - 4 * a * c;
		if (disc < 0) {
			return std::monostate{};
		} else {
			return make_pair((-b - sqrt(disc)) / (2 * a), (-b + sqrt(disc)) / (2 * a));
		}
	}
}

int main()
{
	auto result = Solve(1, 0, -1);

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
