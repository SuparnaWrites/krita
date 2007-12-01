/*
 *  kis_controlframe.h - part of Krita
 *
 *  Copyright (c) 1999 Matthias Elter  <elter@kde.org>
 *  Copyright (c) 2003 Patrick Julien  <freak@codepimps.org>
 *  Copyright (c) 2004 Sven Langkamp  <sven.langkamp@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef __kis_controlframe_h__
#define __kis_controlframe_h__

#include <QMenu>
#include <QKeyEvent>

#include <ktoolbar.h>

class QWidget;
class QTabWidget;

class KToolBar;

class QTableWidgetItem;
class KoResourceItem;

class KisAutobrush;
class KisBrush;
class KisBrushChooser;
class KoSegmentGradient;
class KisGradientChooser;
class KoResourceItemChooser;
class KisResourceMediator;
class KisPaintopBox;
class KisView2;
class KisIconWidget;


/**
 *   Control Frame - status display with access to
 *   color selector, brushes, patterns, and preview
 */
class KisControlFrame : public QObject  //: public KToolBar
{
    Q_OBJECT

public:
    KisControlFrame(KXmlGuiWindow * window, KisView2 * view, const char *name = 0 );
    virtual ~KisControlFrame() {}

public slots:

    void slotSetBrush(QTableWidgetItem *item);
    void slotSetPattern(QTableWidgetItem *item);
    void slotSetGradient(QTableWidgetItem *item);

    void slotBrushChanged(KisBrush * brush);
    void slotPatternChanged(KisPattern * pattern);
    void slotGradientChanged(KoSegmentGradient * gradient);

private:

    void createBrushesChooser(KisView2 * view);
    void createPatternsChooser(KisView2 * view);
    void createGradientsChooser(KisView2 * view);


private:
    QFont m_font;
    KisView2 * m_view;

    QTabWidget * m_brushesTab;
    QTabWidget * m_gradientTab;
    QTabWidget * m_patternsTab;

    KisIconWidget *m_brushWidget;
    KisIconWidget *m_patternWidget;
    KisIconWidget *m_gradientWidget;

    QWidget * m_brushChooserPopup;
    QWidget * m_patternChooserPopup;
    QWidget * m_gradientChooserPopup;

    KisResourceMediator *m_brushMediator;
    KisResourceMediator *m_patternMediator;
    KisResourceMediator *m_gradientMediator;


    KisAutobrush * m_autobrush;
    KisBrushChooser * m_brushChooser;
    KoResourceItemChooser * m_gradientChooser;

    KisPaintopBox * m_paintopBox;
};

#endif

