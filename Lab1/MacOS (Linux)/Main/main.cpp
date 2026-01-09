#include <iostream>
#include "Number.h"
#include "Vector.h" // <-- not needed for first method (explicit_no_include)
#include <dlfcn.h>

void explicit_no_include() {
	//This method works fully without #include "Vector.h", its needed only for implicit usage

	void* handle = dlopen("./libVector.so", RTLD_LAZY);
	if (!handle) {
		std::cerr << "Failed to load libVector.so: " << dlerror() << std::endl;
		return;
	}

	typedef void* (*CreateVectorFunc)(Number, Number);
	typedef Number(*GetRadiusFunc)(const void*);
	typedef void (*DeleteVectorFunc)(void*);

	CreateVectorFunc createVector = (CreateVectorFunc)dlsym(handle, "CreateVector");
	GetRadiusFunc getRadius = (GetRadiusFunc)dlsym(handle, "GetRadius");
	DeleteVectorFunc deleteVector = (DeleteVectorFunc)dlsym(handle, "DeleteVector");

	const char* dlsym_error = dlerror();
	if (dlsym_error) {
		std::cerr << "Failed to get function address: " << dlsym_error << std::endl;
		dlclose(handle);
		return;
	}

	void* vec1 = createVector(Number(3), Number(4));
	void* vec2 = createVector(Number(7), Number(7));

	std::cout << "Vector 1: (" << getRadius(vec1).getNumber() << ")" << std::endl;
	std::cout << "Vector 2: (" << getRadius(vec2).getNumber() << ")" << std::endl;

	deleteVector(vec1);
	deleteVector(vec2);
	dlclose(handle);

	return;
}

void explicit_include() {
	void* handle = dlopen("./libVector.so", RTLD_LAZY);
	if (!handle) {
		std::cerr << "Failed to load libVector.so: " << dlerror() << std::endl;
		return;
	}
	typedef Vector* (*CreateVectorFunc)(Number, Number);
	CreateVectorFunc createVector = (CreateVectorFunc)dlsym(handle, "CreateVector");
	const char* dlsym_error = dlerror();
	if (dlsym_error) {
		std::cerr << "Failed to get createVector function address: " << dlsym_error << std::endl;
		dlclose(handle);
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
	dlclose(handle);
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