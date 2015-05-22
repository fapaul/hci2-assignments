#pragma once

#include <opencv2/core/core.hpp>

using namespace cv;

int intersectLineCircle(Point2f a, Point2f b, Point2f c, double r, Point2f& p1, Point2f& p2);
int intersectLineSegment(Point2f a, Point2f b, Point2f c, double r, Point2f& p1, Point2f& p2);


