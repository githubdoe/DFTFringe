#ifndef OUTLINEHELPDOCWIDGET_H
#define OUTLINEHELPDOCWIDGET_H

#include <QDockWidget>

namespace Ui {
class outlineHelpDocWidget;
}

class outlineHelpDocWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit outlineHelpDocWidget(QWidget *parent = 0);
    ~outlineHelpDocWidget();

private:
    Ui::outlineHelpDocWidget *ui;
};

#endif // OUTLINEHELPDOCWIDGET_H
