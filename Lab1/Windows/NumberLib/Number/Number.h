#pragma once

class Number {
	double number_;
public:
	Number() : number_(0) {}
	Number(double number) : number_(number) {}


	double getNumber() const;
	void setNumber(double);
	Number operator+(const Number&) const;
	Number operator-(const Number&) const;
	Number operator*(const Number&) const;
	Number operator/(const Number&) const;
	Number& operator+= (const Number&);
	Number& operator-= (const Number&);
	Number root() const;
	static Number atan2(Number x, Number y);
};

Number makeNumber(double);
