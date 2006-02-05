/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *  copyright (C) 2006                                                     *
 *  Umbrello UML Modeller Authors <uml-devel@ uml.sf.net>                  *
 ***************************************************************************/

// own header
#include "petaltree2uml.h"
// qt/kde includes
#include <qregexp.h>
#include <kdebug.h>
// app includes
#include "petalnode.h"
#include "import_utils.h"
#include "package.h"
#include "classifier.h"
#include "attribute.h"
#include "operation.h"
#include "association.h"
#include "umlrole.h"
#include "uml.h"
#include "umldoc.h"

namespace Import_Rose {

/**
 * Return the given string without surrounding quotation marks.
 * Also remove a possible prefix "Logical View::", it is not modeled in Umbrello.
 */
QString clean(QString str) {
    if (str.isNull())
        return QString::null;
    str.remove("\"");
    str.remove(QRegExp("^Logical View::"));
    return str;
}

/**
 * Extract the quid attribute from a petal node and return it as a Uml::IDType.
 */
Uml::IDType quid(const PetalNode *node) {
    QString quidStr = node->findAttribute("quid").string;
    if (quidStr.isEmpty())
        return Uml::id_None;
    quidStr.remove("\"");
    return STR2ID(quidStr);
}

/**
 * Extract the quidu attribute from a petal node.
 */
QString quidu(const PetalNode *node) {
    QString quiduStr = node->findAttribute("quidu").string;
    if (quiduStr.isEmpty())
        return QString::null;
    quiduStr.remove("\"");
    return quiduStr;
}

/**
 * Determine the model type corresponding to a name.
 * If the given name consists only of letters, digits, underscores, and
 * scope separators, then return Uml::ot_Class, else return Uml::ot_Datatype.
 */
Uml::Object_Type typeToCreate(QString name) {
    name.remove(QRegExp("^.*::"));  // don't consider the scope prefix, it may contain spaces
    Uml::Object_Type t = (name.contains(QRegExp("\\W")) ? Uml::ot_Datatype : Uml::ot_Class);
    return t;
}

/**
 * Transfer the Rose attribute "exportControl" to the Umbrello object given.
 *
 * @param from   Pointer to PetalNode from which to read the "exportControl" attribute
 * @param to     Pointer to UMLObject in which to set the Uml::Visibility
 */
void transferVisibility(const PetalNode *from, UMLObject *to) {
    QString vis = from->findAttribute("exportControl").string;
    if (vis != QString::null) {
        Uml::Visibility v = Uml::Visibility::fromString(clean(vis.lower()));
        to->setVisibility(v);
    }
}

/**
 * ClassifierListReader factors the common processing for attributes, operations,
 * and operation parameters.
 */
class ClassifierListReader {
public:
    /// constructor
    ClassifierListReader(const char* attributeTag,
                         const char* elementName,
                         const char* itemTypeDesignator) :
        m_attributeTag(attributeTag),
        m_elementName(elementName),
        m_itemTypeDesignator(itemTypeDesignator) {
    }
    /// destructor
    virtual ~ClassifierListReader() {}

    /**
     * Return a UMLClassifierListItem of the specific type desired.
     * Abstract method to be implemented by inheriting classes.
     */
    virtual UMLObject *createListItem() = 0;

    /**
     * Insert the given UMLClassifierListItem at the parent Umbrello object.
     * Abstract method to be implemented by inheriting classes.
     * NB the parent Umbrello object is not included in the ClassifierListReader
     * class - it must be added at inheriting classes.
     *
     * @param node   The PetalNode which corresponds to the parent Umbrello object.
     * @param o      The UMLObject to insert.
     */
    virtual void insertAtParent(const PetalNode *node, UMLObject *o) = 0;

