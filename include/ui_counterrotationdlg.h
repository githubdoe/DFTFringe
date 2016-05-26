/********************************************************************************
** Form generated from reading UI file 'counterrotationdlg.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COUNTERROTATIONDLG_H
#define UI_COUNTERROTATIONDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QRadioButton>

QT_BEGIN_NAMESPACE

class Ui_CounterRotationDlg
{
public:
    QGridLayout *gridLayout;
    QLabel *label_2;
    QLabel *label;
    QLabel *fileName;
    QLineEdit *counterRotation;
    QDialogButtonBox *buttonBox;
    QHBoxLayout *horizontalLayout;
    QRadioButton *CW;
    QRadioButton *CCW;

    void setupUi(QDialog *CounterRotationDlg)
    {
        if (CounterRotationDlg->objectName().isEmpty())
            CounterRotationDlg->setObjectName(QStringLiteral("CounterRotationDlg"));
        CounterRotationDlg->resize(644, 87);
        gridLayout = new QGridLayout(CounterRotationDlg);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label_2 = new QLabel(CounterRotationDlg);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 0, 0, 1, 1);

        label = new QLabel(CounterRotationDlg);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 0, 2, 1, 1);

        fileName = new QLabel(CounterRotationDlg);
        fileName->setObjectName(QStringLiteral("fileName"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(10);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(fileName->sizePolicy().hasHeightForWidth());
        fileName->setSizePolicy(sizePolicy);

        gridLayout->addWidget(fileName, 1, 0, 1, 1);

        counterRotation = new QLineEdit(CounterRotationDlg);
        counterRotation->setObjectName(QStringLiteral("counterRotation"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(1);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(counterRotation->sizePolicy().hasHeightForWidth());
        counterRotation->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(counterRotation, 1, 2, 1, 1);

        buttonBox = new QDialogButtonBox(CounterRotationDlg);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 2, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        CW = new QRadioButton(CounterRotationDlg);
        CW->setObjectName(QStringLiteral("CW"));

        horizontalLayout->addWidget(CW);

        CCW = new QRadioButton(CounterRotationDlg);
        CCW->setObjectName(QStringLiteral("CCW"));

        horizontalLayout->addWidget(CCW);


        gridLayout->addLayout(horizontalLayout, 2, 2, 1, 1);


        retranslateUi(CounterRotationDlg);
        QObject::connect(buttonBox, SIGNAL(accepted()), CounterRotationDlg, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), CounterRotationDlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(CounterRotationDlg);
    } // setupUi

    void retranslateUi(QDialog *CounterRotationDlg)
    {
        CounterRotationDlg->setWindowTitle(QApplication::translate("CounterRotationDlg", "Counter Rotation", 0));
        label_2->setText(QApplication::translate("CounterRotationDlg", "File:", 0));
        label->setText(QApplication::translate("CounterRotationDlg", "Counter Rotation angle", 0));
        fileName->setText(QString());
        CW->setText(QApplication::translate("CounterRotationDlg", "CW", 0));
        CCW->setText(QApplication::translate("CounterRotationDlg", "CCW", 0));
    } // retranslateUi

};

namespace Ui {
    class CounterRotationDlg: public Ui_CounterRotationDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COUNTERROTATIONDLG_H
