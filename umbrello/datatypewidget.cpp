/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   copyright (C) 2003-2008                                               *
 *   Umbrello UML Modeller Authors <uml-devel@uml.sf.net>                  *
 ***************************************************************************/

// own header
#include "datatypewidget.h"

// qt/kde includes
#include <qpainter.h>
#include <kdebug.h>

// app includes
#include "classifier.h"
#include "operation.h"
#include "classifierlistitem.h"
#include "umlview.h"
#include "umldoc.h"
#include "umlscene.h"


#define CIRCLE_SIZE 30

DatatypeWidget::DatatypeWidget(UMLScene* scene, UMLClassifier *d) : NewUMLRectWidget(scene, d) {
    init();
}

DatatypeWidget::~DatatypeWidget() {}

void DatatypeWidget::init() {
    NewUMLRectWidget::setBaseType(Uml::wt_Datatype);
    setSize(100, 30);
    m_pMenu = 0;
}

void DatatypeWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *o, QWidget *)
{
	QPainter &p = *painter;
	qreal offsetX = 0, offsetY = 0;

    setPenFromSettings(p);
    if (NewUMLRectWidget::getUseFillColour())  {
        p.setBrush(NewUMLRectWidget::getFillColour());
    } else {
        // [PORT] Replace with styleOption based code
        //p.setBrush( m_pView->viewport()->palette().color(QPalette::Background) );
    }

    qreal w = getWidth();
    qreal h = getHeight();

    QFontMetrics &fm = getFontMetrics(FT_NORMAL);
    qreal fontHeight  = fm.lineSpacing();
    QString name = getName();

    p.drawRect(offsetX, offsetY, w, h);
    p.setPen(QPen(Qt::black));

    QFont font = NewUMLRectWidget::getFont();
    font.setBold(true);
    p.setFont(font);
    p.drawText(offsetX + DATATYPE_MARGIN, offsetY,
               w - DATATYPE_MARGIN* 2,fontHeight,
               Qt::AlignCenter, umlObject()->getStereotype(true));

    font.setItalic( umlObject()->getAbstract() );
    p.setFont(font);
    p.drawText(offsetX + DATATYPE_MARGIN, offsetY + fontHeight,
               w - DATATYPE_MARGIN * 2, fontHeight, Qt::AlignCenter, name);

    if (isSelected()) {
        drawSelected(&p, offsetX, offsetY);
    }
}

QSizeF DatatypeWidget::calculateSize() {
    if (!umlObject())  {
        return NewUMLRectWidget::calculateSize();
    }
    qreal width, height;
    const QFontMetrics &fm = getFontMetrics(FT_NORMAL);
    const qreal fontHeight = fm.lineSpacing();

    int lines = 1;//always have one line - for name
    lines++; //for the stereotype

    height = width = 0;
    height += lines * fontHeight;

    //now set the width of the concept
    //set width to name to start with
    //set width to name to start with
    width = getFontMetrics(FT_BOLD_ITALIC).boundingRect(umlObject()->getFullyQualifiedName()).width();
    qreal w = getFontMetrics(FT_BOLD).boundingRect(umlObject()->getStereotype(true)).width();

    width = w > width?w:width;

    //allow for width margin
    width += DATATYPE_MARGIN * 2;

    return QSize(width, height);
}

void DatatypeWidget::saveToXMI( QDomDocument & qDoc, QDomElement & qElement ) {
    QDomElement conceptElement = qDoc.createElement("datatypewidget");
    NewUMLRectWidget::saveToXMI(qDoc, conceptElement);
    qElement.appendChild(conceptElement);
}

bool DatatypeWidget::loadFromXMI( QDomElement & qElement ) {
    return NewUMLRectWidget::loadFromXMI(qElement);
}

