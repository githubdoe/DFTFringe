#include "ronchicomparedialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QWidget>
#include <QGuiApplication>
#include <QScreen>
RonchiCompareDialog::RonchiCompareDialog(const QImage& img1, const QString name1, const QImage &img2,
                                         const QString name2, QWidget* parent)
    : QDialog(parent), m_q1(img1), m_q2(img2)
{
    setWindowTitle(tr("Ronchi Comparison Overlay"));


    QScreen *screen = QGuiApplication::primaryScreen();
    int dlgW = static_cast<int>(screen->availableGeometry().width() * 0.75);
    int dlgH = static_cast<int>(screen->availableGeometry().height() * 0.85);
    resize(dlgW, dlgH);
    // Main layout for the dialog
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // 1. Color Legend Labels at the top
    QHBoxLayout* legendLayout = new QHBoxLayout();

    m_baseLabel = new QLabel(name1);
    m_baseLabel->setStyleSheet("color: #00FFFF; font-weight: bold; font-size: 18px;");
    // 1. Get the current font
    QFont font = m_baseLabel->font();

    // 2. Set the desired font size in points (device-independent)
    font.setPointSize(18);
    m_baseLabel->setFont(font);

    m_compLabel = new QLabel(name2);
    m_compLabel->setStyleSheet("color: #FF0000; font-weight: bold; font-size: 18px;");

    legendLayout->addWidget(m_baseLabel);
    legendLayout->addStretch();
    legendLayout->addWidget(m_compLabel);
    mainLayout->addLayout(legendLayout);

    // 2. Main Image Display Area
    m_displayLabel = new QLabel();
    m_displayLabel->setAlignment(Qt::AlignCenter);
    m_displayLabel->setStyleSheet("background-color: black; border: 5px solid #555;");
    m_displayLabel->setMinimumSize(dlgW * .8, dlgH * .8);
    mainLayout->addWidget(m_displayLabel, 1);

    // 3. Controls Area
    mainLayout->addWidget(new QLabel(tr("Blend Ratio (Slide to compare difference):")));

    QSlider* slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 100);
    slider->setValue(50);
    mainLayout->addWidget(slider);

    QPushButton* saveBtn = new QPushButton(tr("Save This Comparison Image"));
    mainLayout->addWidget(saveBtn);

    // Signal/Slot Connections
    connect(slider, &QSlider::valueChanged, this, &RonchiCompareDialog::updateOverlay);
    connect(saveBtn, &QPushButton::clicked, this, &RonchiCompareDialog::onSaveClicked);

    // Perform initial render
    updateOverlay(50);

}

RonchiCompareDialog::~RonchiCompareDialog()
{
    // Destructor implementation to satisfy vtable requirements
}

cv::Mat RonchiCompareDialog::qImageToMat(const QImage& image)
{
    // Force conversion to 4-channel ARGB to ensure predictable memory layout for OpenCV
    QImage swapped = image.convertToFormat(QImage::Format_ARGB32);
    // clone() is critical here to ensure the Mat owns its data buffer
    return cv::Mat(swapped.height(), swapped.width(), CV_8UC4,
                   const_cast<uchar*>(swapped.bits()),
                   swapped.bytesPerLine()).clone();
}

void RonchiCompareDialog::updateOverlay(int val)
{
    double alpha = (100 - val) / 100.0;

    cv::Mat m1 = qImageToMat(m_q1);
    cv::Mat m2 = qImageToMat(m_q2);

    if (m1.empty() || m2.empty()) return;

    // Convert to grayscale for initial processing
    cv::cvtColor(m1, m1, cv::COLOR_BGRA2GRAY);
    cv::cvtColor(m2, m2, cv::COLOR_BGRA2GRAY);



    // SAFETY: Match sizes to prevent OpenCV arithm.cpp crashes
    if (m1.size() != m2.size()) {
        cv::resize(m2, m2, m1.size(), 0, 0, cv::INTER_LINEAR);
    }
    cv::Mat zeros = cv::Mat::zeros(m1.size(), CV_8UC1);
    // 2. Threshold the image
        // Target: pixels < 127 (approx 1/2 intensity)
        // We create a binary mask where the pixels below 128 are "active" (255)

        cv::Mat mask1, mask2;

        cv::threshold(m1, mask1, 127, 255, cv::THRESH_BINARY_INV);
        // Grayscale values below threshold are preserved as intensity in specific channels
        cv::Mat zero = cv::Mat::zeros(m1.size(), CV_8UC1);
        std::vector<cv::Mat> channels1;
        channels1 = {zero, zero, m1};
        cv::Mat result1;
        cv::merge(channels1, result1);


        cv::threshold(m2, mask2, 127, 255, cv::THRESH_BINARY_INV);
        cv::Mat result2;
        std::vector<cv::Mat> channels2;
        channels2 = {m2, m2, zero};

        cv::merge(channels2, result2);





    // Perform the additive blend (Overlay)
    cv::addWeighted(result1, 1.0 - alpha, result2, alpha, 0.0, m_currentMat);

    // Convert the result back to QImage for the Qt Label
    QImage res(m_currentMat.data, m_currentMat.cols, m_currentMat.rows,
               m_currentMat.step, QImage::Format_RGB888);

    // Display result (Swap BGR back to RGB for correct colors in Qt)
    m_displayLabel->setPixmap(QPixmap::fromImage(res.rgbSwapped())
        .scaled(m_displayLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void RonchiCompareDialog::onSaveClicked()
{
    QString path = QFileDialog::getSaveFileName(this, tr("Save Ronchi Comparison"),
                                                "", tr("PNG Images (*.png);;JPG Images (*.jpg)"));
    if (!path.isEmpty()) {
        cv::imwrite(path.toStdString(), m_currentMat);
    }
}
