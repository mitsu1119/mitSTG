#include "collision.h"

// ----------------------------------------- RectShape -------------------------------------------------

Shape::Shape(double left, double top, double right, double bottom): left(left), top(top), right(right), bottom(bottom), radius(0), shapeType(RECT_SHAPE) {
}

Shape::Shape(double centerX, double centerY, double radius): center(Point(centerX, centerY)), left(0), top(0), right(0), bottom(0), radius(radius), shapeType(CIRCLE_SHAPE) {
}

ShapeType Shape::getType() const {
	return shapeType;
}

double Shape::getLeft() const {
	if(shapeType != RECT_SHAPE) return -1;
	return left;
}

double Shape::getTop() const {
	if(shapeType != RECT_SHAPE) return -1;
	return top;
}

double Shape::getRight() const {
	if(shapeType != RECT_SHAPE) return -1;
	return right;
}

double Shape::getBottom() const {
	if(shapeType != RECT_SHAPE) return -1;
	return bottom;
}

const Point *Shape::getCenter() const {
	if(shapeType != CIRCLE_SHAPE) return nullptr;
	return &center;
}

double Shape::getRadius() const {
	if(shapeType != CIRCLE_SHAPE) return -1;
	return radius;
}

void Shape::resetCoord(double left, double top, double right, double bottom) {
	this->left = left, this->top = top, this->right = right, this->bottom = bottom;
}

void Shape::resetCoord(double centerX, double centerY) {
	center.setX(centerX);
	center.setY(centerY);
}

// ----------------------------------------------- Collision ---------------------------------------------------
ShapeCollider::ShapeCollider() {
	colliderTable = std::vector<std::vector<CFUNC>>(SHAPE_NUMBER, std::vector<CFUNC>(SHAPE_NUMBER));
	colliderTable[RECT_SHAPE][RECT_SHAPE] = &ShapeCollider::RectAndRect;
	colliderTable[CIRCLE_SHAPE][CIRCLE_SHAPE] = &ShapeCollider::CircleAndCircle;
	colliderTable[RECT_SHAPE][CIRCLE_SHAPE] = &ShapeCollider::RectAndCircle;
	colliderTable[CIRCLE_SHAPE][RECT_SHAPE] = &ShapeCollider::RectAndCircle;
}

bool ShapeCollider::RectAndRect(const Shape &s1, const Shape &s2) const{
	if(s1.getRight() < s2.getLeft() || s1.getLeft() > s2.getRight() || s1.getBottom() < s2.getTop() || s1.getTop() > s2.getBottom()) return false;
	return true;
}

bool ShapeCollider::CircleAndCircle(const Shape &s1, const Shape &s2) const {
	const Point *c1 = s1.getCenter(), *c2 = s2.getCenter();
	if(std::pow(c1->getX() - c2->getX(), 2) + std::pow(c1->getY() - c2->getY(), 2) <= std::pow(s1.getRadius() + s2.getRadius(), 2)) return true;
	return false;
}

bool ShapeCollider::RectAndCircle(const Shape &s1, const Shape &s2) const {
	const Point *c1;
	double xc, yc, r;
	double x1, x2, y1, y2;
	if(s1.getType() == RECT_SHAPE) {
		c1 = s1.getCenter();
		xc = c1->getX(), yc = c1->getY(), r = s1.getRadius();
		x1 = s2.getLeft(), x2 = s2.getRight(), y1 = s2.getTop(), y2 = s2.getBottom();
	} else {
		c1 = s2.getCenter();
		xc = c1->getX(), yc = c1->getY(), r = s2.getRadius();
		x1 = s1.getLeft(), x2 = s2.getRight(), y1 = s1.getTop(), y2 = s1.getBottom();
	}

	if(xc > x1 && xc < x2 && yc > y1 - r && yc < y2 + r) return true;
	if(xc > x1 - r && xc < x2 + r && yc > y1 && yc < y2) return true;
	if(std::pow(x1 - xc, 2) + std::pow(y1 - yc, 2) < std::pow(r, 2)) return true;
	if(std::pow(x2 - xc, 2) + std::pow(y1 - yc, 2) < std::pow(r, 2)) return true;
	if(std::pow(x2 - xc, 2) + std::pow(y2 - yc, 2) < std::pow(r, 2)) return true;
	if(std::pow(x1 - xc, 2) + std::pow(y2 - yc, 2) < std::pow(r, 2)) return true;
	return false;
}
