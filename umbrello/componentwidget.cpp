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

#include "componentwidget.h"
#include "component.h"
#include "umlview.h"
#include <kdebug.h>
#include <qpainter.h>

ComponentWidget::ComponentWidget(UMLView * view, UMLComponent *c)
  : StereotypedWidget(view, c) {
    init();
    setSize(100, 30);
    calculateSize();
    UMLWidget::setBaseType(Uml::wt_Component);
}

void ComponentWidget::init() {
    m_pMenu = 0;
    //set defaults from m_pView
    if (m_pView) {
        //check to see if correct
        const Settings::OptionState& ops = m_pView->getOptionState();
        m_bShowStereotype = ops.classState.showStereoType;
    }
    //maybe loading and this may not be set.
    if (m_pObject) {
        calculateSize();
        update();
    }
}

ComponentWidget::~ComponentWidget() {}

void ComponentWidget::draw(QPainter & p, int offsetX, int offsetY) {
    UMLComponent *umlcomp = static_cast<UMLComponent*>(m_pObject);
    if (umlcomp == NULL)
        return;
    UMLWidget::setPen(p);
    if ( umlcomp->getExecutable() ) {
        QPen thickerPen = p.pen();
        thickerPen.setWidth(2);
        p.setPen(thickerPen);
    }
    if ( UMLWidget::getUseFillColour() ) {
        p.setBrush( UMLWidget::getFillColour() );
    } else {
        p.setBrush( m_pView->viewport()->backgroundColor() );
    }

    int w = width();
    int h = height();
    QFont font = UMLWidget::getFont();
    font.setBold(true);
    QFontMetrics &fm = getFontMetrics(FT_BOLD);
    int fontHeight  = fm.lineSpacing();
    QString name = getName();
    QString stereotype = m_pObject->getStereotype();

    p.drawRect(offsetX + 2*COMPONENT_MARGIN, offsetY,
               w - 2*COMPONENT_MARGIN, 3*COMPONENT_MARGIN + 2*fontHeight);
    p.drawRect(offsetX, offsetY + COMPONENT_MARGIN, COMPONENT_MARGIN*4, fontHeight);
    p.drawRect(offsetX, offsetY + COMPONENT_MARGIN*2 + fontHeight, COMPONENT_MARGIN*4, fontHeight);

    p.setPen( QPen(Qt::black) );
    p.setFont(font);

    int lines = 1;

    if (!stereotype.isEmpty()) {
        p.drawText(offsetX + (COMPONENT_MARGIN*4), offsetY + (h/2) - fontHeight,
                   w - (COMPONENT_MARGIN*4), fontHeight, Qt::AlignCenter,
                   m_pObject->getStereotype(true));
        lines = 2;
    }

    if ( UMLWidget::getIsInstance() ) {
        font.setUnderline(true);
        p.setFont(font);
        name = UMLWidget::getInstanceName() + " : " + name;
    }

    if (lines == 1) {
        p.drawText(offsetX + (COMPONENT_MARGIN*4), offsetY + (h/2) - (fontHeight/2),
                   w - (COMPONENT_MARGIN*4), fontHeight, Qt::AlignCenter, name );
    } else {
        p.drawText(offsetX + (COMPONENT_MARGIN*4), offsetY + (h/2),
                   w - (COMPONENT_MARGIN*4), fontHeight, Qt::AlignCenter, name );
    }

    if(m_bSelected) {
        drawSelected(&p, offsetX, offsetY);
    }
}

void ComponentWidget::calculateSize() {
    if ( !m_pObject) {
        return;
    }
    int width, height;

    QFontMetrics &fm = getFontMetrics(FT_BOLD_ITALIC);
    int fontHeight  = fm.lineSpacing();

    QString name = m_pObject->getName();
    if ( UMLWidget::getIsInstance() ) {
        name = UMLWidget::getInstanceName() + " : " + name;
    }

    width = fm.width(name);

    int tempWidth = 0;
    if(!m_pObject->getStereotype().isEmpty()) {
        tempWidth = fm.width(m_pObject->getStereotype(true));
    }
    width = tempWidth>width ? tempWidth : width;
    width += COMPONENT_MARGIN * 6;
    width = 70>width ? 70 : width; //minumin width of 70

    height = (2*fontHeight) + (COMPONENT_MARGIN * 3);

    setSize(width, height);
    adjustAssocs( getX(), getY() );//adjust assoc lines
}

void ComponentWidget::saveToXMI(QDomDocument& qDoc, QDomElement& qElement) {
    QDomElement conceptElement = qDoc.createElement("componentwidget");
    StereotypedWidget::saveToXMI(qDoc, conceptElement);
    qElement.appendChild(conceptElement);
}

