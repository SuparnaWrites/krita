/*
 *  Copyright (c) 2008 Lukáš Tvrdý <lukast.dev@gmail.com>
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

#include "kis_chalk_paintop_settings.h"

#include <kis_chalkop_option.h>

#include <kis_paint_action_type_option.h>
#include <kis_airbrush_option.h>

KisChalkPaintOpSettings::KisChalkPaintOpSettings()
{
}

bool KisChalkPaintOpSettings::paintIncremental()
{
    return (enumPaintActionType)getInt("PaintOpAction", WASH) == BUILDUP;
}

bool KisChalkPaintOpSettings::isAirbrushing() const
{
    return getBool(AIRBRUSH_ENABLED);
}

int KisChalkPaintOpSettings::rate() const
{
    return getInt(AIRBRUSH_RATE);
}


void KisChalkPaintOpSettings::paintOutline(const QPointF& pos, KisImageWSP image, QPainter& painter, KisPaintOpSettings::OutlineMode _mode) const
{
    if (_mode != CursorIsOutline) return;
    qreal size = getInt(CHALK_RADIUS) * 2 + 1;
    painter.setPen(Qt::black);
    painter.drawEllipse(image->pixelToDocument(QRectF(0, 0, size, size).translated(- QPoint(size * 0.5, size * 0.5))).translated(pos));
}


QRectF KisChalkPaintOpSettings::paintOutlineRect(const QPointF& pos, KisImageWSP image, KisPaintOpSettings::OutlineMode _mode) const
{
    if (_mode != CursorIsOutline) return QRectF();
    qreal size = getInt(CHALK_RADIUS) * 2 + 1;
    size += 10;
    return image->pixelToDocument(QRectF(0, 0, size, size).translated(- QPoint(size * 0.5, size * 0.5))).translated(pos);
}

QPainterPath KisChalkPaintOpSettings::brushOutline(const QPointF& pos,OutlineMode mode) const
{
    Q_UNUSED(pos);
    QPainterPath path;
    if (mode == CursorIsOutline){
        qreal size = getInt(CHALK_RADIUS) * 2 + 1;
        QRectF rc(0, 0, size, size);
        rc.translate(-rc.center());
        path.addEllipse(rc);
    }
    return path;
}


#if defined(HAVE_OPENGL)
QString KisChalkPaintOpSettings::modelName() const
{
    return "3d-pencil";
}
#endif

