/********************************************************************************
** Form generated from reading UI file 'wavefrontnulldlg.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WAVEFRONTNULLDLG_H
#define UI_WAVEFRONTNULLDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListWidget>

QT_BEGIN_NAMESPACE

class Ui_waveFrontNullDlg
{
public:
    QHBoxLayout *horizontalLayout;
    QListWidget *listWidget;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *waveFrontNullDlg)
    {
        if (waveFrontNullDlg->objectName().isEmpty())
            waveFrontNullDlg->setObjectName(QStringLiteral("waveFrontNullDlg"));
        waveFrontNullDlg->resize(265, 501);
        horizontalLayout = new QHBoxLayout(waveFrontNullDlg);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        listWidget = new QListWidget(waveFrontNullDlg);
        listWidget->setObjectName(QStringLiteral("listWidget"));

        horizontalLayout->addWidget(listWidget);

        buttonBox = new QDialogButtonBox(waveFrontNullDlg);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Vertical);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        horizontalLayout->addWidget(buttonBox);


        retranslateUi(waveFrontNullDlg);
        QObject::connect(buttonBox, SIGNAL(accepted()), waveFrontNullDlg, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), waveFrontNullDlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(waveFrontNullDlg);
    } // setupUi

    void retranslateUi(QDialog *waveFrontNullDlg)
    {
        waveFrontNullDlg->setWindowTitle(QApplication::translate("waveFrontNullDlg", "Wavefront Null Enables", 0));
#ifndef QT_NO_TOOLTIP
        waveFrontNullDlg->setToolTip(QApplication::translate("waveFrontNullDlg", "<html><head/><body><p>Specifies what to remove from the wavefront.  Normally this is the first 8 Zernike terms.</p><p>Except the x and y astig.</p></body></html>", 0));
#endif // QT_NO_TOOLTIP
    } // retranslateUi

};

namespace Ui {
    class waveFrontNullDlg: public Ui_waveFrontNullDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WAVEFRONTNULLDLG_H
