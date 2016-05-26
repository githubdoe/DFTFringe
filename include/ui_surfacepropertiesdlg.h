/********************************************************************************
** Form generated from reading UI file 'surfacepropertiesdlg.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SURFACEPROPERTIESDLG_H
#define UI_SURFACEPROPERTIESDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_surfacePropertiesDlg
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QGroupBox *groupBox;
    QFormLayout *formLayout_2;
    QLabel *label;
    QSlider *xLight;
    QLabel *label_2;
    QSlider *yLight;
    QLabel *label_3;
    QSlider *zLight;
    QLabel *label_4;
    QSlider *diffuse;
    QLabel *label_5;
    QSlider *specular;
    QLabel *label_6;
    QSlider *ambientLight;
    QGroupBox *groupBox_2;
    QFormLayout *formLayout;
    QLabel *label_7;
    QSlider *surfaceAmbient;
    QLabel *label_8;
    QSlider *surfaceDiffuse;
    QLabel *label_9;
    QSlider *surfaceSpecular;
    QLabel *label_10;
    QSlider *surfaceShine;
    QLabel *label_11;
    QSlider *surfaceEmission;
    QSlider *red;
    QSlider *green;
    QSlider *blue;
    QGroupBox *groupBox_3;
    QHBoxLayout *horizontalLayout_2;
    QRadioButton *plastic;
    QRadioButton *Satin;
    QRadioButton *metalFoil;
    QRadioButton *glass;
    QRadioButton *matt;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *surfacePropertiesDlg)
    {
        if (surfacePropertiesDlg->objectName().isEmpty())
            surfacePropertiesDlg->setObjectName(QStringLiteral("surfacePropertiesDlg"));
        surfacePropertiesDlg->resize(599, 380);
        verticalLayout = new QVBoxLayout(surfacePropertiesDlg);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        groupBox = new QGroupBox(surfacePropertiesDlg);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        formLayout_2 = new QFormLayout(groupBox);
        formLayout_2->setObjectName(QStringLiteral("formLayout_2"));
        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, label);

        xLight = new QSlider(groupBox);
        xLight->setObjectName(QStringLiteral("xLight"));
        xLight->setMinimum(-100);
        xLight->setMaximum(100);
        xLight->setOrientation(Qt::Horizontal);

        formLayout_2->setWidget(0, QFormLayout::FieldRole, xLight);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QStringLiteral("label_2"));

        formLayout_2->setWidget(3, QFormLayout::LabelRole, label_2);

        yLight = new QSlider(groupBox);
        yLight->setObjectName(QStringLiteral("yLight"));
        yLight->setMinimum(0);
        yLight->setMaximum(500);
        yLight->setOrientation(Qt::Horizontal);

        formLayout_2->setWidget(3, QFormLayout::FieldRole, yLight);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QStringLiteral("label_3"));

        formLayout_2->setWidget(6, QFormLayout::LabelRole, label_3);

        zLight = new QSlider(groupBox);
        zLight->setObjectName(QStringLiteral("zLight"));
        zLight->setMinimum(-50);
        zLight->setMaximum(50);
        zLight->setOrientation(Qt::Horizontal);

        formLayout_2->setWidget(6, QFormLayout::FieldRole, zLight);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QStringLiteral("label_4"));

        formLayout_2->setWidget(8, QFormLayout::LabelRole, label_4);

        diffuse = new QSlider(groupBox);
        diffuse->setObjectName(QStringLiteral("diffuse"));
        diffuse->setMinimum(0);
        diffuse->setMaximum(100);
        diffuse->setOrientation(Qt::Horizontal);

        formLayout_2->setWidget(10, QFormLayout::FieldRole, diffuse);

        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QStringLiteral("label_5"));

        formLayout_2->setWidget(10, QFormLayout::LabelRole, label_5);

        specular = new QSlider(groupBox);
        specular->setObjectName(QStringLiteral("specular"));
        specular->setMaximum(100);
        specular->setOrientation(Qt::Horizontal);

        formLayout_2->setWidget(11, QFormLayout::FieldRole, specular);

        label_6 = new QLabel(groupBox);
        label_6->setObjectName(QStringLiteral("label_6"));

        formLayout_2->setWidget(11, QFormLayout::LabelRole, label_6);

        ambientLight = new QSlider(groupBox);
        ambientLight->setObjectName(QStringLiteral("ambientLight"));
        ambientLight->setMinimum(0);
        ambientLight->setMaximum(100);
        ambientLight->setOrientation(Qt::Horizontal);

        formLayout_2->setWidget(8, QFormLayout::FieldRole, ambientLight);


        horizontalLayout->addWidget(groupBox);

        groupBox_2 = new QGroupBox(surfacePropertiesDlg);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        formLayout = new QFormLayout(groupBox_2);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        label_7 = new QLabel(groupBox_2);
        label_7->setObjectName(QStringLiteral("label_7"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_7);

        surfaceAmbient = new QSlider(groupBox_2);
        surfaceAmbient->setObjectName(QStringLiteral("surfaceAmbient"));
        surfaceAmbient->setMaximum(100);
        surfaceAmbient->setOrientation(Qt::Horizontal);

        formLayout->setWidget(0, QFormLayout::FieldRole, surfaceAmbient);

        label_8 = new QLabel(groupBox_2);
        label_8->setObjectName(QStringLiteral("label_8"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_8);

        surfaceDiffuse = new QSlider(groupBox_2);
        surfaceDiffuse->setObjectName(QStringLiteral("surfaceDiffuse"));
        surfaceDiffuse->setOrientation(Qt::Horizontal);

        formLayout->setWidget(1, QFormLayout::FieldRole, surfaceDiffuse);

        label_9 = new QLabel(groupBox_2);
        label_9->setObjectName(QStringLiteral("label_9"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_9);

        surfaceSpecular = new QSlider(groupBox_2);
        surfaceSpecular->setObjectName(QStringLiteral("surfaceSpecular"));
        surfaceSpecular->setOrientation(Qt::Horizontal);

        formLayout->setWidget(2, QFormLayout::FieldRole, surfaceSpecular);

        label_10 = new QLabel(groupBox_2);
        label_10->setObjectName(QStringLiteral("label_10"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_10);

        surfaceShine = new QSlider(groupBox_2);
        surfaceShine->setObjectName(QStringLiteral("surfaceShine"));
        surfaceShine->setMaximum(128);
        surfaceShine->setOrientation(Qt::Horizontal);

        formLayout->setWidget(3, QFormLayout::FieldRole, surfaceShine);

        label_11 = new QLabel(groupBox_2);
        label_11->setObjectName(QStringLiteral("label_11"));

        formLayout->setWidget(4, QFormLayout::LabelRole, label_11);

        surfaceEmission = new QSlider(groupBox_2);
        surfaceEmission->setObjectName(QStringLiteral("surfaceEmission"));
        surfaceEmission->setOrientation(Qt::Horizontal);

        formLayout->setWidget(4, QFormLayout::FieldRole, surfaceEmission);

        red = new QSlider(groupBox_2);
        red->setObjectName(QStringLiteral("red"));
        red->setOrientation(Qt::Horizontal);

        formLayout->setWidget(5, QFormLayout::FieldRole, red);

        green = new QSlider(groupBox_2);
        green->setObjectName(QStringLiteral("green"));
        green->setOrientation(Qt::Horizontal);

        formLayout->setWidget(6, QFormLayout::FieldRole, green);

        blue = new QSlider(groupBox_2);
        blue->setObjectName(QStringLiteral("blue"));
        blue->setOrientation(Qt::Horizontal);

        formLayout->setWidget(7, QFormLayout::FieldRole, blue);


        horizontalLayout->addWidget(groupBox_2);


        verticalLayout->addLayout(horizontalLayout);

        groupBox_3 = new QGroupBox(surfacePropertiesDlg);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        horizontalLayout_2 = new QHBoxLayout(groupBox_3);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        plastic = new QRadioButton(groupBox_3);
        plastic->setObjectName(QStringLiteral("plastic"));

        horizontalLayout_2->addWidget(plastic);

        Satin = new QRadioButton(groupBox_3);
        Satin->setObjectName(QStringLiteral("Satin"));

        horizontalLayout_2->addWidget(Satin);

        metalFoil = new QRadioButton(groupBox_3);
        metalFoil->setObjectName(QStringLiteral("metalFoil"));

        horizontalLayout_2->addWidget(metalFoil);

        glass = new QRadioButton(groupBox_3);
        glass->setObjectName(QStringLiteral("glass"));

        horizontalLayout_2->addWidget(glass);

        matt = new QRadioButton(groupBox_3);
        matt->setObjectName(QStringLiteral("matt"));

        horizontalLayout_2->addWidget(matt);


        verticalLayout->addWidget(groupBox_3);

        buttonBox = new QDialogButtonBox(surfacePropertiesDlg);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(surfacePropertiesDlg);
        QObject::connect(buttonBox, SIGNAL(accepted()), surfacePropertiesDlg, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), surfacePropertiesDlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(surfacePropertiesDlg);
    } // setupUi

    void retranslateUi(QDialog *surfacePropertiesDlg)
    {
        surfacePropertiesDlg->setWindowTitle(QApplication::translate("surfacePropertiesDlg", "3D surface lighting and material properties", 0));
        groupBox->setTitle(QApplication::translate("surfacePropertiesDlg", "Lighting properties", 0));
        label->setText(QApplication::translate("surfacePropertiesDlg", "X", 0));
        label_2->setText(QApplication::translate("surfacePropertiesDlg", "Y", 0));
        label_3->setText(QApplication::translate("surfacePropertiesDlg", "Z", 0));
        label_4->setText(QApplication::translate("surfacePropertiesDlg", "Ambient", 0));
        label_5->setText(QApplication::translate("surfacePropertiesDlg", "Diffuse", 0));
        label_6->setText(QApplication::translate("surfacePropertiesDlg", "Specular:", 0));
        groupBox_2->setTitle(QApplication::translate("surfacePropertiesDlg", "Surface Properties", 0));
        label_7->setText(QApplication::translate("surfacePropertiesDlg", "Ambient", 0));
        label_8->setText(QApplication::translate("surfacePropertiesDlg", "Diffuse", 0));
        label_9->setText(QApplication::translate("surfacePropertiesDlg", "Specular:", 0));
        label_10->setText(QApplication::translate("surfacePropertiesDlg", "shine", 0));
        label_11->setText(QApplication::translate("surfacePropertiesDlg", "Emission", 0));
        groupBox_3->setTitle(QApplication::translate("surfacePropertiesDlg", "Predefined", 0));
        plastic->setText(QApplication::translate("surfacePropertiesDlg", "Plastic", 0));
        Satin->setText(QApplication::translate("surfacePropertiesDlg", "Satin", 0));
        metalFoil->setText(QApplication::translate("surfacePropertiesDlg", "Brushed Metal", 0));
        glass->setText(QApplication::translate("surfacePropertiesDlg", "Ceramic", 0));
        matt->setText(QApplication::translate("surfacePropertiesDlg", "Matt", 0));
    } // retranslateUi

};

namespace Ui {
    class surfacePropertiesDlg: public Ui_surfacePropertiesDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SURFACEPROPERTIESDLG_H
