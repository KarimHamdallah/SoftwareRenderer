#include <cmath>

namespace Math
{
	class Mathf
	{
	public:
		static float min(float a, float b) { return (a < b) ? a : b; }
		static float max(float a, float b) { return (a > b) ? a : b; }
	};

	struct vec2
	{
		float x, y;

		static vec2 Sub(const vec2& v1, const vec2& v2) { return { v1.x - v2.x, v1.y - v2.y }; }
		static float Cross(const vec2& v1, const vec2& v2) { return v1.x * v2.y - v1.y * v2.x; }
	};
}