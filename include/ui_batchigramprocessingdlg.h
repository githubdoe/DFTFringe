/********************************************************************************
** Form generated from reading UI file 'batchigramprocessingdlg.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BATCHIGRAMPROCESSINGDLG_H
#define UI_BATCHIGRAMPROCESSINGDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_batchIgramProcessingDlg
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QHBoxLayout *horizontalLayout;
    QRadioButton *manualRb;
    QRadioButton *autoRb;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *batchIgramProcessingDlg)
    {
        if (batchIgramProcessingDlg->objectName().isEmpty())
            batchIgramProcessingDlg->setObjectName(QStringLiteral("batchIgramProcessingDlg"));
        batchIgramProcessingDlg->resize(421, 236);
        verticalLayout = new QVBoxLayout(batchIgramProcessingDlg);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        label = new QLabel(batchIgramProcessingDlg);
        label->setObjectName(QStringLiteral("label"));
        label->setWordWrap(true);

        verticalLayout->addWidget(label);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        manualRb = new QRadioButton(batchIgramProcessingDlg);
        manualRb->setObjectName(QStringLiteral("manualRb"));

        horizontalLayout->addWidget(manualRb);

        autoRb = new QRadioButton(batchIgramProcessingDlg);
        autoRb->setObjectName(QStringLiteral("autoRb"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(autoRb->sizePolicy().hasHeightForWidth());
        autoRb->setSizePolicy(sizePolicy);
        autoRb->setMaximumSize(QSize(16777215, 30));

        horizontalLayout->addWidget(autoRb);


        verticalLayout->addLayout(horizontalLayout);

        buttonBox = new QDialogButtonBox(batchIgramProcessingDlg);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(batchIgramProcessingDlg);
        QObject::connect(buttonBox, SIGNAL(accepted()), batchIgramProcessingDlg, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), batchIgramProcessingDlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(batchIgramProcessingDlg);
    } // setupUi

    void retranslateUi(QDialog *batchIgramProcessingDlg)
    {
        batchIgramProcessingDlg->setWindowTitle(QApplication::translate("batchIgramProcessingDlg", "Batch processing of Igrams", 0));
        label->setText(QApplication::translate("batchIgramProcessingDlg", "<html><head/><body><p>Selet many interferograms from the directory and process each one.</p><p>You can process many Igrams in either Manual Mode: Where the program will load an igram and wait for you to adjust the outline and then press done.<br/>Then the program will process that igram and wait for you to adjust the DFT center filter. When you press compute surface it will create the analysis and wavefront.</p><p>In Auto mode it will not wait but process the igram with the previously used outline and center filter values. </p><p>You can leave this dialog up in manual mode and change to auto at any time in the process.</p></body></html>", 0));
        manualRb->setText(QApplication::translate("batchIgramProcessingDlg", "Manual", 0));
        autoRb->setText(QApplication::translate("batchIgramProcessingDlg", "Auto", 0));
    } // retranslateUi

};

namespace Ui {
    class batchIgramProcessingDlg: public Ui_batchIgramProcessingDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BATCHIGRAMPROCESSINGDLG_H
