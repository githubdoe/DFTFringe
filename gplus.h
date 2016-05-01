#ifndef GPLUS_H
#define GPLUS_H
#include <QPainter>
#include <QPointF>
class gPlus
{
    public:
        gPlus(QPointF p);
        gPlus();
        virtual ~gPlus();
        void draw(QPainter& dc, double scale, int size = 10);
        QPointF m_p;
    protected:
    private:
};

#endif // GPLUS_H
