#include "pdfcalibrationdlg.h"
#include "ui_pdfcalibrationdlg.h"
#include <qimage.h>
#include <QPainter>
#include <QPrinter>
#include <QTextEdit>
#include <QPen>
#include <QDesktopServices>
PdfCalibrationDLg::PdfCalibrationDLg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PdfCalibrationDLg)
{
    ui->setupUi(this);

    QImage testImage(3000,100,QImage::Format_ARGB32);
    QPainter tpainter(&testImage);
    for (int i = 1; i < 30; ++i){
        int x = i * 100;
        tpainter.drawText(x,60,QString("{%1}").arg(x));
    }
    QTextEdit *editor = new QTextEdit;
    editor->setHtml("<p>Enter the most complete number you see at the left back in the calibraion dialog.<b>  Close this viewer</p>");

    QTextCursor cur = editor->textCursor();
    QTextDocument *document = editor->document();
    document->addResource(QTextDocument::ImageResource, QUrl("image"), testImage);
    cur.movePosition(QTextCursor::Up);
    cur.insertImage("image");

    // The actual PDF printer.
    QPrinter printer(QPrinter::QPrinter::HighResolution);


    //setup printer
    printer.setColorMode( QPrinter::Color );
    printer.setFullPage( false );
    printer.setOutputFormat( QPrinter::PdfFormat );
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setOutputFileName( "calibrate.pdf");
    editor->print(&printer);
    QDesktopServices::openUrl(QUrl("calibrate.pdf"));
}

PdfCalibrationDLg::~PdfCalibrationDLg()
{
    delete ui;
}

void PdfCalibrationDLg::on_spinBox_valueChanged(int arg1)
{
    m_rightCrop = arg1;
}


void PdfCalibrationDLg::on_RIghtSideNumber_editingFinished()
{
    m_rightCrop = ui->RIghtSideNumber->value();
}

