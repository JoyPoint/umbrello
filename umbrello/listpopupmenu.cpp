/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <kstddirs.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kdebug.h>

#include "listpopupmenu.h"
#include "umlwidget.h"
#include "conceptwidget.h"
#include "floatingtext.h"
#include "umlview.h"
#include "notewidget.h"
#include "statewidget.h"
#include "activitywidget.h"

ListPopupMenu::ListPopupMenu(QWidget *parent, Menu_Type type, UMLView * view) : KPopupMenu(parent) {
	setupMenu(type, view);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
ListPopupMenu::ListPopupMenu(QWidget *parent, Uml::ListView_Type type) : KPopupMenu(parent) {
	Menu_Type mt = mt_Undefined;
	switch(type)
	{
		case Uml::lvt_Logical_View:
			mt = mt_Logical_View;
			break;

		case Uml::lvt_UseCase_View:
			mt = mt_UseCase_View;
			break;

		case Uml::lvt_Logical_Folder:
			mt = mt_Logical_Folder;
			break;

		case Uml::lvt_UseCase_Folder:
			mt = mt_UseCase_Folder;
			break;

		case Uml::lvt_UseCase_Diagram:
			mt = mt_UseCase_Diagram;
			break;

		case Uml::lvt_Class_Diagram:
			mt = mt_Class_Diagram;
			break;

		case Uml::lvt_Collaboration_Diagram:
			mt = mt_Collaboration_Diagram;
			break;

		case Uml::lvt_Sequence_Diagram:
			mt = mt_Sequence_Diagram;
			break;

		case Uml::lvt_State_Diagram:
			mt = mt_State_Diagram;
			break;

		case Uml::lvt_Activity_Diagram:
			mt = mt_Activity_Diagram;
			break;

		case Uml::lvt_Actor:
			mt = mt_Actor;
			break;

		case Uml::lvt_UseCase:
			mt = mt_UseCase;
			break;

		case Uml::lvt_Class:
			mt = mt_Concept;
			break;

		case Uml::lvt_Package:
			mt = mt_Package;
			break;

		case Uml::lvt_Attribute:
			mt = mt_Attribute;
			break;

		case Uml::lvt_Operation:
			mt = mt_Operation;
			break;
		default:   ;
			//break;
	}
	setupMenu(mt);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
ListPopupMenu::ListPopupMenu(QWidget * parent, UMLWidget * object, bool multi) : KPopupMenu(parent)
{
	//make the right menu for the type
	//make menu for logical view
	m_pInsert = 0;
	m_pShow = 0;
	m_pColor = 0;
	bool sig = false;
	if(!object)
		return;
	StateWidget * pState = static_cast< StateWidget *>( object );
	ActivityWidget * pActivity = static_cast<ActivityWidget *>( object );
	ConceptWidget * c = static_cast< ConceptWidget *>( object );
	UMLView * pView = static_cast<UMLView *>( parent );
	Uml::UMLWidget_Type type = object -> getBaseType();

	if(multi) {
		setupColor(object -> getUseFillColor());
		insertSeparator();
		insertItem(SmallIcon( "editcut" ) , i18n("Cut"), mt_Cut);
		insertItem(SmallIcon( "editcopy" ), i18n("Copy"), mt_Copy);
		insertItem(SmallIcon( "editpaste"), i18n("Paste"), mt_Paste);
		insertSeparator();
		insertItem(SmallIcon( "fonts" ), i18n( "Change Font..." ), mt_Change_Font_Selection );
		insertItem(SmallIcon( "editdelete" ), i18n("Delete Selected Items"), mt_Delete_Selection);
		if(m_pInsert)
			connect(m_pInsert, SIGNAL(activated(int)), this, SIGNAL(activated(int)));
		if(m_pShow)
			connect(m_pShow, SIGNAL(activated(int)), this, SIGNAL(activated(int)));
		if(m_pColor)
			connect(m_pColor, SIGNAL(activated(int)), this, SIGNAL(activated(int)));
		return;
	}

	switch(type) {
		case Uml::wt_Actor:
		case Uml::wt_UseCase:
			setupColor(object -> getUseFillColor());
			insertSeparator();
			insertItem( SmallIcon( "editcut" ) , i18n("Cut"), mt_Cut);
			insertItem( SmallIcon( "editcopy" ), i18n("Copy"), mt_Copy);
			insertItem( SmallIcon( "editpaste" ), i18n("Paste"), mt_Paste);
			insertSeparator();
			insertItem( SmallIcon( "editdelete" ), i18n("Delete"), mt_Delete);
			insertItem( SmallIcon( "charset" ), i18n("Rename..."), mt_Rename);
			insertItem( SmallIcon( "fonts" ),  i18n( "Change Font..." ), mt_Change_Font );
			insertItem( SmallIcon( "info" ), i18n("Properties"), mt_Properties);
			break;

		case Uml::wt_Class:
			m_pInsert = new KPopupMenu(this,"New");
			m_pInsert -> insertItem(SmallIcon( "source" ), i18n("Attribute"), mt_Attribute);
			m_pInsert -> insertItem( SmallIcon( "source"), i18n("Operation"), mt_Operation);
			insertItem(SmallIcon( "filenew"),i18n("New"), m_pInsert);

			m_pShow = new KPopupMenu(this, "Show");
			m_pShow -> setCheckable(true);
			m_pShow -> insertItem( i18n("Attributes"), mt_Show_Attributes);
			m_pShow -> setItemChecked(mt_Show_Attributes, c -> getShowAtts());
			m_pShow -> insertItem( i18n("Operations"), mt_Show_Operations);
			m_pShow -> setItemChecked(mt_Show_Operations, c -> getShowOps());
			m_pShow -> insertItem(i18n("Visibility"), mt_Scope);
			m_pShow -> setItemChecked(mt_Scope, c -> getShowScope());
			m_pShow -> insertItem(i18n("Operation Signature"), mt_Show_Operation_Signature);
			sig = false;
			if( c -> getShowOpSigs() == Uml::st_SigNoScope ||
			        c -> getShowOpSigs() == Uml::st_ShowSig)
				sig = true;
			m_pShow -> setItemChecked(mt_Show_Operation_Signature, sig);
			m_pShow -> insertItem(i18n("Attribute Signature"), mt_Show_Attribute_Signature);
			sig = false;
			if( c -> getShowAttSigs() == Uml::st_SigNoScope ||
			        c -> getShowAttSigs() == Uml::st_ShowSig)
				sig = true;
			m_pShow -> setItemChecked(mt_Show_Attribute_Signature, sig);
			insertItem(SmallIcon( "info"),i18n("Show"), m_pShow);

			setupColor(object -> getUseFillColor());
			insertSeparator();
			insertItem( SmallIcon( "editcut"), i18n("Cut"), mt_Cut);
			insertItem( SmallIcon( "editcopy"), i18n("Copy"), mt_Copy);
			insertItem( SmallIcon( "editpaste"), i18n("Paste"), mt_Paste);
			insertSeparator();
			insertItem( SmallIcon( "editdelete"), i18n("Delete"), mt_Delete);
			insertItem( SmallIcon( "charset"), i18n("Rename..."), mt_Rename);
			insertItem( SmallIcon( "fonts"),  i18n( "Change Font..." ), mt_Change_Font );
			insertItem(SmallIcon( "info"), i18n("Properties"), mt_Properties);
			break;

		case Uml::wt_Package:
			setupColor(object->getUseFillColor());
			insertItem(SmallIcon("editcut"), i18n("Cut"), mt_Cut);
			insertItem(SmallIcon("editcopy"), i18n("Copy"), mt_Copy);
			insertItem(SmallIcon("editpaste"), i18n("Paste"), mt_Paste);
			insertSeparator();
			insertItem(SmallIcon("editdelete"), i18n("Delete"), mt_Delete);
			insertItem(SmallIcon("charset"), i18n("Rename..."), mt_Rename);
			insertItem(SmallIcon("fonts"),  i18n("Change Font..."), mt_Change_Font );
			insertItem(SmallIcon("info"), i18n("Properties"), mt_Properties);
			break;

		case Uml::wt_Object:
			if( pView -> getType() == Uml::dt_Sequence ) {
				m_pShow = new KPopupMenu( this, "Tab" );
				m_pShow -> insertItem( SmallIcon( "1uparrow"), i18n("Up"), mt_Up);
				m_pShow -> insertItem( SmallIcon( "1downarrow"), i18n("Down"), mt_Down);
				insertItem(SmallIcon( "misc"),i18n("Tab"), m_pShow);
			}
			setupColor(object -> getUseFillColor());
			insertSeparator();
			insertItem( SmallIcon( "editcut"), i18n("Cut"), mt_Cut);
			insertItem( SmallIcon( "editcopy"), i18n("Copy"), mt_Copy);
			insertItem( SmallIcon( "editpaste"), i18n("Paste"), mt_Paste);
			insertSeparator();
			insertItem(SmallIcon( "editdelete"), i18n( "Delete"), mt_Delete);
			insertItem( SmallIcon( "charset"), i18n("Rename Class..."), mt_Rename);
			insertItem(SmallIcon( "charset"), i18n("Rename Object..."), mt_Rename_Object);
			insertItem( SmallIcon( "fonts"),  i18n( "Change Font..." ), mt_Change_Font );
			insertItem(SmallIcon( "info"), i18n("Properties"), mt_Properties);
			break;

		case Uml::wt_Message:
			insertItem(SmallIcon( "editcut"), i18n("Cut"), mt_Cut);
			insertItem(SmallIcon( "editcopy"), i18n("Copy"), mt_Copy);
			insertItem(SmallIcon( "editpaste"), i18n("Paste"), mt_Paste);
			insertSeparator();
			insertItem(SmallIcon( "editdelete"), i18n("Delete"), mt_Delete);
			insertItem(SmallIcon( "fonts"),  i18n( "Change Font..." ), mt_Change_Font );
			insertItem(SmallIcon( "filenew"), i18n("New Operation..."), mt_Operation);
			insertItem(SmallIcon( "charset"), i18n("Select Operation..."), mt_Select_Operation);
			insertItem(SmallIcon( "charset"), i18n("Set Sequence Number..."), mt_Sequence_Number);
			break;

		case Uml::wt_Note:
			setupColor(object -> getUseFillColor());
			insertSeparator();
			insertItem(SmallIcon( "editcut"), i18n("Cut"), mt_Cut);
			insertItem(SmallIcon( "editcopy"), i18n("Copy"), mt_Copy);
			insertItem(SmallIcon( "editpaste"), i18n("Paste"), mt_Paste);
			insertItem(SmallIcon( "editdelete"), i18n("Clear"), mt_Clear);
			insertSeparator();
			insertItem(SmallIcon( "charset"), i18n("Change Text..."), mt_Rename);
			insertItem(SmallIcon( "editDelete"), i18n("Delete"), mt_Delete);
			insertItem(SmallIcon( "fonts"),  i18n( "Change Font..." ), mt_Change_Font );
			//insertItem(i18n("link documentation"), mt_Link_Docs);
			//setItemChecked(mt_Link_Docs, ((NoteWidget*)object)->getLinkState());
			break;

		case Uml::wt_State:
			if( pState -> getStateType() == StateWidget::Normal ) {
				m_pInsert = new KPopupMenu(this,"New");
				m_pInsert -> insertItem(SmallIcon( "filenew"), i18n("Activity..."), mt_New_Activity);
				insertItem(SmallIcon( "filenew"),i18n("New"), m_pInsert);
			}
			setupColor( object -> getUseFillColor() );
			insertItem(SmallIcon( "editcut"), i18n("Cut"), mt_Cut);
			insertItem(SmallIcon( "editcopy"), i18n("Copy"), mt_Copy);
			insertItem(SmallIcon( "editpaste"), i18n("Paste"), mt_Paste);
			insertSeparator();
			insertItem(SmallIcon( "editdelete"), i18n("Delete"), mt_Delete);
			if( pState -> getStateType() == StateWidget::Normal ) {
				insertItem(SmallIcon( "charset"), i18n("Change State Name..."), mt_Rename);
				insertItem(SmallIcon( "fonts"),  i18n( "Change Font..." ), mt_Change_Font );
				insertItem(SmallIcon( "info"), i18n("Properties"), mt_Properties);
			}
			break;

		case Uml::wt_Activity:
			if( pActivity -> getActivityType() == ActivityWidget::Normal )
				setupColor( object -> getUseFillColor() );
			insertItem(SmallIcon( "editcut"), i18n("Cut"), mt_Cut);
			insertItem(SmallIcon( "editcopy"), i18n("Copy"), mt_Copy);
			insertItem(SmallIcon( "editpaste"), i18n("Paste"), mt_Paste);
			insertSeparator();
			insertItem(SmallIcon( "editdelete"), i18n("Delete"), mt_Delete);
			if( pActivity -> getActivityType() == ActivityWidget::Normal ) {
				insertItem(SmallIcon( "charset"), i18n("Change Activity Name"), mt_Rename);
				insertItem(SmallIcon( "fonts"),  i18n( "Change Font..." ), mt_Change_Font );
				insertItem(SmallIcon( "info"), i18n("Properties"), mt_Properties);
			}
			break;

		case Uml::wt_Text:
			switch( (static_cast<FloatingText*>(object))->getRole() ) {
				case Uml::tr_MultiB:
				case Uml::tr_MultiA:
 				case Uml::tr_Name:
 				case Uml::tr_RoleAName:
 				case Uml::tr_RoleBName:
				case Uml::tr_ChangeA:
				case Uml::tr_ChangeB:
					insertItem(SmallIcon("editdelete"), i18n("Delete Association"), mt_Delete_Association);
					insertItem(SmallIcon("fonts"),  i18n("Change Font..."), mt_Change_Font );
  					insertItem(i18n("Reset Label Positions"), mt_Reset_Label_Positions);
  					insertItem(SmallIcon( "info"), i18n("Properties..."), mt_Properties);
  					break;

  				case Uml::tr_Coll_Message_Self:
  				case Uml::tr_Coll_Message:
  				case Uml::tr_Seq_Message_Self:
				case Uml::tr_Seq_Message:
//					insertItem(SmallIcon( "editcut"), i18n("Cut"), mt_Cut);
//					insertItem(SmallIcon( "editcopy"), i18n("Copy"), mt_Copy);
//					insertItem(SmallIcon( "editpaste"), i18n("Paste"), mt_Paste);
//					insertSeparator();
					insertItem(SmallIcon( "editdelete"), i18n("Delete"), mt_Delete_Message);
					insertItem(SmallIcon( "fonts"),  i18n( "Change Font..." ), mt_Change_Font );
					insertItem(SmallIcon( "filenew"), i18n("New Operation..."), mt_Operation);
					insertItem(SmallIcon( "charset"), i18n("Select Operation..."), mt_Select_Operation);
					insertItem(SmallIcon( "charset"), i18n("Set Sequence Number..."), mt_Sequence_Number);
					break;

				case Uml::tr_Floating:
				default:
					insertItem(SmallIcon( "editcut"), i18n("Cut"), mt_Cut);
					insertItem(SmallIcon( "editcopy"), i18n("Copy"), mt_Copy);
					insertItem(SmallIcon( "editpaste"), i18n("Paste"), mt_Paste);
					insertSeparator();
					insertItem(SmallIcon( "editdelete"), i18n("Delete"), mt_Delete);
					insertItem(SmallIcon( "charset"), i18n("Change Text..."), mt_Rename);
					insertItem(SmallIcon( "fonts"),  i18n( "Change Font..." ), mt_Change_Font );
					break;
			}
			break;
		default:
			break;
	}//end switch

	if(m_pInsert)
		connect(m_pInsert, SIGNAL(activated(int)), this, SIGNAL(activated(int)));
	if(m_pShow)
		connect(m_pShow, SIGNAL(activated(int)), this, SIGNAL(activated(int)));
	if(m_pColor)
		connect(m_pColor, SIGNAL(activated(int)), this, SIGNAL(activated(int)));

	bool bCutState = pView -> getDocument() -> getCutCopyState();
	setItemEnabled( mt_Cut, bCutState );
	setItemEnabled( mt_Copy, bCutState );
	setItemEnabled( mt_Paste, false );
}
////////////////////////////////////////////////////////////////////////////////////////////////////
ListPopupMenu::~ListPopupMenu() {}
////////////////////////////////////////////////////////////////////////////////////////////////////
void ListPopupMenu::setupColor(bool fc)
{
	m_pColor = new KPopupMenu( this, "Colour");
	m_pColor -> insertItem(SmallIcon( "color_line"), i18n("Line Color"), mt_Line_Color);
	m_pColor -> insertItem(SmallIcon( "color_fill"), i18n("Fill Color"), mt_Fill_Color);
	m_pColor -> insertItem( i18n("Use Fill Color"), mt_Use_Fill_Color);

	m_pColor -> setItemChecked(mt_Use_Fill_Color, fc);
	insertItem(SmallIcon( "colorize"), i18n("Color"), m_pColor);
}

Uml::Diagram_Type ListPopupMenu::convert_MT_DT(Menu_Type mt) {
	Uml::Diagram_Type type =  Uml::dt_Class;

	switch(mt) {
		case mt_UseCase_Diagram:
			type = Uml::dt_UseCase;
			break;

		case mt_Class_Diagram:
			type = Uml::dt_Class;
			break;

		case mt_Sequence_Diagram:
			type = Uml::dt_Sequence;
			break;

		case mt_Collaboration_Diagram:
			type = Uml::dt_Collaboration;
			break;

		case mt_State_Diagram:
			type = Uml::dt_State;
			break;

		case mt_Activity_Diagram:
			type = Uml::dt_Activity;
			break;

		default:
			break;
	}
	return type;
}

Uml::UMLObject_Type ListPopupMenu::convert_MT_OT(Menu_Type mt) {
	Uml::UMLObject_Type type =  Uml::ot_UMLObject;

	switch(mt) {
		case mt_UseCase:
			type = Uml::ot_UseCase;
			break;

		case mt_Actor:
			type = Uml::ot_Actor;
			break;

		case mt_Concept:
			type = Uml::ot_Concept;

			break;

		case mt_Attribute:

			type = Uml::ot_Attribute;
			break;

		case mt_Operation:
			type = Uml::ot_Operation;
			break;
		default:
			break;
	}
	return type;
}

void ListPopupMenu::setupMenu(Menu_Type type, UMLView* view) {
	//make the right menu for the type
	//make menu for logical view
	m_pInsert = 0;

	m_pShow = 0;
	m_pColor = 0;

	KStandardDirs* dirs = KGlobal::dirs();
	QString dataDir = dirs->findResourceDir("data", "umbrello/pics/object.xpm");
	dataDir += "/umbrello/pics/";
	QPixmap classPixmap(dataDir+"umlclass.xpm");
	QPixmap packagePixmap(dataDir+"package.xpm");
	QPixmap actorPixmap(dataDir+"actor.xpm");
	QPixmap usecasePixmap(dataDir+"case.xpm");
	QPixmap initialStatePixmap(dataDir+"initial_state.xpm");
	QPixmap endStatePixmap(dataDir+"end_state.xpm");
	QPixmap branchPixmap(dataDir+"branch.xpm");
	QPixmap objectPixmap(dataDir+"object.xpm");

	switch(type) {
		case mt_Logical_View:
			m_pInsert = new KPopupMenu(this, "New");
			m_pInsert -> insertItem(SmallIcon( "folder_new"), i18n("Folder"), mt_Logical_Folder);
			m_pInsert -> insertItem(classPixmap, i18n("Class..."), mt_Concept);
			m_pInsert->insertItem(packagePixmap, i18n("Package..."), mt_Package);
			m_pInsert -> insertItem(SmallIcon( "folder_green"),i18n("Class Diagram"), mt_Class_Diagram);
			m_pInsert -> insertItem(SmallIcon( "folder_green"),i18n("State Diagram"), mt_State_Diagram);
			m_pInsert -> insertItem(SmallIcon( "folder_green"),i18n("Activity Diagram"), mt_Activity_Diagram);
			m_pInsert -> insertItem(SmallIcon( "folder_green"),i18n("Sequence Diagram"), mt_Sequence_Diagram);
			m_pInsert -> insertItem(SmallIcon( "folder_green"),i18n("Collaboration Diagram"), mt_Collaboration_Diagram);
			insertItem(SmallIcon( "filenew"), i18n("New"), m_pInsert);
			insertSeparator();
//			insertItem(SmallIcon( "editcut"), i18n("Cut"), mt_Cut);
//			insertItem(SmallIcon( "editcopy"), i18n("Copy"), mt_Copy);
			insertItem(SmallIcon( "editpaste"), i18n("Paste"), mt_Paste);
			insertSeparator();
//			insertItem(SmallIcon( "editdelete"), i18n("Delete"), mt_Delete);
			insertItem(SmallIcon( "source_cpp"), i18n("Import Classes..."), mt_Import_Classes);
			insertSeparator();
			insertItem(i18n("Expand All"),mt_Expand_All);
			insertItem(i18n("Collapse All"),mt_Collapse_All);
			break;


		case mt_UseCase_View:
			m_pInsert = new KPopupMenu(this,"New");
			m_pInsert -> insertItem(SmallIcon( "folder_new"),i18n("Folder"), mt_UseCase_Folder);
			m_pInsert -> insertItem(actorPixmap, i18n("Actor"), mt_Actor);
			m_pInsert -> insertItem(usecasePixmap, i18n("Use Case"), mt_UseCase);
			m_pInsert -> insertItem(SmallIcon( "folder_grey"),i18n("Use Case Diagram"), mt_UseCase_Diagram);
			insertItem(SmallIcon( "filenew"), i18n("New"), m_pInsert);
			insertSeparator();
//			insertItem(SmallIcon( "editcut"), i18n("Cut"), mt_Cut);
//			insertItem(SmallIcon( "editcopy"), i18n("Copy"), mt_Copy);
			insertItem(SmallIcon( "editpaste"), i18n("Paste"), mt_Paste);
			insertSeparator();
			insertItem(i18n("Expand All"),mt_Expand_All);
			insertItem(i18n("Collapse All"),mt_Collapse_All);
			break;

		case mt_Logical_Folder:
			m_pInsert = new KPopupMenu(this, "New");
			m_pInsert -> insertItem(SmallIcon( "folder_new"),i18n("Folder"), mt_Logical_Folder);
			m_pInsert -> insertItem(classPixmap, i18n("Class..."), mt_Concept);
			m_pInsert -> insertItem(packagePixmap, i18n("Package..."), mt_Package);
			m_pInsert -> insertItem(SmallIcon( "folder_green"),i18n("Class Diagram"), mt_Class_Diagram);
			m_pInsert -> insertItem(SmallIcon( "folder_green"),i18n("State Diagram"), mt_State_Diagram);

			m_pInsert -> insertItem(SmallIcon( "folder_green"),i18n("Activity Diagram"), mt_Activity_Diagram);
			m_pInsert -> insertItem(SmallIcon( "folder_green"),i18n("Sequence Diagram"), mt_Sequence_Diagram);
			m_pInsert -> insertItem(SmallIcon( "folder_green"),i18n("Collaboration Diagram"), mt_Collaboration_Diagram);
			insertItem(SmallIcon( "filenew"), i18n("New"), m_pInsert);
			insertSeparator();
			insertItem(SmallIcon( "editcut"), i18n("Cut"), mt_Cut);

			insertItem(SmallIcon( "editcopy"), i18n("Copy"), mt_Copy);
			insertItem(SmallIcon( "editpaste"), i18n("Paste"), mt_Paste);
			insertSeparator();
			insertItem(SmallIcon( "charset"), i18n("Rename..."), mt_Rename);
			insertItem(SmallIcon( "editdelete"), i18n("Delete"), mt_Delete);

			insertItem(SmallIcon( "source_cpp"), i18n("Import Classes..."), mt_Import_Classes);
			insertSeparator();
			insertItem(i18n("Expand All"),mt_Expand_All);
			insertItem(i18n("Collapse All"),mt_Collapse_All);
			break;

		case mt_UseCase_Folder:
			m_pInsert = new KPopupMenu(this,"New");
			m_pInsert -> insertItem(SmallIcon( "folder_new"),i18n("Folder"), mt_UseCase_Folder);
			m_pInsert -> insertItem(actorPixmap, i18n("Actor"), mt_Actor);
			m_pInsert -> insertItem(usecasePixmap, i18n("Use Case"), mt_UseCase);
			m_pInsert -> insertItem(SmallIcon( "folder_grey"),i18n("Use Case Diagram"), mt_UseCase_Diagram);
			insertItem(SmallIcon( "filenew"), i18n("New"), m_pInsert);
			insertSeparator();
			insertItem(SmallIcon( "editcut"), i18n("Cut"), mt_Cut);
			insertItem(SmallIcon( "editcopy"), i18n("Copy"), mt_Copy);
			insertItem(SmallIcon( "editpaste"), i18n("Paste"), mt_Paste);
			insertSeparator();
			insertItem(SmallIcon( "charset"), i18n("Rename..."), mt_Rename);
			insertItem(SmallIcon( "editdelete"), i18n("Delete"), mt_Delete);
			insertSeparator();
			insertItem(i18n("Expand All"),mt_Expand_All);
			insertItem(i18n("Collapse All"),mt_Collapse_All);
			break;

		case mt_UseCase_Diagram:
		case mt_Sequence_Diagram:
		case mt_Class_Diagram:
		case mt_Collaboration_Diagram:
		case mt_State_Diagram:
		case mt_Activity_Diagram:
			insertItem(SmallIcon( "editcut"), i18n("Cut"), mt_Cut);
			insertItem(SmallIcon( "editcopy"), i18n("Copy"), mt_Copy);
			insertItem(SmallIcon( "editpaste"), i18n("Paste"), mt_Paste);
			insertSeparator();
			insertItem(SmallIcon( "charset"), i18n("Rename..."), mt_Rename);
			insertItem(SmallIcon( "editdelete"), i18n("Delete"), mt_Delete);
			insertItem(SmallIcon( "image"), i18n("Export as Picture..."), mt_Export_Image);
			insertItem(SmallIcon( "info"), i18n("Properties"), mt_Properties);
			break;

		case mt_On_UseCase_Diagram:
			m_pInsert = new KPopupMenu( this, "New" );
			m_pInsert -> insertItem(actorPixmap, i18n( "Actor..." ), mt_Actor );
			m_pInsert -> insertItem(usecasePixmap, i18n( "Use Case..."), mt_UseCase );
			m_pInsert -> insertItem(SmallIcon( "text"), i18n( "Text Line..." ), mt_FloatText );
			insertItem(SmallIcon( "filenew"), i18n("New"), m_pInsert);
			insertSeparator();
			setupDiagramMenu(view);
			break;

		case mt_On_Class_Diagram:
			m_pInsert = new KPopupMenu( this, "New" );
			m_pInsert -> insertItem(classPixmap, i18n("Class..."), mt_Concept);
			m_pInsert -> insertItem(packagePixmap, i18n("Package..."), mt_Package);
			m_pInsert -> insertItem(SmallIcon( "text"), i18n( "Text Line..." ), mt_FloatText );
			insertItem(SmallIcon( "filenew"), i18n("New"), m_pInsert);
			insertSeparator();
			setupDiagramMenu(view);
			break;

		case mt_On_State_Diagram:
			m_pInsert = new KPopupMenu( this, "New" );
			m_pInsert -> insertItem(initialStatePixmap, i18n("Initial State"), mt_Initial_State );
			m_pInsert -> insertItem(endStatePixmap, i18n("End State"), mt_End_State );
			m_pInsert -> insertItem(usecasePixmap, i18n("State..."), mt_State );
			m_pInsert -> insertItem(SmallIcon( "text"), i18n( "Text Line..." ), mt_FloatText );
			insertItem(SmallIcon( "filenew"), i18n("New"), m_pInsert);
			insertSeparator();
			setupDiagramMenu(view);
			break;

		case mt_On_Activity_Diagram:
			m_pInsert = new KPopupMenu( this, "New" );
			m_pInsert -> insertItem(initialStatePixmap, i18n("Initial Activity"), mt_Initial_Activity );
			m_pInsert -> insertItem(endStatePixmap, i18n("End Activity"), mt_End_Activity );
			m_pInsert -> insertItem(usecasePixmap, i18n("Activity..."), mt_Activity );
			m_pInsert -> insertItem(branchPixmap, i18n("Branch/Merge"), mt_Branch );
			m_pInsert -> insertItem(SmallIcon( "text"), i18n( "Text Line..." ), mt_FloatText );
			insertItem(SmallIcon( "filenew"), i18n("New"), m_pInsert);
			insertSeparator();
			setupDiagramMenu(view);
			break;

		case mt_On_Sequence_Diagram:
		case mt_On_Collaboration_Diagram:
			m_pInsert = new KPopupMenu( this, "New" );
			m_pInsert -> insertItem(objectPixmap, i18n("Object..."), mt_Object);
			m_pInsert -> insertItem( SmallIcon( "text"),i18n( "Text Line..." ), mt_FloatText );
			insertItem(SmallIcon( "filenew"), i18n("New"), m_pInsert);
			insertSeparator();
			setupDiagramMenu(view);
			break;

		case mt_Concept:
			m_pInsert = new KPopupMenu(this,"New");
			m_pInsert -> insertItem(SmallIcon( "source"), i18n("Attribute"), mt_Attribute);
			m_pInsert -> insertItem(SmallIcon( "source"), i18n("Operation"), mt_Operation);
			insertItem(SmallIcon( "filenew"), i18n("New"), m_pInsert);
			insertSeparator();
			insertItem(SmallIcon( "editcut"), i18n("Cut"), mt_Cut);
			insertItem(SmallIcon( "editcopy"), i18n("Copy"), mt_Copy);
			insertItem(SmallIcon( "editpaste"), i18n("Paste"), mt_Paste);
			insertSeparator();
			insertItem(SmallIcon( "charset"), i18n("Rename..."), mt_Rename);
			insertItem(SmallIcon( "editdelete"), i18n("Delete"), mt_Delete);
			insertItem(SmallIcon( "info"), i18n("Properties"), mt_Properties);
			break;

		case mt_Package:
			insertItem(SmallIcon("editcut"), i18n("Cut"), mt_Cut);
			insertItem(SmallIcon("editcopy"), i18n("Copy"), mt_Copy);
			insertItem(SmallIcon("editpaste"), i18n("Paste"), mt_Paste);
			insertSeparator();
			insertItem(SmallIcon("charset"), i18n("Rename..."), mt_Rename);
			insertItem(SmallIcon("editdelete"), i18n("Delete"), mt_Delete);
			insertItem(SmallIcon("info"), i18n("Properties"), mt_Properties);
			break;

		case mt_Actor:
		case mt_UseCase:
			insertItem(SmallIcon( "editcut"), i18n("Cut"), mt_Cut);
			insertItem(SmallIcon( "editcopy"), i18n("Copy"), mt_Copy);
			insertItem(SmallIcon( "editpaste"), i18n("Paste"), mt_Paste);
			insertSeparator();
			insertItem(SmallIcon( "charset"), i18n("Rename..."), mt_Rename);
			insertItem(SmallIcon( "editdelete"), i18n("Delete"), mt_Delete);
			insertItem(SmallIcon( "info"), i18n("Properties"), mt_Properties);
			break;

		case mt_Attribute:
			insertItem(SmallIcon( "editcut"), i18n("Cut"), mt_Cut);
			insertItem(SmallIcon( "editcopy"), i18n("Copy"), mt_Copy);
			insertItem(SmallIcon( "editpaste"), i18n("Paste"), mt_Paste);
			insertSeparator();
			insertItem(SmallIcon( "charset"), i18n("Rename..."), mt_Rename);
			insertItem(SmallIcon( "editdelete"), i18n("Delete"), mt_Delete);
			insertItem(SmallIcon( "info"), i18n("Properties"), mt_Properties);
			break;

		case mt_Operation:
			insertItem(SmallIcon( "editcut"), i18n("Cut"), mt_Cut);
			insertItem(SmallIcon( "editcopy"), i18n("Copy"), mt_Copy);
			insertItem(SmallIcon( "editpaste"), i18n("Paste"), mt_Paste);
			insertSeparator();
			insertItem(SmallIcon( "charset"), i18n("Rename..."), mt_Rename);
			insertItem(SmallIcon( "editdelete"), i18n("Delete"), mt_Delete);
			insertItem(SmallIcon( "info"), i18n("Properties"), mt_Properties);
			break;

		case mt_New_Parameter:
			insertItem(SmallIcon( "source"),i18n("New Parameter..."), mt_New_Parameter);
			break;

		case mt_New_Operation:
		insertItem(SmallIcon( "source"),i18n("New Operation..."), mt_New_Operation);
			break;

		case mt_New_Attribute:
			insertItem(SmallIcon( "source"), i18n("New Attribute..."), mt_New_Attribute);
			break;

		case mt_New_Template:
			insertItem(SmallIcon("source"), i18n("New Template..."), mt_New_Template);
			break;

		case mt_New_Activity:
			m_pInsert = new KPopupMenu(this,"New");
			m_pInsert -> insertItem(SmallIcon( "source"),i18n("Activity..."), mt_New_Activity);
			insertItem(SmallIcon( "editcut"), i18n("New"), m_pInsert);
			break;

		case mt_Activity_Selected:
			m_pInsert = new KPopupMenu(this,"New");
			m_pInsert -> insertItem(SmallIcon( "source"),i18n("Activity..."), mt_New_Activity);
			insertItem(SmallIcon( "filenew"), i18n("New"), m_pInsert);
			insertItem(SmallIcon( "charset"), i18n("Rename..."), mt_Rename);
			insertItem(SmallIcon( "editdelete"), i18n("Delete"), mt_Delete);
			break;

		case mt_Parameter_Selected:
			insertItem(SmallIcon( "source"),i18n("New Parameter..."), mt_New_Parameter);
			insertItem(SmallIcon( "charset"), i18n("Rename..."), mt_Rename);
			insertItem(SmallIcon( "editdelete"), i18n("Delete"), mt_Delete);
			insertItem(SmallIcon( "info"), i18n("Properties"), mt_Properties);
			break;

		case mt_Operation_Selected:
			insertItem(SmallIcon( "source"),i18n("New Operation..."), mt_New_Operation);
			insertItem(SmallIcon( "charset"), i18n("Rename..."), mt_Rename);
			insertItem(SmallIcon( "editdelete"), i18n("Delete"), mt_Delete);
			insertItem(SmallIcon( "info"), i18n("Properties"), mt_Properties);
			break;

		case mt_Attribute_Selected:
			insertItem(SmallIcon( "source"),i18n("New Attribute..."), mt_New_Attribute);
			insertItem(SmallIcon( "charset"), i18n("Rename..."), mt_Rename);
			insertItem(SmallIcon( "editdelete"), i18n("Delete"), mt_Delete);
			insertItem(SmallIcon( "info"), i18n("Properties"), mt_Properties);
			break;

		case mt_Template_Selected:
			insertItem(SmallIcon("source"),i18n("New Template..."), mt_New_Attribute);
			insertItem(SmallIcon("charset"), i18n("Rename..."), mt_Rename);
			insertItem(SmallIcon("editdelete"), i18n("Delete"), mt_Delete);
			insertItem(SmallIcon("info"), i18n("Properties"), mt_Properties);
			break;

		case mt_Association_Selected:
			insertItem(SmallIcon( "editdelete"), i18n("Delete"), mt_Delete);
			insertItem(SmallIcon( "info"), i18n("Properties"), mt_Properties);
			break;

		case mt_Anchor:
			insertItem(SmallIcon( "editdelete"),i18n("Delete Anchor"), mt_Delete);
			break;

		case mt_MultiB:
			insertItem(SmallIcon( "editdelete"),i18n("Delete Association"), mt_Delete_Association);
			insertItem(SmallIcon( "charset"),i18n("Change Multiplicity..."), mt_Rename_MultiB);
			insertItem(SmallIcon( "fonts"),i18n( "Change Font..." ), mt_Change_Font );
			insertItem(i18n("Reset Label Positions"), mt_Reset_Label_Positions);
			insertItem(SmallIcon( "info"),i18n("Properties"), mt_Properties);
			break;

		case mt_MultiA:
			insertItem(SmallIcon( "editdelete"),i18n("Delete Association"), mt_Delete_Association);
			insertItem(SmallIcon( "charset"),i18n("Change Multiplicity..."), mt_Rename_MultiA);
			insertItem(SmallIcon( "fonts"),i18n( "Change Font..." ), mt_Change_Font );
			insertItem(i18n("Reset Label Positions"), mt_Reset_Label_Positions);
  			insertItem(SmallIcon( "info"),i18n("Properties..."), mt_Properties);
  			break;

 		case mt_Name:
 			insertItem(SmallIcon( "editdelete"),i18n("Delete Association"), mt_Delete_Association);
 			insertItem(SmallIcon( "charset"), i18n("Change Name"), mt_Rename_Name);
 			insertItem(SmallIcon( "fonts"), i18n( "Change Font" ), mt_Change_Font );
			insertItem(i18n("Reset Label Positions"), mt_Reset_Label_Positions);
 			insertItem(SmallIcon( "info"),i18n("Properties..."), mt_Properties);
 			break;

 		case mt_RoleName:
  			insertItem(SmallIcon( "editdelete"),i18n("Delete Association"), mt_Delete_Association);
 			insertItem(SmallIcon( "charset"), i18n("Change association name"), mt_Rename_Name);
 			insertItem(SmallIcon( "charset"), i18n("Change role A name"), mt_Rename_RoleAName);
 			insertItem(SmallIcon( "charset"), i18n("Change role B name"), mt_Rename_RoleBName);
  			insertItem(SmallIcon( "fonts"), i18n( "Change Font" ), mt_Change_Font );
			insertItem(i18n("Reset Label Positions"), mt_Reset_Label_Positions);
  			insertItem(SmallIcon( "info"),i18n("Properties..."), mt_Properties);
  			break;

		case mt_Collaboration_Message:
//			insertItem(SmallIcon( "editcut"), i18n("Cut"), mt_Cut);
//			insertItem(SmallIcon( "editcopy"), i18n("Copy"), mt_Copy);
//			insertItem(SmallIcon( "editpaste"), i18n("Paste"), mt_Paste);
//			insertSeparator();
			insertItem(SmallIcon( "editdelete"), i18n("Delete"), mt_Delete);
			insertItem(SmallIcon( "fonts"),  i18n( "Change Font..." ), mt_Change_Font );
			insertItem(SmallIcon( "filenew"), i18n("New Operation..."), mt_Operation);
			insertItem(SmallIcon( "charset"), i18n("Select Operation..."), mt_Select_Operation);
			insertItem(SmallIcon( "charset"), i18n("Set Sequence Number..."), mt_Sequence_Number);
			break;
		default:
			insertItem(i18n("Expand All"),mt_Expand_All);
			insertItem(i18n("Collapse All"),mt_Collapse_All);
			break;
	}//end switch

	if( view ) {
		bool bCutState = view -> getDocument() -> getCutCopyState();
		setItemEnabled( mt_Undo, view->getUMLApp()->getUndoEnabled() );
		setItemEnabled( mt_Redo, view->getUMLApp()->getRedoEnabled() );
		setItemEnabled( mt_Cut, bCutState );
		setItemEnabled( mt_Copy, bCutState );
		setItemEnabled( mt_Paste, view -> getDocument() -> getPasteState() );
	}
	if(m_pInsert)
		connect(m_pInsert, SIGNAL(activated(int)), this, SIGNAL(activated(int)));
	if(m_pShow)
		connect(m_pShow, SIGNAL(activated(int)), this, SIGNAL(activated(int)));
	if(m_pColor)
		connect(m_pColor, SIGNAL(activated(int)), this, SIGNAL(activated(int)));
}

void ListPopupMenu::setupDiagramMenu(UMLView* view) {
	insertItem(SmallIcon("undo"), i18n("Undo"), mt_Undo);
	insertItem(SmallIcon("redo"), i18n("Redo"), mt_Redo);
	insertSeparator();
	insertItem(SmallIcon("editcut"), i18n("Cut"), mt_Cut);
	insertItem(SmallIcon("editcopy"), i18n("Copy"), mt_Copy);
	insertItem(SmallIcon("editpaste"), i18n("Paste"), mt_Paste);
	insertSeparator();
	insertItem(SmallIcon("editclear"), i18n("Clear Diagram"), mt_Clear);
	insertItem(i18n("Snap to Grid"), mt_SnapToGrid);
	setItemChecked(mt_SnapToGrid, view->getSnapToGrid() );
	insertItem(i18n("Show Grid"), mt_ShowSnapGrid );
	setItemChecked(mt_ShowSnapGrid, view->getShowSnapGrid() );
	insertItem(SmallIcon("image"), i18n("Export as Picture..."), mt_Export_Image);
	insertItem(SmallIcon("info"), i18n("Properties"), mt_Properties);
}
