/***************************************************************************
                          docbookgenerator.h  -  description
                             -------------------
    begin                : THu Jun 22 2006
    copyright            : (C) 2006 by Gael de Chalendar (aka Kleag)
    email                : kleag@free.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DOCBOOKGENERATOR_H
#define DOCBOOKGENERATOR_H

#include <kurl.h>
#include <qobject.h>

class UMLDoc;

class DocbookGeneratorJob;
/**
 * class DocbookGenerator is a documentation generator for UML documents.
 * It uses libxslt to convert the XMI generated by UMLDoc::saveToXMI through
 * the XSLT file stored in resources.
 *
 * @todo Add configure checks for libxml2 and libxslt and use conditional
 * compilation of this library and its callers
 * @todo allow to specify the destination and ensure that it works with distant
 * ones
 */
class DocbookGenerator : public QObject
{
  Q_OBJECT
  public:

    /**
     * Constructor
     */
    DocbookGenerator();

    /**
     * Empty Destructor
     */
    virtual ~DocbookGenerator();

    /**
     * Exports the current model to docbook in a directory named as the model
     * with the .xmi suffix removed. The docbook file will have the same name
     * with the .docbook suffix. Figures will be named as the corresponding
     * diagrams in the GUI
     * @todo change file naming to avoid paths with spaces or non-ASCII chars
     * @todo better handling of error conditions
     * @return true if saving is successful and false otherwise.
     */
    bool generateDocbookForProject();

    /**
     * Exports the current model to docbook in the given directory
     * @param destDir the directory where the docbook file and the figures will
     * be written
     * @todo better handling of error conditions
     * @return true if saving is successful and false otherwise.
     */
    void generateDocbookForProjectInto(const KUrl& destDir);

  signals:
 
    /** Emitted when the documentation generation is finished 
     * @param status true if success else false
     */
    void finished(bool status);
  
  private slots:
  
    void slotDocbookGenerationFinished(const QString&);
   
    void threadFinished();

  private:

    DocbookGeneratorJob* docbookGeneratorJob;

    bool m_pStatus;
    bool m_pThreadFinished;
    KUrl m_destDir;
    UMLDoc* umlDoc;
};

#endif // DOCBOOKGENERATOR_H
