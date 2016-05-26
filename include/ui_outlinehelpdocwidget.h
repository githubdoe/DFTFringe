/********************************************************************************
** Form generated from reading UI file 'outlinehelpdocwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OUTLINEHELPDOCWIDGET_H
#define UI_OUTLINEHELPDOCWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_outlineHelpDocWidget
{
public:
    QWidget *dockWidgetContents;
    QVBoxLayout *verticalLayout;
    QTextEdit *textEdit;

    void setupUi(QDockWidget *outlineHelpDocWidget)
    {
        if (outlineHelpDocWidget->objectName().isEmpty())
            outlineHelpDocWidget->setObjectName(QStringLiteral("outlineHelpDocWidget"));
        outlineHelpDocWidget->resize(173, 527);
        outlineHelpDocWidget->setFloating(false);
        outlineHelpDocWidget->setAllowedAreas(Qt::LeftDockWidgetArea);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QStringLiteral("dockWidgetContents"));
        verticalLayout = new QVBoxLayout(dockWidgetContents);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        textEdit = new QTextEdit(dockWidgetContents);
        textEdit->setObjectName(QStringLiteral("textEdit"));
        textEdit->setReadOnly(true);

        verticalLayout->addWidget(textEdit);

        outlineHelpDocWidget->setWidget(dockWidgetContents);

        retranslateUi(outlineHelpDocWidget);

        QMetaObject::connectSlotsByName(outlineHelpDocWidget);
    } // setupUi

    void retranslateUi(QDockWidget *outlineHelpDocWidget)
    {
        outlineHelpDocWidget->setWindowTitle(QApplication::translate("outlineHelpDocWidget", "outline Help", 0));
        textEdit->setHtml(QApplication::translate("outlineHelpDocWidget", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">1. Load an interferogram image file.</span></p>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">Left click the mouse on one edge of the mirror.</span></p>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">With the left button down drag the mouse to the other side of the mirror.</span></p>\n"
"<p style="
                        "\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">scroll wheel will zoom the image.</span></p>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">[Shift] drag moves the circle.</span></p>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">[control] scroll wheel enlareges/shrink the circle.</span></p>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">Use the adjust buttons to modify the outline.</span></p>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; font-weight:600; color:#0000ff;\""
                        ">Outline Hot Keys:</span></p>\n"
"<table border=\"0\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;\" cellspacing=\"2\" cellpadding=\"0\">\n"
"<tr>\n"
"<td>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">Undo</span></p></td>\n"
"<td>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">[ctrl z] </span></p></td>\n"
"<td></td></tr>\n"
"<tr>\n"
"<td>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:600;\">Redo</span></p></td>\n"
"<td>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">[ctrl y]</span></p></td>\n"
"<td></td></tr>\n"
"<tr>\n"
""
                        "<td>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:600;\">Mouve outline</span></p></td>\n"
"<td>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">up/down/left/right arrow keys.</span></p></td>\n"
"<td></td></tr>\n"
"<tr>\n"
"<td>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:600;\">decrease/Increase Radius</span></p></td>\n"
"<td>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">[ctrl -/+] keys </span></p></td>\n"
"<td></td></tr>\n"
"<tr>\n"
"<td>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-inden"
                        "t:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:600;\">zoom out/in</span></p></td>\n"
"<td>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">-/+ keys </span></p></td>\n"
"<td></td></tr>\n"
"<tr>\n"
"<td>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:600;\">zoom to full image</span></p></td>\n"
"<td>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">f key</span></p></td>\n"
"<td></td></tr>\n"
"<tr>\n"
"<td>\n"
"<p style=\" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:600;\">scroll image </span></p></td>\n"
"<td>\n"
"<p style=\" margin-top:12px; ma"
                        "rgin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">shift arrow keys.</span></p></td>\n"
"<td></td></tr></table></body></html>", 0));
    } // retranslateUi

};

namespace Ui {
    class outlineHelpDocWidget: public Ui_outlineHelpDocWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OUTLINEHELPDOCWIDGET_H
