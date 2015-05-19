#include "util.h"

#define EQ(a, b) (abs((a)-(b)) < 0.00001)

inline int sgn(double x) {
	return (x < 0) ? -1 : 1;
}

int intersectLineCircle(Point a, Point b, Point c, double r, Point& p1, Point& p2) {
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


bool inSegment(Point a, Point b, Point c) {
	/*double cross = (c.y - a.y) * (b.x - a.x) - (c.x - a.x) * (b.y - a.y);
	if (abs(cross) > 0.1) return false;

	double dot = (c.x - a.x) * (b.x - a.x) + (c.y - a.y) * (b.y - a.y);
	if (dot < 0) return false;

	double sqlen = (b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y);
	if (dot > sqlen) return false;

	return true;*/

	return (min(a.x, b.x) <= c.x && c.x <= max(a.x, b.x)) &&
		(min(a.y, b.y) <= c.y && c.y <= max(a.y, b.y));
}


int intersectLineSegment(Point a, Point b, Point c, double r, Point& p1, Point& p2) {
	int n = intersectLineCircle(a, b, c, r, p1, p2);
	
	if (n == 0) {
		return 0;
	}
	
	Point p1a, p2a;

	n = 0;
	if (inSegment(a, b, p1)) {
		p1a = p1;
		n++;
	}

	if (inSegment(a, b, p2)) {
		if (n == 0) {
			p1a = p2;
		}
		p2a = p2;
		n++;
	}

	p1 = p1a;
	p2 = p2a;
	
	return n;
}