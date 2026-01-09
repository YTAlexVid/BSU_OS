#include <iostream>
#include "Number.h"
#include "Vector.h" // <-- not needed for first method (explicit_no_include)
#include "windows.h"

void explicit_no_include() {
	//This method works fully without #Include "Vector.h", its needed only for implicit usage

	HMODULE hMod = LoadLibraryA("Vector.dll");
	if (hMod == NULL) {
		std::cerr << "Failed to load Vector.dll" << std::endl;
		return;
	}

	typedef void* (*CreateVectorFunc)(Number, Number);
	typedef Number(*GetRadiusFunc)(const void*);
	typedef void (*DeleteVectorFunc)(void*);
	
	CreateVectorFunc createVector = (CreateVectorFunc)GetProcAddress(hMod, "CreateVector");
	GetRadiusFunc getRadius = (GetRadiusFunc)GetProcAddress(hMod, "GetRadius");
	DeleteVectorFunc deleteVector = (DeleteVectorFunc)GetProcAddress(hMod, "DeleteVector");

	if (createVector == NULL) {
		std::cerr << "Failed to get createVector function address" << std::endl;
		FreeLibrary(hMod);
		return;
	}

	void* vec1 = createVector(Number(3), Number(4));
	void* vec2 = createVector(Number(7), Number(7));

	std::cout << "Vector 1: (" << getRadius(vec1).getNumber() << ")" << std::endl;
	std::cout << "Vector 2: (" << getRadius(vec2).getNumber() << ")" << std::endl;

	deleteVector(vec1);
	deleteVector(vec2);
	FreeLibrary(hMod);

	return;
}

void explicit_include() {
	HMODULE hMod = LoadLibraryA("Vector.dll");
	if (hMod == NULL) {
		std::cerr << "Failed to load Vector.dll" << std::endl;
		return;
	}
	typedef Vector* (*CreateVectorFunc)(Number, Number);
	CreateVectorFunc createVector = (CreateVectorFunc)GetProcAddress(hMod, "CreateVector");
	if (createVector == NULL) {
		std::cerr << "Failed to get createVector function address" << std::endl;
		FreeLibrary(hMod);
		return;
	}
	Vector* vec1 = createVector(Number(3), Number(4));
	Vector* vec2 = createVector(Number(7), Number(7));
	Vector vec3 = *vec1 + *vec2;
	std::cout << "Vector 1: (" << vec1->radius().getNumber() << ", " << vec1->angle().getNumber() << ")" << std::endl;
	std::cout << "Vector 2: (" << vec2->radius().getNumber() << ", " << vec2->angle().getNumber() << ")" << std::endl;
	std::cout << "Vector 3 (sum): (" << vec3.radius().getNumber() << ", " << vec3.angle().getNumber() << ")" << std::endl;
	delete vec1;
	delete vec2;
	FreeLibrary(hMod);
	return;
}

void implcit() {
	Vector vec1 = Vector(Number(3), Number(4));
	Vector vec2 = Vector(Number(7), Number(7));
	Vector vec3 = vec1 + vec2;
	std::cout << "Vector 1: (" << vec1.radius().getNumber() << ", " << vec1.angle().getNumber() << ")" << std::endl;
	std::cout << "Vector 2: (" << vec2.radius().getNumber() << ", " << vec2.angle().getNumber() << ")" << std::endl;
	std::cout << "Vector 3 (sum): (" << vec3.radius().getNumber() << ", " << vec3.angle().getNumber() << ")" << std::endl;
}

void number_test() {
	Number n1 = Number(3);
	Number n2 = Number(4);
	Number n3 = n1 + n2;
	std::cout << "Sum: " << n3.getNumber() << std::endl;
	Number n4 = n2 - n1;
	std::cout << "Sub: " << n4.getNumber() << std::endl;
	Number n5 = n1 * n2;
	std::cout << "Mul: " << n5.getNumber() << std::endl;
	Number n6 = n2 / n1;
	std::cout << "Div: " << n6.getNumber() << std::endl;
	Number n7 = n1.root();
	std::cout << "Sqrt: " << n7.getNumber() << std::endl;
	Number n8 = Number::atan2(n1, n2);
	std::cout << "Atan2: " << n8.getNumber() << std::endl;
}

int main()
{
	std::cout << "Explicit no include:" << std::endl;
	explicit_no_include();
	std::cout << "Explicit include:" << std::endl;
	explicit_include();
	std::cout << "Implicit include:" << std::endl;
	implcit();
	std::cout << "Number test:" << std::endl;
	number_test();
}