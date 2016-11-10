/******************************************************************************
**
**  Copyright 2016 Dale Eason
**  This file is part of DFTFringe
**  is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation version 3 of the License

** DFTFringe is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with DFTFringe.  If not, see <http://www.gnu.org/licenses/>.

****************************************************************************/
#ifndef BOUNDARY_H
#define BOUNDARY_H
#include <QPainter>
class boundary
{
    public:
        boundary();
        virtual ~boundary();
        virtual  void draw(QPainter& painter, double scale, double scale2 = -1.) = 0;
        virtual void enlarge(int del) = 0;
        virtual void translate(QPointF del) = 0;
        virtual void scale(double factor) = 0;
        virtual bool isValid() = 0;
        virtual bool isInside(QPointF& p, int offset = 0) =0;
    protected:
    private:
};
QDataStream & operator<<(QDataStream & stream, const boundary &outline);
#endif // BOUNDARY_H

