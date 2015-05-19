#pragma once

#include <opencv2/core/core.hpp>

using namespace cv;

int intersectLineCircle(Point a, Point b, Point c, double r, Point& p1, Point& p2);
int intersectLineSegment(Point a, Point b, Point c, double r, Point& p1, Point& p2);


