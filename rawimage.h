/******************************************************************************
**
**  RAW camera file support for DFTFringe.
**
**  Reads camera RAW files (Canon CR2/CR3, Nikon NEF, Sony ARW, Adobe DNG,
**  Panasonic RW2, etc.) using LibRaw and returns a demosaiced QImage that the
**  rest of DFTFringe can treat exactly like a jpg/png/bmp interferogram.
**
**  This file is part of DFTFringe and is distributed under the same GNU GPLv3
**  license as the rest of the project.
**
****************************************************************************/
#ifndef RAWIMAGE_H
#define RAWIMAGE_H

#include <QImage>
#include <QString>
#include <QStringList>

namespace RawImage {

// Returns the list of supported RAW extensions, lower case, without the
// leading dot, e.g. {"cr2", "nef", "arw", "dng", ...}.
QStringList extensions();

// Returns a QFileDialog name-filter fragment containing every supported RAW
// extension in both lower and upper case, e.g.
//   "*.cr2 *.CR2 *.nef *.NEF ..."
QString nameFilter();

// True if fileName has one of the RAW extensions above (case insensitive).
bool isRawFile(const QString &fileName);

// Decodes a camera RAW file into an 8 bit RGB QImage (Format_RGB888).
// Returns a null QImage on failure; if errorMsg is non-null it receives a
// human readable description of the failure.
QImage load(const QString &fileName, QString *errorMsg = nullptr);

} // namespace RawImage

#endif // RAWIMAGE_H
