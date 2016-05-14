#ifndef STATSVIEW_H
#define STATSVIEW_H

#include <QDialog>
#include <QSplitter>

namespace Ui {
class statsView;
}

class statsView : public QDialog
{
    Q_OBJECT

public:
    explicit statsView(QWidget * w1, QWidget *w2, QWidget * w3, QWidget *parent = 0);
    QSplitter *m_hs;
    QSplitter *m_vs;
    ~statsView();

private slots:

    void on_showWftNames_toggled(bool checked);

    void on_includeInputWavefronts_toggled(bool checked);

    void on_removeOutliers_toggled(bool checked);

private:
    Ui::statsView *ui;
};

#endif // STATSVIEW_H
