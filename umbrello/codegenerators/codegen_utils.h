/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   (C) 2004  Umbrello UML Modeller Authors <uml-devel@ uml.sf.net>       *
 ***************************************************************************/

#ifndef CODEGEN_UTILS_H
#define CODEGEN_UTILS_H

#include <qstringlist.h>

namespace Umbrello {

	/**
	 * Add C++ primitives as datatypes
	 */
	void createCppDatatypes();

	/**
	 * Get list of C++ reserved keywords
	 */
	const QStringList reservedCppKeywords();

}

#endif  // CODEGEN_UTILS_H

