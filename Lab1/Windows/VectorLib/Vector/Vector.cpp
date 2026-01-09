#include "framework.h"
#include "NumberConstants.h"
#include "Vector.h"
Vector::Vector() : x_(ZERO), y_(ZERO) {}
Vector::Vector(const Number& x, const Number& y) : x_(x), y_(y) {}
Vector Vector::operator+(const Vector& v) const {
	return Vector(this->x_ + v.x_, this->y_ + v.y_);
}
Vector Vector::operator-(const Vector& v) const {
	return Vector(this->x_ - v.x_, this->y_ - v.y_);
}
Vector& Vector::operator+=(const Vector& v) {
	this->x_ += v.x_;
	this->y_ += v.y_;
	return *this;
}
Vector& Vector::operator-=(const Vector& v) {
	this->x_ -= v.x_;
	this->y_ -= v.y_;
	return *this;
}
Number Vector::radius() const {
	return (this->x_ * this->x_ + this->y_ * this->y_).root();
}
Number Vector::angle() const {
	return Number::atan2(this->y_, this->x_);
}
extern "C" Vector* CreateZeroVector() {
	return new Vector();
}
extern "C" Vector* CreateVector(Number a, Number b) {
	return new Vector(a, b);
}
extern "C" Number GetRadius(const Vector* v) {
	return v->radius();
}
extern "C" void DeleteVector(Vector* p) {
	delete p;
}
