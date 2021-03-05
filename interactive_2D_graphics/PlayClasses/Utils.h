#ifndef MAIN_UTILS_H
#define MAIN_UTILS_H
#include "../Constants.h"

#include <cmath>
#include <vector>
#include <iostream>

double getPointsDistance(Point p1, Point p2);
double detNearestPointT(Point main_point, std::shared_ptr<std::vector<PointT>>& points, int & nearest_index);
Point PointT2Point(PointT p);

Pixel blend(Pixel oldPixel, Pixel newPixel);

void drawAsset(Image &screen, const std::shared_ptr<Image> &asset, int global_x, int global_y);
void drawSaveAsset(Image &screen, const std::shared_ptr<Image> &asset, int global_x, int global_y);
void drawTrSaveAsset(Image &screen, const std::shared_ptr<Image> &asset, int global_x, int global_y);


int getTransitionDirection(PointT p);


#endif //MAIN_UTILS_H
