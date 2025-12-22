#ifndef WAVEFRONTLOADERWORKER_H
#define WAVEFRONTLOADERWORKER_H

#include <QObject>
#include <QStringList>
#include <atomic>

// Forward declaration of SurfaceManager to keep header clean
class SurfaceManager;

class WavefrontLoaderWorker : public QObject {
    Q_OBJECT
public:
    explicit WavefrontLoaderWorker(QObject *parent = nullptr);

public slots:
    /**
     * @brief The main loop that processes the wavefront files.
     */
    void process(QStringList args, SurfaceManager* manager);

    /**
     * @brief Slot to trigger a safe exit of the loop.
     */
    void cancel();

signals:
    void progressLabelChanged(const QString &text);
    void progressValueChanged(int value);
    void finished();

private:
    std::atomic<bool> m_isCancelled;
};

#endif // WAVEFRONTLOADERWORKER_H
