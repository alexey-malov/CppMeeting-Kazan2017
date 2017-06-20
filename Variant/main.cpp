#include <variant>
#include <cmath>
#include <iostream>
#include <limits>
#include <chrono>

#define HAS_BOOST 1
#if HAS_BOOST
#include "variant_utils.h"
#endif

#define HAS_CONSTEXPR_IF 0

using namespace std;

template<class T> struct always_false : std::false_type {};

struct NotAQudaraticEquation {};
using QuadraticEquationRoots = variant<
	std::monostate, // no roots
	double,			// one root
	pair<double, double>, // two roots
	NotAQudaraticEquation>; // not a quadratic equation

QuadraticEquationRoots Solve(double a, double b, double c) {
	if (abs(a) < std::numeric_limits<double>::epsilon()) {
		return NotAQudaraticEquation{};
	} else {
		auto disc = b * b - 4 * a * c;
		if (disc < 0) {
			return std::monostate{};
		} else if (disc < std::numeric_limits<double>::epsilon()) {
			return -b / (2 * a);
		} else {
			auto sqrtDisc = sqrt(disc);
			return make_pair((-b - sqrtDisc) / (2 * a), (-b + sqrtDisc) / (2 * a));
		}
	}
}

namespace account
{
enum class SubscriptionPeriod { MONTHLY, FORTNIGHT, QUARTERLY, HALF_YEAR, ANNUAL, CENTURY, };

struct OfflineState {};

struct AnonymousTrialState {};

struct LoggedTrialState {
	std::wstring accountId;
	std::wstring email;
};

struct SubscribedState {
	std::wstring accountId;
	std::wstring email;
	SubscriptionPeriod period;
	std::chrono::time_point<std::chrono::steady_clock> expirationTime;
	unsigned downloadsLeft;
};

using AccountState = boost::variant<
	OfflineState,
	AnonymousTrialState,
	LoggedTrialState,
	SubscribedState
>;
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
		auto result = Solve(1, 0, -1);
		if (get_if<std::monostate>(&result))
			cout << "No real roots\n";
		else if (auto singleRoot = get_if<double>(&result))
			cout << "One roo	t:" << *singleRoot << "\n";
		else if (auto twoRoots = get_if<pair<double, double>>(&result))
			cout << "Two roots: " << twoRoots->first << ", " << twoRoots->second << "\n";
		else if (get_if<NotAQudaraticEquation>(&result))
			cout << "This is not a quadratic equation\n";
		else
			cout << "Valueless by exception\n";
	}
#if HAS_CONSTEXPR_IF
	{
		auto result = Solve(1, 0, 0);
		auto visitor = [](auto&& arg) {
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, std::monostate>)
				cout << "No real roots\n";
			else if constexpr (std::is_same_v<T, double>)
				cout << "1 root:" << arg << "\n";
			else if constexpr (std::is_same_v<T, pair<double, double>>)
				cout << "Two roots: " << arg.first << ", " << arg.second << "\n";
			else if constexpr (std::is_same_v<T, NotAQudaraticEquation>)
				cout << "This is not a quadratic equation\n";
			else
				static_assert(always_false<T>::value, "non-exhaustive visitor!");
		};
		visit(visitor, result);
	}
#endif
#if HAS_BOOST
	{
		boost::variant<int, double> bv = 42;
		if (auto pInt = boost::get<int>(&bv))
			cout << "Variant holds int value of " << *pInt << "\n";
		else if (auto pDouble = variant_utils::get_if<double>(&bv))
			cout << "Variant holds double value of " << *pDouble << "\n";
	}
#endif
}
