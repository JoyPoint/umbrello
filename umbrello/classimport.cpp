/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "classimport.h"
#include "uml.h"
#include "umldoc.h"
#include "umlobject.h"
#include "docwindow.h"
#include "package.h"
#include "enum.h"
#include "datatype.h"
#include "class.h"
#include "operation.h"
#include "attribute.h"
#include "association.h"
#include "classparser/lexer.h"
#include "classparser/driver.h"
#include "classparser/cpptree2uml.h"
#include <kdebug.h>
#include <klocale.h>
#include <qregexp.h>

class CppDriver : public Driver {
public:
	void setupLexer(Lexer* lexer) {
		Driver::setupLexer(lexer);
		lexer->setRecordComments(true);
	}
};

ClassImport::ClassImport(UMLDoc * parentDoc) {
	m_umldoc = parentDoc;
	m_driver = new CppDriver();
}

ClassImport::~ClassImport() {}

QString ClassImport::doxyComment(QString comment) {
	QStringList lines = QStringList::split("\n", comment);
	if (lines.first() != "/**")
		return "";
	lines.pop_front();  // remove comment start
	lines.pop_back();   // remove comment end
	if (! lines.count())
		return "";
	for (QStringList::Iterator lit = lines.begin(); lit != lines.end(); ++lit) {
		(*lit).remove(QRegExp("^\\s+"));
		(*lit).remove(QRegExp("^\\*+\\s?"));
	}
	return lines.join("\n");
}

/*
UMLObject* ClassImport::findUMLObject(QString name,
				      Uml::Object_Type type) {
	// Why an extra wrapper? See comment at addMethodParameter()
	UMLObject * o = m_umldoc->findUMLObject(name, type);
	return o;
}
 */

UMLObject *ClassImport::createUMLObject(Uml::Object_Type type,
					QString name,
					QString stereotype,
					QString comment,
					UMLPackage *parentPkg) {
	UMLObject * o = m_umldoc->findUMLObject(name);
	if (o == NULL) {
		QString typeName(name);
		int isPointer = typeName.contains('*');
		typeName.replace(QRegExp("^const\\s+"), "");
		typeName.replace(QRegExp("[^:\\w].*$"), "");
		if (isPointer) {
			UMLObject *origType = m_umldoc->findUMLObject(typeName);
			if (origType == NULL)
				origType = m_umldoc->createUMLObject(Uml::ot_Class, typeName, parentPkg);
			o = m_umldoc->createUMLObject(Uml::ot_Datatype, name, parentPkg);
			UMLDatatype *dt = static_cast<UMLDatatype*>(o);
			UMLClassifier *c = dynamic_cast<UMLClassifier*>(origType);
			if (c)
				dt->setOriginType(c);
			else
				kdError() << "ClassImport::createUMLObject(" << name << "): "
					  << "origType " << typeName << " is not a UMLClassifier"
					  << endl;
			dt->setIsReference();
		} else
			o = m_umldoc->createUMLObject(type, name, parentPkg);
	} else
		o->setUMLPackage(parentPkg);
	QString strippedComment = doxyComment(comment);
	if (! strippedComment.isEmpty()) {
		o->setDoc(strippedComment);
		UMLApp::app()->getDocWindow()->showDocumentation(o, true);
	}
	if (!stereotype.isEmpty()) {
		o->setStereotype(stereotype);
	}
	return o;
}

UMLOperation* ClassImport::makeOperation(UMLClass *parent, QString name) {
	UMLOperation *op = new UMLOperation(parent, name, m_umldoc->getUniqueID());
	return op;
}

