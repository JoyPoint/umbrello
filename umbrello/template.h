/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TEMPLATE_H
#define TEMPLATE_H

#include "classifierlistitem.h"

/**
 *	This class holds information used by template classes, called
 *	paramaterised class in UML and a generic in Java.  It has a
 *	type (usually just "class") and name.
 *
 *	@short Sets up template information.
 *	@author Jonathan Riddell
 *	@see	UMLObject
 * Bugs and comments to uml-devel@lists.sf.net or http://bugs.kde.org
 */

class UMLTemplate : public UMLClassifierListItem {
public:
	/**
	 *	Sets up an template.
	 *
	 *	@param	parent		The parent of this UMLTemplate (i.e. it's concept).
	 *	@param	name		The name of this UMLTemplate.
	 *	@param	id		The unique id given to this UMLTemplate.
	 *	@param	type		The type of this UMLTemplate.
	 */
	UMLTemplate(QObject* parent, QString name, int id, QString type = "class");

	/**
	 *	Sets up a template.
	 *
	 *	@param	parent		The parent of this UMLTemplate (i.e. it's concept).
	 */
	UMLTemplate(QObject* parent);

	/**
	 * 	Overloaded '==' operator
	 */
	bool operator==(UMLTemplate &rhs);

	/**
	 *	Standard deconstructor.
	 */
	~UMLTemplate();

	/**
	 *	Returns the type of the UMLTemplate.
	 *
	 *	@return The type of the UMLTemplate.
	 */
	QString getTypeName();

	/**
	 *	Sets the type of the UMLTemplate.
	 *
	 *	@param	type	The type of the UMLTemplate.
	 */
	void setTypeName(QString type);

	/**
	 *	Returns a string representation of the UMLTemplate.
	 *
	 *	@return	Returns a string representation of the UMLTemplate.
	 */
	QString toString(Signature_Type sig = st_NoSig);

	/**
	 * Display the properties configuration dialogue for the template.
	 */
	bool showPropertiesDialogue(QWidget* parent);
	
	/**
	 * Write the <template> element.
	 */
	bool saveToXMI(QDomDocument & qDoc, QDomElement & qElement);

	/** 
	 * Loads the <template> element.
	 */
	bool loadFromXMI(QDomElement & element);
private:
	/**
	 * The type of the template, usually just "class"
	 */
	QString m_TypeName;
};

#endif
