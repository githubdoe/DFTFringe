/******************************************************************************
**
**  RAW camera file support for DFTFringe.  See rawimage.h.
**
**  This file is part of DFTFringe and is distributed under the same GNU GPLv3
**  license as the rest of the project.
**
****************************************************************************/
#include "rawimage.h"

#include <QFileInfo>
#include <libraw/libraw.h>

namespace RawImage {

QStringList extensions()
{
    // Common camera RAW extensions handled by LibRaw.
    static const QStringList exts = {
        "cr2", "cr3", "crw",          // Canon
        "nef", "nrw",                 // Nikon
        "arw", "srf", "sr2",          // Sony
        "dng",                        // Adobe / generic
        "raf",                        // Fujifilm
        "rw2",                        // Panasonic
        "orf",                        // Olympus
        "pef", "ptx",                 // Pentax
        "raw", "rwl",                 // Leica
        "3fr", "fff",                 // Hasselblad
        "mef",                        // Mamiya
        "mos", "iiq",                 // Leaf / Phase One
        "dcr", "kdc",                 // Kodak
        "srw",                        // Samsung
        "erf",                        // Epson
        "x3f"                         // Sigma
    };
    return exts;
}

QString nameFilter()
{
    QStringList parts;
    for (const QString &e : extensions()) {
        parts << ("*." + e.toLower());
        parts << ("*." + e.toUpper());
    }
    return parts.join(' ');
}

bool isRawFile(const QString &fileName)
{
    const QString suffix = QFileInfo(fileName).suffix().toLower();
    return extensions().contains(suffix);
}

QImage load(const QString &fileName, QString *errorMsg)
{
    auto fail = [&](const QString &msg) -> QImage {
        if (errorMsg)
            *errorMsg = msg;
        return QImage();
    };

    LibRaw raw;

    // Processing parameters chosen to preserve fringe contrast:
    //  - 8 bit output so it maps straight onto the existing 8 bit pipeline
    //  - sRGB output colour space
    //  - camera white balance for a neutral looking image
    //  - no automatic brightness stretch, which could clip bright fringes
    raw.imgdata.params.output_bps   = 8;
    raw.imgdata.params.output_color = 1;   // sRGB
    raw.imgdata.params.use_camera_wb = 1;
    raw.imgdata.params.no_auto_bright = 1;

    int ret = raw.open_file(fileName.toLocal8Bit().constData());
    if (ret != LIBRAW_SUCCESS)
        return fail(QStringLiteral("LibRaw open_file failed: ") + libraw_strerror(ret));

    ret = raw.unpack();
    if (ret != LIBRAW_SUCCESS) {
        raw.recycle();
        return fail(QStringLiteral("LibRaw unpack failed: ") + libraw_strerror(ret));
    }

    ret = raw.dcraw_process();
    if (ret != LIBRAW_SUCCESS) {
        raw.recycle();
        return fail(QStringLiteral("LibRaw dcraw_process failed: ") + libraw_strerror(ret));
    }

    int errc = 0;
    libraw_processed_image_t *proc = raw.dcraw_make_mem_image(&errc);
    if (proc == nullptr || errc != LIBRAW_SUCCESS) {
        if (proc)
            LibRaw::dcraw_clear_mem(proc);
        raw.recycle();
        return fail(QStringLiteral("LibRaw dcraw_make_mem_image failed: ")
                    + libraw_strerror(errc));
    }

    QImage result;

    if (proc->type == LIBRAW_IMAGE_BITMAP && proc->colors == 3 && proc->bits == 8) {
        // Interleaved RGB, 8 bits per channel.  QImage does not own the data,
        // so copy() before proc is freed below.
        QImage tmp(proc->data, proc->width, proc->height,
                   proc->width * 3, QImage::Format_RGB888);
        result = tmp.copy();
    }
    else if (proc->type == LIBRAW_IMAGE_JPEG) {
        // Some cameras (e.g. Sigma X3F) hand back an embedded JPEG.
        result = QImage::fromData(proc->data, proc->data_size, "JPEG");
    }
    else {
        LibRaw::dcraw_clear_mem(proc);
        raw.recycle();
        return fail(QStringLiteral("Unsupported RAW image layout: colors=%1 bits=%2")
                    .arg(proc->colors).arg(proc->bits));
    }

    LibRaw::dcraw_clear_mem(proc);
    raw.recycle();

    if (result.isNull())
        return fail(QStringLiteral("Decoded RAW image was empty."));

    return result;
}

} // namespace RawImage
