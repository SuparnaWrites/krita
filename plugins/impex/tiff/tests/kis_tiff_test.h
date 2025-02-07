/*
 * SPDX-FileCopyrightText: 2007 Cyrille Berger <cberger@cberger.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef _KIS_TIFF_TEST_H_
#define _KIS_TIFF_TEST_H_

#include <simpletest.h>

class KisTiffTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testFiles();
    void testRoundTripRGBF16();

    void testSaveTiffColorSpace(QString colorModel, QString colorDepth, QString colorProfile);
    void testSaveTiffRgbaColorSpace();
    void testSaveTiffGreyAColorSpace();
    void testSaveTiffCmykColorSpace();
    void testSaveTiffLabColorSpace();
    void testSaveTiffYCbCrAColorSpace();

    void testImportFromWriteonly();
    void testExportToReadonly();
    void testImportIncorrectFormat();
};

#endif
