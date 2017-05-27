#define HAS_STD_OPTIONAL 1
#define HAS_BOOST_OPTIONAL 1

#if HAS_STD_OPTIONAL
#include <optional>
#include <string_view>
#endif
#if HAS_BOOST_OPTIONAL
#include <boost/optional.hpp>
#endif

#define CATCH_CONFIG_MAIN
#include "../unit_testing/catch.hpp"
#include <iostream>
#include <algorithm>
#include <numeric>

#if HAS_STD_OPTIONAL
namespace stdoptional_demo_1
{
using std::optional;
using std::nullopt;

struct RGBAColor
{
	float r = 0.f;
	float g = 0.f;
	float b = 0.f;
	float a = 0.f;
};
namespace palette
{
	constexpr RGBAColor BLACK{ 0, 0, 0, 1 };
	constexpr RGBAColor RED{ 1, 0, 0, 1 };
	constexpr RGBAColor GREEN{ 0, 1, 0, 1 };
	constexpr RGBAColor BLUE{ 0, 0, 1, 1 };
	constexpr RGBAColor WHITE{ 1, 1, 1, 1 };
	constexpr RGBAColor YELLOW{ 1, 1, 0, 1 };
}

struct ICanvas
{
	virtual ~ICanvas() = default;
	virtual void BeginSolidFill(const RGBAColor& fillColor) = 0;
	virtual void EndFill() = 0;
	virtual void MoveTo(float x, float y) = 0;
	virtual void SetLineColor(const optional<RGBAColor>& lineColor) = 0;
	virtual void LineTo(float x, float y) = 0;
};

// Rectangular shape which may or may not have an outline and fill
struct Rect
{
	void Draw(ICanvas & canvas) const {
		if (fillColor) canvas.BeginSolidFill(*fillColor);

		canvas.SetLineColor(lineColor);
		canvas.MoveTo(left, top);
		canvas.LineTo(left + width, top);
		canvas.LineTo(left + width, top + height);
		canvas.LineTo(left, top + height);
		canvas.LineTo(left, top);

		if (fillColor)  canvas.EndFill();
	}

	float left = 0.f, top = 0.f, width = 0.f, height = 0.f;
	optional<RGBAColor> fillColor;
	optional<RGBAColor> lineColor;
};
void DrawPicture(ICanvas& canvas)
{
	Rect{ 10, 10, 20, 10, palette::YELLOW, palette::RED }.Draw(canvas);
	Rect{ 5, 5, 10, 10, nullopt, palette::GREEN }.Draw(canvas);
	Rect{ 20, 5, 5, 20, palette::BLUE }.Draw(canvas);
}
}

namespace stdoptional_demo_2
{

using namespace std;
// Lazy evaluation of signal characteristics
struct Signal
{
	double GetMaxAmplitude()const {
		if (!m_maxAmplitude) {
			m_maxAmplitude = m_samples.empty() ? 0.0
				: *max_element(m_samples.begin(), m_samples.end());
		}
		return *m_maxAmplitude;
	}

	void SetSamples(vector<double> samples) {
		m_samples = move(samples);
		m_maxAmplitude.reset();
	}
private:
	vector<double> m_samples;
	mutable optional<double> m_maxAmplitude;
};

}

#endif // #if HAS_STD_OPTIONAL

#if HAS_BOOST_OPTIONAL

#endif

