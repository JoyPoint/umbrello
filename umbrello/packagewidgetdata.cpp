/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qdatastream.h>
#include <kdebug.h>

#include "packagewidgetdata.h"

PackageWidgetData::PackageWidgetData(SettingsDlg::OptionState optionState):UMLWidgetData(optionState) {
	m_bShowStereotype = true;
	m_Type = Uml::wt_Package;
}

PackageWidgetData::~PackageWidgetData() {}

PackageWidgetData::PackageWidgetData(PackageWidgetData& Other) : UMLWidgetData(Other) {
	*this = Other;
}

PackageWidgetData & PackageWidgetData::operator=(PackageWidgetData& Other) {
	*((UMLWidgetData*)this) = (UMLWidgetData)Other;

	m_bShowStereotype = Other.m_bShowStereotype;

	return *this;
}

bool PackageWidgetData::operator==(PackageWidgetData & Other) {
	if(!(*((UMLWidgetData*)this) == (UMLWidgetData)Other)) {
		return false;
	}

	if(m_bShowStereotype != Other.m_bShowStereotype) {
		return false;
	}

	return true;
}

long PackageWidgetData::getClipSizeOf() {
	long l_size = UMLWidgetData::getClipSizeOf();
	//Q_UINT32 tmp; //tmp is used to calculate the size of each serialized null string

	l_size += sizeof((int)m_bShowStereotype);

	return l_size;
}

/** No descriptions */
bool PackageWidgetData::serialize(QDataStream *s, bool archive, int fileversion) {
	if(!UMLWidgetData::serialize(s, archive, fileversion)) {
		return false;
	}
	if(archive) {
		*s << (int)m_bShowStereotype;
	} else {
		int showStereotype = 0;
		*s >> showStereotype;
		m_bShowStereotype = (bool)showStereotype;
	}

	return true;
}

/** Read property of bool m_bShowStereotype. */
bool PackageWidgetData::getShowStereotype() {
	return m_bShowStereotype;
}

/** Write property of bool m_bShowStereotype. */
void PackageWidgetData::setShowStereotype( bool ShowStereotype) {
	m_bShowStereotype = ShowStereotype;
}

/** No descriptions */
void PackageWidgetData::print2cerr() {
	UMLWidgetData::print2cerr();
	if(m_bShowStereotype) {
		kdDebug() << "m_bShowStereotype = true" << endl;
	} else {
		kdDebug() << "m_bShowStereotype = false" << endl;
	}
}

bool PackageWidgetData::saveToXMI(QDomDocument& qDoc, QDomElement& qElement) {
	QDomElement conceptElement = qDoc.createElement("packagewidget");
	bool status = UMLWidgetData::saveToXMI(qDoc, conceptElement);
	conceptElement.setAttribute("showstereotype", m_bShowStereotype);
	qElement.appendChild(conceptElement);
	return status;
}

bool PackageWidgetData::loadFromXMI(QDomElement& qElement) {
	if ( !UMLWidgetData::loadFromXMI(qElement) ) {
		return false;
	}
	QString showstereo = qElement.attribute("showstereotype", "0");
	m_bShowStereotype = (bool)showstereo.toInt();
	return true;
}




