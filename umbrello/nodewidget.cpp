/*
 *  copyright (C) 2003-2004
 *  Umbrello UML Modeller Authors <uml-devel@ uml.sf.net>
 */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "nodewidget.h"
#include "node.h"
#include "umlview.h"
#include <kdebug.h>
#include <qpainter.h>

NodeWidget::NodeWidget(UMLView * view, UMLNode *n )
  : StereotypedWidget(view, n) {
    init();
    setSize(100, 30);
    calculateSize();
}

void NodeWidget::init() {
    UMLWidget::setBaseType(Uml::wt_Node);
    m_pMenu = 0;

    const Settings::OptionState& ops = m_pView->getOptionState();
    m_bShowStereotype = ops.classState.showStereoType;

    calculateSize();
    update();
}

NodeWidget::~NodeWidget() {}

void NodeWidget::draw(QPainter & p, int offsetX, int offsetY) {
    UMLWidget::setPen(p);
    if ( UMLWidget::getUseFillColour() ) {
        p.setBrush( UMLWidget::getFillColour() );
    } else {
        p.setBrush( m_pView->viewport()->backgroundColor() );
    }

    int w = width();
    int h = height();
    int bodyOffsetY = offsetY + (h/3);
    int bodyWidth = w - (w/3);
    int bodyHeight = h - (h/3);
    QFont font = UMLWidget::getFont();
    font.setBold(true);
    QFontMetrics &fm = getFontMetrics(FT_BOLD);
    int fontHeight  = fm.lineSpacing();
    QString name = getName();

    QPointArray pointArray(6);
    pointArray.setPoint(0, offsetX, bodyOffsetY);
    pointArray.setPoint(1, offsetX + (w/3), offsetY);
    pointArray.setPoint(2, offsetX + w - 1, offsetY);
    pointArray.setPoint(3, offsetX + w - 1, offsetY + ((h/3)*2) );
    pointArray.setPoint(4, offsetX + bodyWidth, offsetY + h - 1);
    pointArray.setPoint(5, offsetX, offsetY + h - 1);
    p.drawPolygon(pointArray);
    p.drawRect(offsetX, bodyOffsetY, bodyWidth, bodyHeight);
    p.drawLine(offsetX + w - 1, offsetY, offsetX + bodyWidth - 2, bodyOffsetY + 1);

    p.setPen( QPen(Qt::black) );
    p.setFont(font);

    int lines = 1;
    if (m_pObject) {
        QString stereotype = m_pObject->getStereotype();
        if (!stereotype.isEmpty()) {
            p.drawText(offsetX, bodyOffsetY + (bodyHeight/2) - fontHeight,
                       bodyWidth, fontHeight, Qt::AlignCenter, m_pObject->getStereotype(true));
            lines = 2;
        }
    }

    if ( UMLWidget::getIsInstance() ) {
        font.setUnderline(true);
        p.setFont(font);
        name = UMLWidget::getInstanceName() + " : " + name;
    }

    if (lines == 1) {
        p.drawText(offsetX, bodyOffsetY + (bodyHeight/2) - (fontHeight/2),
                   bodyWidth, fontHeight, Qt::AlignCenter, name);
    } else {
        p.drawText(offsetX, bodyOffsetY + (bodyHeight/2),
                   bodyWidth, fontHeight, Qt::AlignCenter, name);
    }

    if(m_bSelected) {
        drawSelected(&p, offsetX, offsetY);
    }
}

void NodeWidget::calculateSize() {
    if (m_pObject == NULL)
        return;

    QFontMetrics &fm = getFontMetrics(FT_BOLD_ITALIC);
    int fontHeight  = fm.lineSpacing();

    QString name = m_pObject->getName();
    if ( UMLWidget::getIsInstance() ) {
        name = UMLWidget::getInstanceName() + " : " + name;
    }

    int width = fm.width(name);

    int tempWidth = 0;
    if (!m_pObject->getStereotype().isEmpty()) {
        tempWidth = fm.width(m_pObject->getStereotype(true));
    }
    width = tempWidth>width ? tempWidth : width;
    width += NODE_MARGIN * 2;

    int height = (2*fontHeight) + (NODE_MARGIN * 2);

    width = static_cast<int>(width * 1.5);
    height = static_cast<int>(height * 1.5);

    setSize(width, height);
    adjustAssocs( getX(), getY() );//adjust assoc lines
}

void NodeWidget::saveToXMI(QDomDocument& qDoc, QDomElement& qElement) {
    QDomElement conceptElement = qDoc.createElement("nodewidget");
    StereotypedWidget::saveToXMI(qDoc, conceptElement);
    qElement.appendChild(conceptElement);
}

