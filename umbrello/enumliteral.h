/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ENUMLITERAL_H
#define ENUMLITERAL_H

#include "classifierlistitem.h"

/**
 * This class is used to set up information for an enum literal.  Enum
 * literals are the values that enums can be set to.
 *
 * @short Sets up attribute information.
 * @author Paul Hensgen <phensgen@techie.com>
 * @see UMLObject
 * Bugs and comments to uml-devel@lists.sf.net or http://bugs.kde.org
 */

class UMLEnumLiteral : public UMLClassifierListItem {
public:
	/**
	 * Sets up an enum literal.
	 *
	 * @param parent	The parent of this UMLEnumLiteral.
	 * @param name		The name of this UMLEnumLiteral.
	 * @param id		The unique id given to this UMLEnumLiteral.
	 */
	UMLEnumLiteral(const UMLObject* parent, QString name, int id);

	/**
	 * Sets up an enum literal.
	 *
	 * @param parent	The parent of this UMLEnumLiteral.
	 */
	UMLEnumLiteral(const UMLObject* parent);

	/**
	 * Overloaded '==' operator
	 */
	bool operator==(UMLEnumLiteral &rhs);

	/**
	 * Copy the internal presentation of this object into the new
	 * object.
	 */
	virtual void copyInto(UMLEnumLiteral *rhs) const;

	/**
	 * Make a clone of this object.
	 */
	virtual UMLObject* clone() const;

	/**
	 * destructor
	 */
	virtual ~UMLEnumLiteral();

	/**
	 * Returns the type of the UMLEnumLiteral.
	 *
	 * @return	The type of the UMLEnumLiteral.
	 */
	QString getTypeName();

	/**
	 * Creates the <UML:EnumLiteral> XMI element.
	 */
	void saveToXMI(QDomDocument& qDoc, QDomElement& qElement);

	/**
	 * Display the properties configuration dialogue for the enum literal.
	 */
	bool showPropertiesDialogue(QWidget* parent);

protected:
	/**
	 * Loads the <UML:EnumLiteral> XMI element (empty.)
	 */
	bool load(QDomElement& element);

};

#endif
