#pragma once
#include <vector>
#include "util.h"

// ------------------------------ ShapeType -----------------------------------------------------
enum ShapeType {
	RECT_SHAPE, CIRCLE_SHAPE, SHAPE_NUMBER
};

class Shape {
private:
	ShapeType shapeType;
	double left, top, right, bottom;
	double radius;
	Point center;

public:
	// RECT_SHAPE
	Shape(double left, double top, double right, double bottom);
	// CIRCLE_SHAPE
	Shape(double centerX, double centerY, double radius);

	ShapeType getType() const;
	double getLeft() const;
	double getTop() const;
	double getRight() const;
	double getBottom() const;
	const Point *getCenter() const;
	double getRadius() const;

	void resetCoord(double left, double top, double right, double bottom);
	void resetCoord(double centerX, double centerY);
};
typedef std::unordered_map<std::string, const Shape *> ShapeDataBase;

// ----------------------------- Collision -----------------------------------------------------------
class ShapeCollider {
private:
	typedef bool(ShapeCollider:: *CFUNC)(const Shape &s1, const Shape &s2) const;
	std::vector<std::vector<CFUNC>> colliderTable;

	bool RectAndRect(const Shape &s1, const Shape &s2) const;
	bool CircleAndCircle(const Shape &s1, const Shape &s2) const;
	bool RectAndCircle(const Shape &s1, const Shape &s2) const;

public:
	ShapeCollider();

	bool operator ()(const Shape &s1, const Shape &s2) {
		return (this->*colliderTable[s1.getType()][s2.getType()])(s1, s2);
	}
};
