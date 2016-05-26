/********************************************************************************
** Form generated from reading UI file 'contourtools.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONTOURTOOLS_H
#define UI_CONTOURTOOLS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ContourTools
{
public:
    QWidget *dockWidgetContents;
    QGroupBox *groupBox_2;
    QWidget *widget;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_2;
    QCheckBox *checkBox;
    QCheckBox *FilledContourChk;
    QLabel *label_5;
    QDoubleSpinBox *contoursteps;
    QPushButton *LineColorBtn;
    QWidget *lineColorDisplay;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QDoubleSpinBox *errorRangeSpin;
    QLabel *label_6;
    QLabel *label_2;
    QLabel *label_3;
    QComboBox *colorRangeCB;
    QLineEdit *max;
    QLineEdit *newMin;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_4;
    QComboBox *zeroOffsetCB;
    QComboBox *ColorMapCB;
    QLabel *label;
    QLineEdit *minTest;

    void setupUi(QDockWidget *ContourTools)
    {
        if (ContourTools->objectName().isEmpty())
            ContourTools->setObjectName(QStringLiteral("ContourTools"));
        ContourTools->resize(204, 497);
        ContourTools->setMinimumSize(QSize(100, 400));
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QStringLiteral("dockWidgetContents"));
        groupBox_2 = new QGroupBox(dockWidgetContents);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setGeometry(QRect(11, 67, 165, 141));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy);
        groupBox_2->setMinimumSize(QSize(0, 80));
        widget = new QWidget(groupBox_2);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(13, 56, 16, 28));
        groupBox_3 = new QGroupBox(groupBox_2);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        groupBox_3->setGeometry(QRect(-1, 0, 171, 151));
        gridLayout_2 = new QGridLayout(groupBox_3);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        checkBox = new QCheckBox(groupBox_3);
        checkBox->setObjectName(QStringLiteral("checkBox"));
        checkBox->setChecked(true);

        gridLayout_2->addWidget(checkBox, 0, 0, 1, 2);

        FilledContourChk = new QCheckBox(groupBox_3);
        FilledContourChk->setObjectName(QStringLiteral("FilledContourChk"));
        FilledContourChk->setChecked(true);

        gridLayout_2->addWidget(FilledContourChk, 1, 0, 1, 2);

        label_5 = new QLabel(groupBox_3);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout_2->addWidget(label_5, 3, 0, 1, 1);

        contoursteps = new QDoubleSpinBox(groupBox_3);
        contoursteps->setObjectName(QStringLiteral("contoursteps"));
        contoursteps->setEnabled(false);

        gridLayout_2->addWidget(contoursteps, 3, 1, 1, 1);

        LineColorBtn = new QPushButton(groupBox_3);
        LineColorBtn->setObjectName(QStringLiteral("LineColorBtn"));

        gridLayout_2->addWidget(LineColorBtn, 2, 0, 1, 1);

        lineColorDisplay = new QWidget(groupBox_3);
        lineColorDisplay->setObjectName(QStringLiteral("lineColorDisplay"));
        lineColorDisplay->setMaximumSize(QSize(16777215, 10));

        gridLayout_2->addWidget(lineColorDisplay, 2, 1, 1, 1);

        groupBox = new QGroupBox(dockWidgetContents);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(10, 301, 181, 151));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setHorizontalSpacing(3);
        gridLayout->setVerticalSpacing(5);
        errorRangeSpin = new QDoubleSpinBox(groupBox);
        errorRangeSpin->setObjectName(QStringLiteral("errorRangeSpin"));
        errorRangeSpin->setEnabled(false);

        gridLayout->addWidget(errorRangeSpin, 8, 2, 1, 1);

        label_6 = new QLabel(groupBox);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout->addWidget(label_6, 8, 0, 1, 1);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QStringLiteral("label_2"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(label_2, 4, 0, 1, 1);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QStringLiteral("label_3"));
        sizePolicy1.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(label_3, 3, 0, 1, 1);

        colorRangeCB = new QComboBox(groupBox);
        colorRangeCB->setObjectName(QStringLiteral("colorRangeCB"));

        gridLayout->addWidget(colorRangeCB, 1, 0, 1, 3);

        max = new QLineEdit(groupBox);
        max->setObjectName(QStringLiteral("max"));
        max->setEnabled(true);
        sizePolicy.setHeightForWidth(max->sizePolicy().hasHeightForWidth());
        max->setSizePolicy(sizePolicy);
        max->setMaximumSize(QSize(1000, 16777215));

        gridLayout->addWidget(max, 3, 1, 1, 2);

        newMin = new QLineEdit(groupBox);
        newMin->setObjectName(QStringLiteral("newMin"));

        gridLayout->addWidget(newMin, 4, 2, 1, 1);

        layoutWidget = new QWidget(dockWidgetContents);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 220, 171, 61));
        verticalLayout_2 = new QVBoxLayout(layoutWidget);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        label_4 = new QLabel(layoutWidget);
        label_4->setObjectName(QStringLiteral("label_4"));
        sizePolicy.setHeightForWidth(label_4->sizePolicy().hasHeightForWidth());
        label_4->setSizePolicy(sizePolicy);

        verticalLayout_2->addWidget(label_4);

        zeroOffsetCB = new QComboBox(layoutWidget);
        zeroOffsetCB->setObjectName(QStringLiteral("zeroOffsetCB"));

        verticalLayout_2->addWidget(zeroOffsetCB);

        ColorMapCB = new QComboBox(dockWidgetContents);
        ColorMapCB->setObjectName(QStringLiteral("ColorMapCB"));
        ColorMapCB->setGeometry(QRect(90, 10, 107, 22));
        label = new QLabel(dockWidgetContents);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(18, 11, 65, 20));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy2);
        label->setMaximumSize(QSize(16777215, 20));
        minTest = new QLineEdit(dockWidgetContents);
        minTest->setObjectName(QStringLiteral("minTest"));
        minTest->setEnabled(true);
        minTest->setGeometry(QRect(50, 280, 114, 22));
        ContourTools->setWidget(dockWidgetContents);

        retranslateUi(ContourTools);

        QMetaObject::connectSlotsByName(ContourTools);
    } // setupUi

    void retranslateUi(QDockWidget *ContourTools)
    {
        ContourTools->setWindowTitle(QApplication::translate("ContourTools", "Contour", 0));
        groupBox_2->setTitle(QString());
        groupBox_3->setTitle(QApplication::translate("ContourTools", "Contour Lines", 0));
        checkBox->setText(QApplication::translate("ContourTools", "Show Lines", 0));
        FilledContourChk->setText(QApplication::translate("ContourTools", "Filled Contour", 0));
        label_5->setText(QApplication::translate("ContourTools", "Waves", 0));
        LineColorBtn->setText(QApplication::translate("ContourTools", "Line Color", 0));
        groupBox->setTitle(QApplication::translate("ContourTools", "Error scale", 0));
        label_6->setText(QApplication::translate("ContourTools", "Waves", 0));
        label_2->setText(QApplication::translate("ContourTools", "Min", 0));
        label_3->setText(QApplication::translate("ContourTools", "Max", 0));
#ifndef QT_NO_TOOLTIP
        colorRangeCB->setToolTip(QApplication::translate("ContourTools", "<html><head/><body><p><span style=\" font-weight:600;\">Auto -</span> Set min and max range to mean +- 3 stand<span style=\" font-weight:600;\">ard deviations</span><br/>This is the default and usually creates a map with good resolution.<br/><br/><span style=\" font-weight:600;\">Min/Max -</span> Use the min and max value.  &lt;br&gt;Will create a compressed map if there are large spikes and dips on top of a smoother surface.<span style=\" font-weight:600;\"><br/><br/>Fraction of Wavelenth -</span> User specified range.</p></body></html>", 0));
#endif // QT_NO_TOOLTIP
        label_4->setText(QApplication::translate("ContourTools", "Zero Offet:", 0));
        label->setText(QApplication::translate("ContourTools", "Color pallet", 0));
    } // retranslateUi

};

namespace Ui {
    class ContourTools: public Ui_ContourTools {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONTOURTOOLS_H
