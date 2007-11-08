/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   copyright (C) 2006-2007                                               *
 *   Umbrello UML Modeller Authors <uml-devel@uml.sf.net>                  *
 ***************************************************************************/

// own header
#include "folder.h"
// qt/kde includes
#include <qfile.h>
#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
// app includes
#include "uml.h"
#include "umldoc.h"
#include "umlview.h"
#include "umllistview.h"
#include "optionstate.h"
#include "object_factory.h"
#include "model_utils.h"

UMLFolder::UMLFolder(const QString & name, Uml::IDType id)
        : UMLPackage(name, id) {
    init();
}

UMLFolder::~UMLFolder() {
    // TODO : check if safe
    while ( !m_diagrams.isEmpty() ) {
        delete m_diagrams.takeFirst();
    }
}

void UMLFolder::init() {
    m_BaseType = Uml::ot_Folder;
    // Porting to QList. No autodelete supported. TODO: check if all elements of m_diagrams are properly disposed
    //m_diagrams.setAutoDelete(true);
    UMLObject::setStereotype("folder");
}

UMLObject* UMLFolder::clone() const {
    UMLFolder *clone = new UMLFolder();
    UMLObject::copyInto(clone);
    return clone;
}

void UMLFolder::setLocalName(const QString& localName) {
    m_localName = localName;
}

QString UMLFolder::getLocalName() {
    return m_localName;
}

void UMLFolder::addView(UMLView *view) {
    m_diagrams.append(view);
}

void UMLFolder::removeView(UMLView *view) {
    // m_diagrams is set to autodelete!
    m_diagrams.removeAll(view);
    delete view;
}

void UMLFolder::appendViews(UMLViewList& viewList, bool includeNested) {
    if (includeNested) {
        foreach (UMLObject* o, m_objects ) {
            if (o->getBaseType() == Uml::ot_Folder) {
                UMLFolder *f = static_cast<UMLFolder*>(o);
                f->appendViews(viewList);
            }
        }
    }
    foreach (UMLView* v, m_diagrams )
        viewList.append(v);
}

void UMLFolder::activateViews() {
    foreach (UMLObject* o, m_objects ) {
        if (o->getBaseType() == Uml::ot_Folder) {
            UMLFolder *f = static_cast<UMLFolder*>(o);
            f->activateViews();
        }
    }

    foreach (UMLView* v, m_diagrams)
        v->activateAfterLoad();
    // Make sure we have a treeview item for each diagram.
    // It may happen that we are missing them after switching off tabbed widgets.
    Settings::OptionState optionState = Settings::getOptionState();
    if (optionState.generalState.tabdiagrams)
        return;
    UMLListView *lv = UMLApp::app()->getListView();
    foreach (UMLView* v,  m_diagrams ) {
        if (lv->findItem(v->getID()) != NULL)
            continue;
        lv->createDiagramItem(v);
    }
}

UMLView *UMLFolder::findView(Uml::IDType id) {
    foreach (UMLView* v, m_diagrams ) {
        if (v->getID() == id)
            return v;
    }

    UMLView* v = NULL;
    foreach (UMLObject* o, m_objects ) {
        if (o->getBaseType() != Uml::ot_Folder)
            continue;
        UMLFolder *f = static_cast<UMLFolder*>(o);
        v = f->findView(id);
        if (v)
            break;
    }
    return v;
}

UMLView *UMLFolder::findView(Uml::Diagram_Type type, const QString &name, bool searchAllScopes) {

    foreach (UMLView* v, m_diagrams ) {
        if (v->getType() == type && v->getName() == name)
            return v;
    }

    UMLView* v = NULL;
    if (searchAllScopes) {
        foreach (UMLObject* o, m_objects  ) {
            if (o->getBaseType() != Uml::ot_Folder)
                continue;
            UMLFolder *f = static_cast<UMLFolder*>(o);
            v = f->findView(type, name, searchAllScopes);
            if (v)
                break;
        }
    }
    return v;
}

void UMLFolder::setViewOptions(const Settings::OptionState& optionState) {
    // for each view update settings
    foreach (UMLView* v, m_diagrams )
        v->setOptionState(optionState);
}

void UMLFolder::removeAllViews() {
    foreach (UMLObject* o, m_objects) {
        if (o->getBaseType() != Uml::ot_Folder)
            continue;
        UMLFolder *f = static_cast<UMLFolder*>(o);
        f->removeAllViews();
    }

    foreach (UMLView* v, m_diagrams ) {
        // TODO ------------------ check this code - bad: calling back to UMLDoc::removeView()
        v->removeAllAssociations(); // note : It may not be apparent, but when we remove all associations
        // from a view, it also causes any UMLAssociations that lack parent
        // association widgets (but once had them) to remove themselves from
        // this document.
        UMLApp::app()->getDocument()->removeView(v, false);
    }

    // m_diagrams.clear()
    while ( !m_diagrams.empty() ) {
        delete m_diagrams.takeFirst();
    }
}

