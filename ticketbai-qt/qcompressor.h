#ifndef QCOMPRESSOR_H
#define QCOMPRESSOR_H

#include <QByteArray>
#include "ticketbai-qt_global.h"

class TICKETBAIQT_EXPORT QCompressor
{
public:
  static bool gzipCompress(QByteArray input, QByteArray &output, int level = -1);
  static bool gzipDecompress(QByteArray input, QByteArray &output);
};

#endif // QCOMPRESSOR_H
