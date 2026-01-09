#include "Number.h"
#include <cmath>

double Number::getNumber() const {
	return this->number_;
}
void Number::setNumber(double number) {
	this->number_ = number;
}
Number Number::operator+(const Number& b) const {
	return Number(this->number_ + b.number_);
}
Number Number::operator-(const Number& b) const {
	return Number(this->number_ - b.number_);
}
Number Number::operator*(const Number& b) const {
	return Number(this->number_ * b.number_);
}
Number Number::operator/(const Number& b) const {
	if (b.number_ == 0) throw "Division by zero";
	return Number(this->number_ / b.number_);
}
Number& Number::operator+= (const Number& b) {
	this->number_ += b.number_;
	return *this;
}
Number& Number::operator-= (const Number& b) {
	this->number_ -= b.number_;
	return *this;
}

Number Number::root() const {
	return Number(sqrt(this->number_));
}

Number Number::atan2(Number x, Number y) {
	if (x.number_ == 0 && y.number_ == 0) return Number(0);
	return Number(std::atan2(x.number_, y.number_));
}


Number makeNumber(double number) {
	return Number(number);
}