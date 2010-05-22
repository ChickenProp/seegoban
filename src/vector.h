#ifndef PH_VECTOR_H
#define PH_VECTOR_H

namespace ph {

class vec2f {
public:
	float x;
	float y;

	vec2f() : x(0.f), y(0.f) {}
	vec2f(const float _x, const float _y) : x(_x), y(_y) {}

	float lengthSquared () const {
		return x*x + y*y;
	}
	float length () const {
		return sqrt(x*x + y*y);
	}

	float distanceSquared (const vec2f &v) const {
		return (*this - v).lengthSquared();
	}
	float distance (const vec2f &v) const {
		return (*this - v).length();
	}

	vec2f normalize () const {
		if (lengthSquared() == 0)
			return *this;
		else
			return *this / length();
	}

	float dot (const vec2f &v) const {
		return x*v.x + y*v.y;
	}

	vec2f project(const vec2f &v) const {
		return v.normalize() * (this->dot(v.normalize()));
	}
	vec2f project(const vec2f &u, const vec2f &v) const {
		return u + (*this - u).project(v - u);
	}

	vec2f operator+ (const vec2f &v) const {
		return vec2f(x + v.x, y + v.y);
	}
	vec2f operator- (const vec2f &v) const {
		return vec2f(x - v.x, y - v.y);
	}
	vec2f operator- () const {
		return vec2f(-x, -y);
	}
	vec2f operator* (const float a) const {
		return vec2f(a*x, a*y);
	}
	vec2f operator/ (const float a) const {
		return vec2f(x/a, y/a);
	}

	vec2f& operator+= (const vec2f v) {
		x += v.x;
		y += v.y;
		return *this;
	}
	vec2f& operator-= (const vec2f v) {
		x -= v.x;
		y -= v.y;
		return *this;
	}
	vec2f& operator*= (const float a) {
		x *= a;
		y *= a;
		return *this;
	}
	vec2f& operator/= (const float a) {
		x /= a;
		y /= a;
		return *this;
	}

#ifdef SFML_VECTOR2_HPP
	template <typename T>
	vec2f(const sf::Vector2<T> &v) : x(v.x), y(v.y) {}

	template <typename T>
	operator sf::Vector2<T>() {
		return sf::Vector2<T>((T)x, (T)y);
	}
#endif

};

inline vec2f operator* (const float a, const vec2f &v) {
	return vec2f(a*v.x, a*v.y);
}

} //namespace ph

#endif
