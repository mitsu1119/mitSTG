#pragma once
#include <vector>

// ------------------------------ ShapeType -----------------------------------------------------
enum ShapeType {
	RECT_SHAPE, SHAPE_NUMBER
};

class Shape {
private:
	ShapeType shapeType;
	double left, top, right, bottom;

public:
	// RECT_SHAPE
	Shape(double left, double top, double right, double bottom);

	ShapeType getType() const;
	double getLeft() const;
	double getTop() const;
	double getRight() const;
	double getBottom() const;
};

// ----------------------------- Collision -----------------------------------------------------------
class ShapeCollider {
private:
	typedef bool(ShapeCollider:: *CFUNC)(const Shape &s1, const Shape &s2);
	std::vector<std::vector<CFUNC>> colliderTable;

	bool RectAndRect(const Shape &s1, const Shape &s2);

public:
	ShapeCollider();

	bool operator ()(const Shape &s1, const Shape &s2) {
		return (this->*colliderTable[s1.getType()][s2.getType()])(s1, s2);
	}
};
