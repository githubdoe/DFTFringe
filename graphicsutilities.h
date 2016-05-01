#ifndef GRAPHICSUTILITIES_H
#define GRAPHICSUTILITIES_H

#include <QtCore>
#include "circleoutline.h"
#include <iostream>
#include <fstream>

void drawPlus(QPointF p, QPainter& dc);

CircleOutline readCircle(std::ifstream& file);
void writeCircle(std::ofstream& file, CircleOutline &circle);
#endif // GRAPHICSUTILITIES_H
