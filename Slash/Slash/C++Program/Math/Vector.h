#pragma once
#include<Windows.h>

struct Vector2_LONG {
public:

	LONG x;
	LONG y;

	Vector2_LONG(LONG x = 0, LONG y = 0);

	static Vector2_LONG Zero() { return Vector2_LONG(0, 0); }

};
