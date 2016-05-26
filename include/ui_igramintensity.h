/********************************************************************************
** Form generated from reading UI file 'igramintensity.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_IGRAMINTENSITY_H
#define UI_IGRAMINTENSITY_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <intensityplot.h>

QT_BEGIN_NAMESPACE

class Ui_igramIntensity
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton;
    QCheckBox *showRed;
    QCheckBox *showBlue;
    QCheckBox *showGreen;
    QCheckBox *checkBox;
    QSpacerItem *horizontalSpacer;
    intensityPlot *plot;

    void setupUi(QDialog *igramIntensity)
    {
        if (igramIntensity->objectName().isEmpty())
            igramIntensity->setObjectName(QStringLiteral("igramIntensity"));
        igramIntensity->resize(668, 358);
        verticalLayout = new QVBoxLayout(igramIntensity);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetMaximumSize);
        pushButton = new QPushButton(igramIntensity);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        horizontalLayout->addWidget(pushButton);

        showRed = new QCheckBox(igramIntensity);
        showRed->setObjectName(QStringLiteral("showRed"));
        showRed->setChecked(true);

        horizontalLayout->addWidget(showRed);

        showBlue = new QCheckBox(igramIntensity);
        showBlue->setObjectName(QStringLiteral("showBlue"));
        showBlue->setChecked(true);

        horizontalLayout->addWidget(showBlue);

        showGreen = new QCheckBox(igramIntensity);
        showGreen->setObjectName(QStringLiteral("showGreen"));
        showGreen->setChecked(true);

        horizontalLayout->addWidget(showGreen);

        checkBox = new QCheckBox(igramIntensity);
        checkBox->setObjectName(QStringLiteral("checkBox"));

        horizontalLayout->addWidget(checkBox);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);

        plot = new intensityPlot(igramIntensity);
        plot->setObjectName(QStringLiteral("plot"));

        verticalLayout->addWidget(plot);


        retranslateUi(igramIntensity);

        QMetaObject::connectSlotsByName(igramIntensity);
    } // setupUi

    void retranslateUi(QDialog *igramIntensity)
    {
        igramIntensity->setWindowTitle(QApplication::translate("igramIntensity", "Interferogram Intensity profile", 0));
        pushButton->setText(QApplication::translate("igramIntensity", "Save as Image", 0));
        showRed->setText(QApplication::translate("igramIntensity", "Red", 0));
        showBlue->setText(QApplication::translate("igramIntensity", "Blue", 0));
        showGreen->setText(QApplication::translate("igramIntensity", "Green", 0));
        checkBox->setText(QApplication::translate("igramIntensity", "Stay on top screen", 0));
    } // retranslateUi

};

namespace Ui {
    class igramIntensity: public Ui_igramIntensity {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_IGRAMINTENSITY_H
