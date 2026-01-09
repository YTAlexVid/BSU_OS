#ifndef VECTOR_LIBRARY_H
#define VECTOR_LIBRARY_H

#include "Vector_Export.h"
#include "Number.h"

class VECTOR_API Vector {
    Number x_;
    Number y_;
public:
    Vector();
    Vector(const Number &, const Number &);

    Vector operator+(const Vector &) const;
    Vector operator-(const Vector &) const;
    Vector & operator+=(const Vector &);
    Vector & operator-=(const Vector &);

    Number radius() const;
    Number angle() const;
};

extern "C" {
    VECTOR_API Vector* CreateZeroVector();
    VECTOR_API Vector* CreateVector(Number, Number);
    VECTOR_API Number GetRadius(const Vector*);
    VECTOR_API void DeleteVector(Vector*);
}

#endif //VECTOR_LIBRARY_H
