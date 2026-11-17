#include <QtTest>
#include "circlefit.h"

#include <algorithm>
#include <array>
#include <cmath>

class CircleFitTest : public QObject
{
    Q_OBJECT

private slots:
    void exactCircle_data();
    void exactCircle();
};

void CircleFitTest::exactCircle_data()
{
    QTest::addColumn<double>("centerX");
    QTest::addColumn<double>("centerY");
    QTest::addColumn<double>("radius");
    QTest::addColumn<double>("startAngle");
    QTest::addColumn<double>("sweep");

    QTest::newRow("unit") << 0.0 << 0.0 << 1.0 << 0.0 << 6.0;
    QTest::newRow("translated") << 12.0 << -7.0 << 4.0 << 0.3 << 6.0;
    QTest::newRow("small-radius") << -0.5 << 0.25 << 0.01 << 0.7 << 6.0;
    QTest::newRow("partial-arc") << -3.0 << 5.0 << 2.0 << 0.4 << 1.5;
}

void CircleFitTest::exactCircle()
{
    QFETCH(double, centerX);
    QFETCH(double, centerY);
    QFETCH(double, radius);
    QFETCH(double, startAngle);
    QFETCH(double, sweep);

    constexpr int count = 24;
    std::array<double, count> x;
    std::array<double, count> y;
    for (int i = 0; i < count; ++i) {
        const double angle = startAngle + sweep * i / (count - 1);
        x[i] = centerX + radius * std::cos(angle);
        y[i] = centerY + radius * std::sin(angle);
    }

    CircleData data(count, x.data(), y.data());
    const Circle fitted = CircleFitByHyper(data);
    const double tolerance = 1e-9 * std::max(1.0, radius);

    QVERIFY(std::isfinite(fitted.a));
    QVERIFY(std::isfinite(fitted.b));
    QVERIFY(std::isfinite(fitted.r));
    QVERIFY(std::isfinite(fitted.s));
    QVERIFY(std::abs(fitted.a - centerX) <= tolerance);
    QVERIFY(std::abs(fitted.b - centerY) <= tolerance);
    QVERIFY(std::abs(fitted.r - radius) <= tolerance);
    QVERIFY(fitted.s >= 0.0);
    QVERIFY(fitted.s <= tolerance);
    QCOMPARE(data.n, count);
    for (int i = 0; i < count; ++i) {
        QVERIFY(data.X[i] == x[i]);
        QVERIFY(data.Y[i] == y[i]);
    }
}

QTEST_APPLESS_MAIN(CircleFitTest)
#include "tst_circlefit.moc"