    /**
     * Iterate over the attributes of the given PetalNode and for each recognized
     * attribute do the following:
     *   - invoke createListItem()
     *   - fill common properties such as name, unique ID, visibility, etc. into
     *     the new UMLClassifierListItem
     *   - invoke insertAtParent() with the new classifier list item as the argument
     * This is the user entry point.
     */
    void read(const PetalNode *node, QString name) {
        PetalNode *attributes = node->findAttribute(m_attributeTag).node;
        if (attributes == NULL) {
            kdDebug() << "umbrellify(" << name << "): no " << m_attributeTag << " found"
                      << endl;
            return;
        }
        PetalNode::NameValueList attributeList = attributes->attributes();
        for (uint i = 0; i < attributeList.count(); i++) {
            PetalNode *attNode = attributeList[i].second.node;
            QStringList initialArgs = attNode->initialArgs();
            if (attNode->name() != m_elementName) {
                kdDebug() << "umbrellify(" << name << "): expecting " << m_elementName
                          << ", " << "found " << initialArgs[0] << endl;
                continue;
            }
            UMLObject *item = createListItem();
            if (initialArgs.count() > 1)
                item->setName(clean(initialArgs[1]));
            item->setID(quid(attNode));
            QString quidref = quidu(attNode);
            if (!quidref.isEmpty()) {
                item->setSecondaryId(quidref);
            }
            QString type = clean(attNode->findAttribute(m_itemTypeDesignator).string);
            if (!type.isEmpty()) {
                item->setSecondaryFallback(type);
            }
            transferVisibility(attNode, item);
            QString doc = attNode->findAttribute("documentation").string;
            if (! doc.isEmpty())
                item->setDoc(doc);
            insertAtParent(attNode, item);
        }
    }
protected:
    const QString m_attributeTag, m_elementName, m_itemTypeDesignator;
};

class AttributesReader : public ClassifierListReader {
public:
    AttributesReader(UMLClassifier *c)
      : ClassifierListReader("class_attributes", "ClassAttribute", "type") {
        m_classifier = c;
    }
    virtual ~AttributesReader() {}
    UMLObject *createListItem() {
        return new UMLAttribute(m_classifier);
    }
    void insertAtParent(const PetalNode *, UMLObject *item) {
        m_classifier->addAttribute(static_cast<UMLAttribute*>(item));
    }
protected:
    UMLClassifier *m_classifier;
};

class ParametersReader : public ClassifierListReader {
public:
    ParametersReader(UMLOperation *op)
      : ClassifierListReader("parameters", "Parameter", "type") {
        m_operation = op;
    }
    virtual ~ParametersReader() {}
    UMLObject *createListItem() {
        return new UMLAttribute(m_operation);
    }
    void insertAtParent(const PetalNode *, UMLObject *item) {
        m_operation->addParm(static_cast<UMLAttribute*>(item));
    }
protected:
    UMLOperation *m_operation;
};

class OperationsReader : public ClassifierListReader {
public:
    OperationsReader(UMLClassifier *c)
      : ClassifierListReader("operations", "Operation", "result") {
        m_classifier = c;
    }
    virtual ~OperationsReader() {}
    UMLObject *createListItem() {
        return new UMLOperation(m_classifier);
    }
    void insertAtParent(const PetalNode *node, UMLObject *item) {
        UMLOperation *op = static_cast<UMLOperation*>(item);
        ParametersReader parmReader(op);
        parmReader.read(node, m_classifier->getName());
        m_classifier->addOperation(op);
    }
protected:
    UMLClassifier *m_classifier;
};

class SuperclassesReader : public ClassifierListReader {
public:
    SuperclassesReader(UMLClassifier *c)
      : ClassifierListReader("superclasses", "Inheritance_Relationship", "supplier") {
        m_classifier = c;
    }
    virtual ~SuperclassesReader() {}
    UMLObject *createListItem() {
        return new UMLAssociation(Uml::at_Generalization);
    }
    void insertAtParent(const PetalNode *, UMLObject *item) {
        UMLAssociation *assoc = static_cast<UMLAssociation*>(item);
        assoc->setObject(m_classifier, Uml::A);
        // Move the secondary ID and fallback to the role B.
        QString secondaryId = assoc->getSecondaryId();
        QString secondaryFallback = assoc->getSecondaryFallback();
        assoc->getUMLRole(Uml::B)->setSecondaryId(secondaryId);
        assoc->getUMLRole(Uml::B)->setSecondaryFallback(secondaryFallback);
        assoc->setSecondaryId(QString::null);
        assoc->setSecondaryFallback(QString::null);

        UMLApp::app()->getDocument()->addAssociation(assoc);
    }
protected:
    UMLClassifier *m_classifier;
};

/**
 * Create an Umbrello object from a PetalNode.
 *
 * @return   True for success.
 *           Given a PetalNode for which the mapping to Umbrello is not yet
 *           implemented umbrellify() is a no-op but also returns true.
 */
bool umbrellify(PetalNode *node, UMLPackage *parentPkg = NULL) {
    if (node == NULL) {
        kdError() << "umbrellify: node is NULL" << endl;
        return false;
    }
    QStringList args = node->initialArgs();
    QString objType = args[0];
    QString name = clean(args[1]);
    Uml::IDType id = quid(node);
    if (objType == "Class_Category") {
        UMLObject *o = Import_Utils::createUMLObject(Uml::ot_Package, name, parentPkg);
        o->setID(id);
        PetalNode *logical_models = node->findAttribute("logical_models").node;
        if (logical_models == NULL) {
            kdError() << "umbrellify: cannot find logical_models" << endl;
            return false;
        }
        UMLPackage *localParent = static_cast<UMLPackage*>(o);
        PetalNode::NameValueList atts = logical_models->attributes();
        for (uint i = 0; i < atts.count(); i++) {
            umbrellify(atts[i].second.node, localParent);
        }
    } else if (objType == "Class") {
        UMLObject *o = Import_Utils::createUMLObject(Uml::ot_Class, name, parentPkg);
        o->setID(id);
        UMLClassifier *c = static_cast<UMLClassifier*>(o);
        // insert attributes
        AttributesReader attReader(c);
        attReader.read(node, c->getName());
        // insert operations
        OperationsReader opReader(c);
        opReader.read(node, c->getName());
        // insert generalizations
        SuperclassesReader superReader(c);
        superReader.read(node, c->getName());
    } else {
        kdDebug() << "umbrellify: object type " << objType
                  << " is not yet implemented" << endl;
    }
    return true;
}

bool petalTree2Uml(PetalNode *root) {
    if (root == NULL) {
        kdError() << "petalTree2Uml: root is NULL" << endl;
        return false;
    }
    if (root->name() != "Design") {
        kdError() << "petalTree2Uml: expecting root name Design" << endl;
        return false;
    }
    PetalNode *root_category = root->findAttribute("root_category").node;
    if (root_category == NULL) {
        kdError() << "petalTree2Uml: cannot find root_category" << endl;
        return false;
    }
    if (root_category->name() != "Class_Category") {
        kdError() << "petalTree2Uml: expecting root_category object Class_Category" << endl;
        return false;
    }
    PetalNode *logical_models = root_category->findAttribute("logical_models").node;
    if (logical_models == NULL) {
        kdError() << "petalTree2Uml: cannot find logical_models" << endl;
        return false;
    }
    UMLDoc *umldoc = UMLApp::app()->getDocument();
    umldoc->determineNativity("Certainly Not Native At All");
    Import_Utils::assignUniqueIdOnCreation(false);
    PetalNode::NameValueList atts = logical_models->attributes();
    for (uint i = 0; i < atts.count(); i++) {
        umbrellify(atts[i].second.node);
    }
    Import_Utils::assignUniqueIdOnCreation(true);
    umldoc->resolveTypes();
    return true;
}

}  // namespace Import_Rose

