// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the VECTOR_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// VECTOR_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#pragma once

#ifdef VECTOR_EXPORTS
#define VECTOR_API __declspec(dllexport)
#else
#define VECTOR_API __declspec(dllimport)
#endif

#include "Number.h"

// This class is exported from the dll
class VECTOR_API Vector
{
	Number x_;
	Number y_;
public:
	Vector();
	Vector(const Number&, const Number&);

	Vector operator+(const Vector&) const;
	Vector operator-(const Vector&) const;
	Vector& operator+=(const Vector&);
	Vector& operator-=(const Vector&);

	Number radius() const;
	Number angle() const;
};


extern "C" VECTOR_API Vector* CreateZeroVector();

extern "C" VECTOR_API Vector* CreateVector(Number, Number);

extern "C" VECTOR_API Number GetRadius(const Vector*);

extern "C" VECTOR_API void DeleteVector(Vector*);