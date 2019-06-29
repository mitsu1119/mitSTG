#include "collision.h"

// ----------------------------------------- RectShape -------------------------------------------------

Shape::Shape(double left, double top, double right, double bottom): left(left), top(top), right(right), bottom(bottom), shapeType(RECT_SHAPE) {
}

ShapeType Shape::getType() const {
	return shapeType;
}

double Shape::getLeft() const {
	return left;
}

double Shape::getTop() const {
	return top;
}

double Shape::getRight() const {
	return right;
}

double Shape::getBottom() const {
	return bottom;
}

// ----------------------------------------------- Collision ---------------------------------------------------
ShapeCollider::ShapeCollider() {
	colliderTable = std::vector<std::vector<CFUNC>>(SHAPE_NUMBER, std::vector<CFUNC>(SHAPE_NUMBER));
	colliderTable[RECT_SHAPE][RECT_SHAPE] = &ShapeCollider::RectAndRect;
}

bool ShapeCollider::RectAndRect(const Shape &s1, const Shape &s2) {
	if(s1.getRight() < s2.getLeft() || s1.getLeft() > s2.getRight() || s1.getBottom() < s2.getTop() || s1.getTop() > s2.getBottom()) return false;
	return true;
}


