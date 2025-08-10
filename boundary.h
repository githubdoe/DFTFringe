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

// asbtract base class. Zero risk of slicing.
class boundary
{
    protected:
        boundary() = default;
        ~boundary() = default;
        // Copy operations
        boundary(const boundary&) = default;
        boundary& operator=(const boundary&) = default;
        // Move operations
        boundary(boundary&&) noexcept = default;
        boundary& operator=(boundary&&) noexcept = default;
    public:
        virtual void draw(QPainter& painter, double scale, double scale2 = -1.) = 0;
        virtual void enlarge(int del) = 0;
        virtual void translate(QPointF del) = 0;
        virtual void scale(double factor) = 0;
        virtual bool isValid() = 0;
        virtual bool isInside(QPointF& p, int offset = 0) = 0;
    private:
};
QDataStream & operator<<(QDataStream & stream, const boundary &outline);
#endif // BOUNDARY_H

