#include "util.h"
#include <iostream>

#define EQ(a, b) (abs((a)-(b)) < 0.00001)

inline int sgn(double x) {
	return (x < 0) ? -1 : 1;
}

int intersectLineCircle(Point2f a, Point2f b, Point2f c, double r, Point2f& p1, Point2f& p2) {
	// translate circle to (0|0)
	double ax = a.x - c.x;
	double ay = a.y - c.y;
	double bx = b.x - c.x;
	double by = b.y - c.y;

	double dx = bx - ax; // Delta X
	double dy = by - ay; // Delta Y
	double dr = sqrt(dx*dx + dy*dy); // slope
	double d = ax * by - bx * ay; // ?
	
	double disc = (r*r) * (dr*dr) - (d*d); // discriminant
	
	if (disc < 0) {
		return 0; // no intersection
	}

	double discsq = sqrt(disc); // sqrt of discriminant
	
	// calculate first intersection and translate back
	p1.x = (d * dy + sgn(dy) * dx * discsq) / (dr*dr) + c.x;
	p1.y = (-d * dx + abs(dy) * discsq) / (dr*dr) + c.y;
	
	if (disc == 0) {
		p2.x = p1.x;
		p2.y = p1.y;
		
		return 1;
	}
	
	// calculate second intersection and translate back
	p2.x = (d * dy - sgn(dy) * dx * discsq) / (dr*dr) + c.x;
	p2.y = (-d * dx - abs(dy) * discsq) / (dr*dr) + c.y;
	
	return 2;
}


bool inSegment(Point2f a, Point2f b, Point2f c) {
	/*double cross = (c.y - a.y) * (b.x - a.x) - (c.x - a.x) * (b.y - a.y);
	if (abs(cross) > 0.1) return false;

	double dot = (c.x - a.x) * (b.x - a.x) + (c.y - a.y) * (b.y - a.y);
	if (dot < 0) return false;

	double sqlen = (b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y);
	if (dot > sqlen) return false;

	return true;*/

	return (min(a.x, b.x)-1 <= c.x && c.x <= max(a.x, b.x)+1) &&
		(min(a.y, b.y)-1 <= c.y && c.y <= max(a.y, b.y)+1);
}


int intersectLineSegment(Point2f a, Point2f b, Point2f c, double r, Point2f& p1, Point2f& p2) {
	int n = intersectLineCircle(a, b, c, r, p1, p2);
	
	if (n == 0) {
		return 0;
	}

	std::cout << "DEBUG: " << p1.x << " " << p1.y << " " << p2.x << " " << p2.y << std::endl;
	
	Point2f p1a, p2a;

	n = 0;
	if (inSegment(a, b, p1)) {
		p1a = p1;
		n++;
		std::cout << "p1 drin" << std::endl;
	}

	if (inSegment(a, b, p2)) {
		if (n == 0) {
			p1a = p2;
		}
		p2a = p2;
		n++;
		std::cout << "p2 drin" << std::endl;
	}

	p1 = p1a;
	p2 = p2a;
	
	return n;
}