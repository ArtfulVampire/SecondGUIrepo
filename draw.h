#ifndef DRAW_H
#define DRAW_H
#include <QPaintDevice>
#include <QPainter>
#include <QSvgGenerator>
#include <QPixmap>
#include <iostream>
#include <coord.cpp>
#include <cmath>

using namespace std;
using namespace coords;

void draw(QPaintDevice * dev, double ** arr, int spLength, int num);

#endif // DRAW_H
