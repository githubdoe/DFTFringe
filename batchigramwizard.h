#ifndef BATCHIGRAMWIZARD_H
#define BATCHIGRAMWIZARD_H

#include <QWizard>
#include <QPushButton>
#include <QRadioButton>
#include <QStringList>


namespace Ui {
class batchIgramWizard;
}
class QListWidget;
class batchIgramWizard : public QWizard
{
    Q_OBJECT
    enum { Page_Intro, Page_Process};

public:
    static QRadioButton *autoRb;
    static QRadioButton *manualRb;
    static QPushButton *goPb;
    explicit batchIgramWizard(QStringList files, QWidget *parent = 0 , Qt::WindowFlags flags = 0);
    ~batchIgramWizard();
    void listReady(QStringList list);
signals:
    void swapBathConnections(bool);


private:
    Ui::batchIgramWizard *ui;
};

class batchIntro : public QWizardPage
{

    Q_OBJECT

    public:
        batchIntro(QStringList files, QWidget *manger, QWidget *parent = 0);
        QListWidget *filesList;
public slots:
    void processBatch();
    void addFiles();
    void showContextMenu(const QPoint &pos);
    void eraseItem();
signals:
    void processBatchList(QStringList);
};
#endif // BATCHIGRAMWIZARD_H
