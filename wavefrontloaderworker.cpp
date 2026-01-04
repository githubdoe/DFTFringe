#include "wavefrontloaderworker.h"
#include <QString>
#include "surfacemanager.h"
// Ensure your SurfaceManager header is included here
// #include "surfacemanager.h"

WavefrontLoaderWorker::WavefrontLoaderWorker(QObject *parent)
    : QObject(parent), m_isCancelled(false)
{
}

void WavefrontLoaderWorker::process(const QStringList& args, SurfaceManager* manager) {
    m_isCancelled = false; // Reset the flag for this run
    int cnt = 0;

    for (const QString &arg : args) {
        // Safe check for cross-thread cancellation
        if (m_isCancelled.load()) {
            break;
        }

        if (arg.endsWith(".wft", Qt::CaseInsensitive)) {
            // Signal the UI thread to update the label
            emit progressLabelChanged(arg);

            try {
                // The actual heavy task
                manager->loadWavefront(arg);
            } catch (...) {
                // Handle potential exceptions during load
                break;
            }

            // Signal the UI thread to update the progress bar
            emit progressValueChanged(++cnt);
        }
    }

    emit finished();
}

void WavefrontLoaderWorker::cancel() {
    m_isCancelled.store(true);
}
