#ifndef REGIONEDITTOOLS_H
#define REGIONEDITTOOLS_H

#include <QDockWidget>

namespace Ui {
class regionEditTools;
}

class regionEditTools : public QDockWidget
{
    Q_OBJECT

public:
    explicit regionEditTools(QWidget *parent = 0);
    ~regionEditTools();
    bool m_doLine;
    bool m_doFreeform;
    int currentNdx;
    void addRegion(QString text);
    void deleteRegion(int r);
    void clear();
signals:
    void addregion();
    void deleteregion(int);
    void selectRegion(int);
private slots:
    void on_delete_2_clicked();

    void on_freeform_clicked(bool checked);

    void on_line_clicked(bool checked);    void on_listWidget_clicked(const QModelIndex &index);

private:
    Ui::regionEditTools *ui;
};

#endif // REGIONEDITTOOLS_H