void UMLFolder::setFolderFile(const QString& fileName) {
    m_folderFile = fileName;
}

QString UMLFolder::getFolderFile() {
    return m_folderFile;
}

void UMLFolder::saveContents(QDomDocument& qDoc, QDomElement& qElement) {
    QDomElement ownedElement = qDoc.createElement("UML:Namespace.ownedElement");
    UMLObject *obj = NULL;
    // Save contained objects if any.
    for (UMLObjectListIt oit(m_objects); oit.hasNext(); ) {
        obj = oit.next();
        obj->saveToXMI (qDoc, ownedElement);
    }
    // Save asscociations if any.
    for (UMLObjectListIt ait(m_List); ait.hasNext(); ) {
        obj = ait.next();
        obj->saveToXMI (qDoc, ownedElement);
    }
    qElement.appendChild(ownedElement);
    // Save diagrams to `extension'.
    if (m_diagrams.count()) {
        QDomElement diagramsElement = qDoc.createElement("diagrams");

        foreach (UMLView* pView, m_diagrams ) {
            pView->saveToXMI(qDoc, diagramsElement);
        }
        QDomElement extension = qDoc.createElement("XMI.extension");
        extension.setAttribute("xmi.extender", "umbrello");
        extension.appendChild( diagramsElement );
        qElement.appendChild(extension);
    }
}

void UMLFolder::save(QDomDocument& qDoc, QDomElement& qElement) {
    UMLDoc *umldoc = UMLApp::app()->getDocument();
    QString elementName("UML:Package");
    const Uml::Model_Type mt = umldoc->rootFolderType(this);
    if (mt != Uml::N_MODELTYPES)
        elementName = "UML:Model";
    QDomElement folderElement = UMLObject::save(elementName, qDoc);
    saveContents(qDoc, folderElement);
    qElement.appendChild(folderElement);
}

void UMLFolder::saveToXMI(QDomDocument& qDoc, QDomElement& qElement) {
    if (m_folderFile.isEmpty()) {
        save(qDoc, qElement);
        return;
    }
    // See if we can create the external file.
    // If not then internalize the folder.
    UMLDoc *umldoc = UMLApp::app()->getDocument();
    QString fileName = umldoc->url().directory() + '/' + m_folderFile;
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        uError() << m_folderFile << ": "
            << "cannot create file, contents will be saved in main model file"
            << endl;
        m_folderFile.clear();
        save(qDoc, qElement);
        return;
    }
    // Okay, external file is writable.  Wrap up main file.
    QDomElement folderElement = UMLObject::save("UML:Package", qDoc);
    QDomElement extension = qDoc.createElement("XMI.extension");
    extension.setAttribute("xmi.extender", "umbrello");
    QDomElement fileElement = qDoc.createElement("external_file");
    fileElement.setAttribute("name", m_folderFile);
    extension.appendChild(fileElement);
    folderElement.appendChild(extension);
    qElement.appendChild(folderElement);

    // Save folder to external file.
    QDomDocument folderDoc;
    QDomElement folderRoot;
    QDomProcessingInstruction xmlHeading =
        folderDoc.createProcessingInstruction("xml",
                                              "version=\"1.0\" encoding=\"UTF-8\"");
    folderDoc.appendChild(xmlHeading);
    folderRoot = folderDoc.createElement("external_file");
    folderRoot.setAttribute("name", m_Name);
    folderRoot.setAttribute("filename", m_folderFile);
    folderRoot.setAttribute("mainModel", umldoc->url().fileName());
    folderRoot.setAttribute("parentId", ID2STR(m_pUMLPackage->getID()));
    folderRoot.setAttribute("parent", m_pUMLPackage->getFullyQualifiedName("::", true));
    saveContents(folderDoc, folderRoot);
    folderDoc.appendChild(folderRoot);
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    stream << folderDoc.toString();
    file.close();
}

bool UMLFolder::loadDiagramsFromXMI(QDomNode& diagrams) {
    const Settings::OptionState optionState = Settings::getOptionState();
    UMLDoc *umldoc = UMLApp::app()->getDocument();
    bool totalSuccess = true;
    for (QDomElement diagram = diagrams.toElement(); !diagram.isNull();
         diagrams = diagrams.nextSibling(), diagram = diagrams.toElement()) {
        QString tag = diagram.tagName();
        if (tag != "diagram") {
            uDebug() << "ignoring " << tag << " in <diagrams>" << endl;
            continue;
        }
        UMLView * pView = new UMLView(this);
        pView->setOptionState(optionState);
        if (pView->loadFromXMI(diagram)) {
            pView->hide();
            umldoc->addView(pView);
        } else {
            delete pView;
            totalSuccess = false;
        }
    }
    return totalSuccess;
}

