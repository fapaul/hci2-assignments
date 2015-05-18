#include <cmath>
#include <cstdio>

using namespace std;

class Point {
public:
	double x,y;
	
	Point() {
		x = 0;
		y = 0;
	}
	
	Point(double _x, double _y) {
		x = _x;
		y = _y;
	}
};

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


int main() {
	/*Point a(-1, 0);
	Point b(0, 1);
	Point c(1, 1);
	double r = 1.0;*/
	Point a(0, -3);
	Point b(1, 0);
	Point c(-2, -4);
	double r = 5.0;
	
	Point p1, p2;
	
	int intersections = intersectLineCircle(a, b, c, r, p1, p2);
	printf("Intersections: %d\n", intersections);
	
	if (intersections == 1) {
		printf("(%f %f)\n", p1.x, p1.y);
	} else if (intersections == 2) {
		printf("(%f %f) (%f %f)\n", p1.x, p1.y, p2.x, p2.y);
	}
	
	return 0;
}