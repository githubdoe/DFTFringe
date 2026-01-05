#ifndef PROFILECURVE_H
#define PROFILECURVE_H

#include <qwt_plot_curve.h>
#include <QPen>
#include <cmath>

/**
 * @brief The ProfileCurve class
 * A custom QwtPlotCurve that automatically highlights segments where the
 * vertical delta between adjacent points exceeds a specified threshold.
 */
class ProfileCurve : public QwtPlotCurve
{
public:
    explicit ProfileCurve(const QString &title = QString());

    /**
     * @brief Configure the slope highlighting behavior
     * @param show - Toggle the orange highlight on/off
     * @param limit - The vertical delta threshold (hDelLimit)
     * @param highlightWidth - Width of the orange pen
     */
    void setSlopeSettings(bool show, double limit, int highlightWidth = 2);

protected:
    /**
     * @brief Overrides the standard line drawing to add the highlight pass
     */
    virtual void drawLines(QPainter *painter,
                           const QwtScaleMap &xMap,
                           const QwtScaleMap &yMap,
                           const QRectF &canvasRect,
                           int from, int to) const override;

private:
    bool m_showSlopeError;
    double m_slopeLimit;
    int m_highlightWidth;
};

#endif // PROFILECURVE_H