bool UMLFolder::loadFolderFile(const QString& path) {
    QFile file(path);
    if (!file.exists()) {
        KMessageBox::error(0, i18n("The folderfile %1 does not exist.", path), i18n("Load Error"));
        return false;
    }
    if (!file.open(QIODevice::ReadOnly)) {
        KMessageBox::error(0, i18n("The folderfile %1 cannot be opened.", path), i18n("Load Error"));
        return false;
    }
    QTextStream stream(&file);
    QString data = stream.readAll();
    file.close();
    QDomDocument doc;
    QString error;
    int line;
    if (!doc.setContent( data, false, &error, &line)) {
        uError() << "Can't set content:" << error << " line:" << line << endl;
        return false;
    }
    QDomNode rootNode = doc.firstChild();
    while (rootNode.isComment() || rootNode.isProcessingInstruction()) {
        rootNode = rootNode.nextSibling();
    }
    if (rootNode.isNull()) {
        uError() << "Root node is Null" << endl;
        return false;
    }
    QDomElement element = rootNode.toElement();
    QString type = element.tagName();
    if (type != "external_file") {
        uError() << "Root node has unknown type " << type << endl;
        return false;
    }
    return load(element);
}

bool UMLFolder::load(QDomElement& element) {
    UMLDoc *umldoc = UMLApp::app()->getDocument();
    bool totalSuccess = true;
    for (QDomNode node = element.firstChild(); !node.isNull();
            node = node.nextSibling()) {
        if (node.isComment())
            continue;
        QDomElement tempElement = node.toElement();
        QString type = tempElement.tagName();
        if (Model_Utils::isCommonXMIAttribute(type))
            continue;
        if (Uml::tagEq(type, "Namespace.ownedElement") ||
                Uml::tagEq(type, "Namespace.contents")) {
            //CHECK: Umbrello currently assumes that nested elements
            // are ownedElements anyway.
            // Therefore these tags are not further interpreted.
            if (! load(tempElement)) {
                uDebug() << "An error happened while loading the " << type
                    << " of the " << m_Name << endl;
                totalSuccess = false;
            }
            continue;
        } else if (type == "XMI.extension") {
            for (QDomNode xtnode = node.firstChild(); !xtnode.isNull();
                                              xtnode = xtnode.nextSibling()) {
                QDomElement el = xtnode.toElement();
                const QString xtag = el.tagName();
                if (xtag == "diagrams") {
                    QDomNode diagramNode = xtnode.firstChild();
                    if (!loadDiagramsFromXMI(diagramNode))
                        totalSuccess = false;
                } else if (xtag == "external_file") {
                    const QString rootDir(umldoc->url().directory());
                    QString fileName = el.attribute("name", "");
                    const QString path(rootDir + '/' + fileName);
                    if (loadFolderFile(path))
                        m_folderFile = fileName;
                } else {
                    uDebug() << m_Name << ": ignoring XMI.extension " << xtag << endl;
                    continue;
                }
            }
            continue;
        }
        // Do not re-create the predefined Datatypes folder in the Logical View,
        // it already exists.
        UMLFolder *logicalView = umldoc->getRootFolder(Uml::mt_Logical);
        if (this == logicalView && Uml::tagEq(type, "Package")) {
            QString thisName = tempElement.attribute("name", "");
            if (thisName == "Datatypes") {
                UMLFolder *datatypeFolder = umldoc->getDatatypeFolder();
                if (!datatypeFolder->loadFromXMI(tempElement))
                    totalSuccess = false;
                continue;
            }
        }
        UMLObject *pObject = NULL;
        // Avoid duplicate creation of forward declared object
        QString idStr = tempElement.attribute("xmi.id", "");
        if (!idStr.isEmpty()) {
            Uml::IDType id = STR2ID(idStr);
            pObject = umldoc->findObjectById(id);
            if (pObject) {
                uDebug() << "object " << idStr << "already exists" << endl;
            }
        }
        if (pObject == NULL) {
            QString stereoID = tempElement.attribute("stereotype", "");
            pObject = Object_Factory::makeObjectFromXMI(type, stereoID);
            if (!pObject) {
                uWarning() << "Unknown type of umlobject to create: " << type << endl;
                continue;
            }
        }
        pObject->setUMLPackage(this);
        if (!pObject->loadFromXMI(tempElement)) {
            removeObject(pObject);
            delete pObject;
            totalSuccess = false;
        }
    }
    return totalSuccess;
}

#include "folder.moc"
