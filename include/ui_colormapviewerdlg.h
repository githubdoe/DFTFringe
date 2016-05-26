/********************************************************************************
** Form generated from reading UI file 'colormapviewerdlg.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COLORMAPVIEWERDLG_H
#define UI_COLORMAPVIEWERDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_colorMapViewerDlg
{
public:
    QGridLayout *gridLayout;
    QPushButton *browsePb;
    QLineEdit *path;
    QDialogButtonBox *buttonBox;
    QLabel *label;
    QListWidget *listWidget;
    QPushButton *aboutpb;

    void setupUi(QDialog *colorMapViewerDlg)
    {
        if (colorMapViewerDlg->objectName().isEmpty())
            colorMapViewerDlg->setObjectName(QStringLiteral("colorMapViewerDlg"));
        colorMapViewerDlg->resize(796, 659);
        gridLayout = new QGridLayout(colorMapViewerDlg);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        browsePb = new QPushButton(colorMapViewerDlg);
        browsePb->setObjectName(QStringLiteral("browsePb"));

        gridLayout->addWidget(browsePb, 0, 2, 1, 1);

        path = new QLineEdit(colorMapViewerDlg);
        path->setObjectName(QStringLiteral("path"));

        gridLayout->addWidget(path, 0, 1, 1, 1);

        buttonBox = new QDialogButtonBox(colorMapViewerDlg);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 3, 0, 1, 2);

        label = new QLabel(colorMapViewerDlg);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        listWidget = new QListWidget(colorMapViewerDlg);
        listWidget->setObjectName(QStringLiteral("listWidget"));

        gridLayout->addWidget(listWidget, 1, 0, 1, 3);

        aboutpb = new QPushButton(colorMapViewerDlg);
        aboutpb->setObjectName(QStringLiteral("aboutpb"));

        gridLayout->addWidget(aboutpb, 3, 2, 1, 1);


        retranslateUi(colorMapViewerDlg);
        QObject::connect(buttonBox, SIGNAL(accepted()), colorMapViewerDlg, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), colorMapViewerDlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(colorMapViewerDlg);
    } // setupUi

    void retranslateUi(QDialog *colorMapViewerDlg)
    {
        colorMapViewerDlg->setWindowTitle(QApplication::translate("colorMapViewerDlg", "Dialog", 0));
        browsePb->setText(QApplication::translate("colorMapViewerDlg", "...", 0));
        label->setText(QApplication::translate("colorMapViewerDlg", "Directory:", 0));
        aboutpb->setText(QApplication::translate("colorMapViewerDlg", "About", 0));
    } // retranslateUi

};

namespace Ui {
    class colorMapViewerDlg: public Ui_colorMapViewerDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COLORMAPVIEWERDLG_H
