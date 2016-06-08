//in header file
#include <QLabel>
class squareImage: public QLabel{
   Q_OBJECT
public:
   squareImage( QWidget* parent=0);
    int heightForWidth(int w) const {return w;}
   //other methods
};

