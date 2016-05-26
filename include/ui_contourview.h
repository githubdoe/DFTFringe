/********************************************************************************
** Form generated from reading UI file 'contourview.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONTOURVIEW_H
#define UI_CONTOURVIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <contourplot.h>

QT_BEGIN_NAMESPACE

class Ui_contourView
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QDoubleSpinBox *doubleSpinBox;
    QLabel *label_2;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer;
    ContourPlot *widget;

    void setupUi(QWidget *contourView)
    {
        if (contourView->objectName().isEmpty())
            contourView->setObjectName(QStringLiteral("contourView"));
        contourView->resize(400, 300);
        verticalLayout = new QVBoxLayout(contourView);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(contourView);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        doubleSpinBox = new QDoubleSpinBox(contourView);
        doubleSpinBox->setObjectName(QStringLiteral("doubleSpinBox"));
        doubleSpinBox->setMinimum(0.02);
        doubleSpinBox->setSingleStep(0.025);
        doubleSpinBox->setValue(0.1);

        horizontalLayout->addWidget(doubleSpinBox);

        label_2 = new QLabel(contourView);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout->addWidget(label_2);

        pushButton = new QPushButton(contourView);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        horizontalLayout->addWidget(pushButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);

        widget = new ContourPlot(contourView);
        widget->setObjectName(QStringLiteral("widget"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(10);
        sizePolicy.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(widget);


        retranslateUi(contourView);

        QMetaObject::connectSlotsByName(contourView);
    } // setupUi

    void retranslateUi(QWidget *contourView)
    {
        contourView->setWindowTitle(QApplication::translate("contourView", "Form", 0));
        label->setText(QApplication::translate("contourView", "Contour Lines every ", 0));
        label_2->setText(QApplication::translate("contourView", "waves", 0));
        pushButton->setText(QApplication::translate("contourView", "Show All Wavefronts", 0));
    } // retranslateUi

};

namespace Ui {
    class contourView: public Ui_contourView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONTOURVIEW_H