UMLObject* ClassImport::insertAttribute(UMLClass *o, Uml::Scope scope, QString name,
					QString type, QString comment /* ="" */,
					bool isStatic /* =false */) {
	Uml::Object_Type ot = o->getBaseType();
	if (ot != Uml::ot_Class) {
		kdDebug() << "ClassImport::insertAttribute: Don't know what to do with "
			  << o->getName() << " (object type " << ot << ")" << endl;
		return NULL;
	}
	QString strippedComment = doxyComment(comment);
	UMLAttribute *attr = ((UMLClass*)o)->addAttribute(name);
	attr->setScope(scope);
	attr->setStatic(isStatic);
	UMLObject *obj = m_umldoc->findUMLObject(type);
	UMLClassifier *classifier = dynamic_cast<UMLClassifier*>(obj);
	if (classifier == NULL) {
		kdDebug() << "ClassImport::insertAttribute(" << name
			  << "): Attribute type " << type
			  << " has no model representation" << endl;
		QString typeName(type);
		int isPointer = typeName.contains('*');
		typeName.replace(QRegExp("^const\\s+"), "");
		typeName.replace(QRegExp("[^:\\w].*$"), "");
		if (type == typeName) {
			obj = m_umldoc->createUMLObject(Uml::ot_Class, type);
			classifier = static_cast<UMLClassifier*>(obj);
		} else {
			obj = m_umldoc->findUMLObject(typeName);
			classifier = dynamic_cast<UMLClassifier*>(obj);
			if (classifier == NULL) {
				obj = m_umldoc->createUMLObject(Uml::ot_Class, typeName);
				classifier = static_cast<UMLClassifier*>(obj);
			}
			if (isPointer) {
				UMLObject *pointerDecl = m_umldoc->createUMLObject(Uml::ot_Datatype, type);
				UMLDatatype *dt = static_cast<UMLDatatype*>(pointerDecl);
				dt->setOriginType(classifier);
				dt->setIsReference();
				classifier = dt;
			}
		}
	}
	attr->setType(classifier);
	if (! strippedComment.isEmpty()) {
		attr->setDoc(strippedComment);
		UMLApp::app()->getDocWindow()->showDocumentation(attr, true);
	}

	m_umldoc->setModified(true);
	return attr;
}

void ClassImport::insertMethod(UMLClass *klass, UMLOperation *op,
					 Uml::Scope scope, QString type,
					 bool isStatic, bool isAbstract,
					 QString comment /* = "" */) {
	op->setScope(scope);
	op->setReturnType(type);
	op->setStatic(isStatic);
	op->setAbstract(isAbstract);
	klass->addOperation(op);
	//m_umldoc->signalUMLObjectCreated(op);
	QString strippedComment = doxyComment(comment);
	if (! strippedComment.isEmpty()) {
		op->setDoc(strippedComment);
		UMLApp::app()->getDocWindow()->showDocumentation(op, true);
	}
	//setModified(true);
}

UMLAttribute* ClassImport::addMethodParameter(UMLOperation *method,
					      QString type, QString name,
					      QString initialValue, QString doc,
					      Uml::Parameter_Direction kind) {
	// We don't necessarily expose the full declaration of UMLOperation
	// to clients of ClassImport.  I.e. if clients only see a pointer
	// to the forward declaration of UMLOperation, they can't call any
	// methods on that pointer.
	// That's the raison d'etre for these thin wrappers.
	return method->addParm(type, name, initialValue, doc, kind);
}

void ClassImport::addEnumLiteral(UMLEnum *enumType, QString literal) {
	// Why an extra wrapper? See comment at addMethodParameter()
	enumType->addEnumLiteral( literal, m_umldoc->getUniqueID() );
}

void ClassImport::createGeneralization(UMLClass *child, QString parentName) {
	UMLObject *parent = m_umldoc->findUMLObject( parentName, Uml::ot_Class );
	if (parent == NULL) {
	    kdDebug() << "ClassImport::createGeneralization: Could not find UML object for "
		      << parentName << endl;
	    parent = m_umldoc->createUMLObject(Uml::ot_Class, parentName);
	}
	UMLAssociation *assoc = new UMLAssociation( Uml::at_Generalization,
						    child, parent );
	m_umldoc->addAssociation(assoc);
}

void ClassImport::importCPP(QStringList headerFileList) {
	// Reset the driver
	m_driver->reset();
	// Add some standard include paths
	m_driver->addIncludePath( "/usr/include" );
	m_driver->addIncludePath( "/usr/include/c++" );
	m_driver->addIncludePath( "/usr/include/g++" );
	m_driver->addIncludePath( "/usr/local/include" );
	// FIXME: The following hack is to be replaced by a config menu in umbrello
	char *umbrello_incpath = getenv( "UMBRELLO_INCPATH" );
	if (umbrello_incpath) {
		QStringList includes = QStringList::split( ':', umbrello_incpath );
		for (QStringList::Iterator i = includes.begin();
					   i != includes.end(); i++)
			m_driver->addIncludePath( *i );
	}
	for (QStringList::Iterator fileIT = headerFileList.begin();
				   fileIT != headerFileList.end(); ++fileIT) {
		QString fileName = (*fileIT);
		m_umldoc->writeToStatusBar(i18n("Importing file: %1").arg(fileName));
		m_driver->parseFile( fileName );
		TranslationUnitAST *ast = m_driver->translationUnit( fileName );
		if (ast == NULL)
			continue;
		CppTree2Uml modelFeeder( fileName, this );
		// kdDebug() << "Now calling modelFeeder.parseTranslationUnit for file "
		//	   << fileName << endl;
		modelFeeder.parseTranslationUnit( ast );
	}
	m_umldoc->writeToStatusBar("Ready.");
}

