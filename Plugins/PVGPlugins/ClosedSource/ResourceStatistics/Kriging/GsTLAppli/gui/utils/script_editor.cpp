/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "gui" module of the Geostatistical Earth
** Modeling Software (GEMS)
**
** This file may be distributed and/or modified under the terms of the 
** license defined by the Stanford Center for Reservoir Forecasting and 
** appearing in the file LICENSE.XFREE included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.gnu.org/copyleft/gpl.html for GPL licensing information.
**
** Contact the Stanford Center for Reservoir Forecasting, Stanford University
** if any conditions of this licensing are not clear to you.
**
**********************************************************************/
//#ifdef _DEBUG
//#undef _DEBUG
//#include <Python.h>
//#define _DEBUG
//#else
//#include <Python.h>
//#endif
#include <Python.h>

#include <GsTLAppli/gui/utils/script_editor.h>
#include <GsTLAppli/gui/utils/script_syntax_highlighter.h>
#include <GsTLAppli/actions/python_wrapper.h>
#include <GsTLAppli/actions/python_commands.h>
#include <GsTLAppli/appli/project.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/gui/utils/qtbased_scribes.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/grid_property.h>
#include <GsTLAppli/appli/manager_repository.h>

#include <qtextedit.h>
#include <qsplitter.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qframe.h>
#include <qpixmap.h>
#include <qaction.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qtoolbar.h>
#include <qstatusbar.h>
#include <qfiledialog.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <qcursor.h>
#include <qspinbox.h>
#include <qinputdialog.h>




//===========================================
//  Some embedded images

static const char* const image0_data[] = {
"22 22 7 1",
". c None",
"# c #000000",
"b c #2e2e2e",
"c c #5c5c5c",
"d c #878787",
"e c #c2c2c2",
"a c #ffffff",
"......................",
"....##########........",
"....#aaaaaaa#b#.......",
"....#aaaaaaa#cb#......",
"....#aaaaaaa#dcb#.....",
"....#aaaaaaa#edcb#....",
"....#aaaaaaa#aedcb#...",
"....#aaaaaaa#######...",
"....#aaaaaaaaaaaaa#...",
"....#aaaaaaaaaaaaa#...",
"....#aaaaaaaaaaaaa#...",
"....#aaaaaaaaaaaaa#...",
"....#aaaaaaaaaaaaa#...",
"....#aaaaaaaaaaaaa#...",
"....#aaaaaaaaaaaaa#...",
"....#aaaaaaaaaaaaa#...",
"....#aaaaaaaaaaaaa#...",
"....#aaaaaaaaaaaaa#...",
"....#aaaaaaaaaaaaa#...",
"....###############...",
"......................",
"......................"};

static const char* const image1_data[] = {
"22 22 5 1", 
". c None",
"# c #000000",
"c c #848200",
"a c #ffff00",
"b c #ffffff",
"......................",
"......................",
"......................",
"............####....#.",
"...........#....##.##.",
"..................###.",
".................####.",
".####...........#####.",
"#abab##########.......",
"#babababababab#.......",
"#ababababababa#.......",
"#babababababab#.......",
"#ababab###############",
"#babab##cccccccccccc##",
"#abab##cccccccccccc##.",
"#bab##cccccccccccc##..",
"#ab##cccccccccccc##...",
"#b##cccccccccccc##....",
"###cccccccccccc##.....",
"##cccccccccccc##......",
"###############.......",
"......................"};


static const char* const image2_data[] = {
"22 22 5 1",
". c None",
"# c #000000",
"a c #848200",
"b c #c1c1c1",
"c c #cab5d1",
"......................",
".####################.",
".#aa#bbbbbbbbbbbb#bb#.",
".#aa#bbbbbbbbbbbb#bb#.",
".#aa#bbbbbbbbbcbb####.",
".#aa#bbbccbbbbbbb#aa#.",
".#aa#bbbccbbbbbbb#aa#.",
".#aa#bbbbbbbbbbbb#aa#.",
".#aa#bbbbbbbbbbbb#aa#.",
".#aa#bbbbbbbbbbbb#aa#.",
".#aa#bbbbbbbbbbbb#aa#.",
".#aaa############aaa#.",
".#aaaaaaaaaaaaaaaaaa#.",
".#aaaaaaaaaaaaaaaaaa#.",
".#aaa#############aa#.",
".#aaa#########bbb#aa#.",
".#aaa#########bbb#aa#.",
".#aaa#########bbb#aa#.",
".#aaa#########bbb#aa#.",
".#aaa#########bbb#aa#.",
"..##################..",
"......................"};


static const char* const image4_data[] = {
"22 22 3 1",
". c None",
"# c #000084",
"a c #848284",
"......................",
"......................",
"......................",
"......................",
"......................",
"...........######a....",
"..#......##########...",
"..##...####......##a..",
"..###.###.........##..",
"..######..........##..",
"..#####...........##..",
"..######..........##..",
"..#######.........##..",
"..########.......##a..",
"...............a###...",
"...............###....",
"......................",
"......................",
"......................",
"......................",
"......................",
"......................"};

static const char* const image5_data[] = {
"22 22 3 1",
". c None",
"a c #000084",
"# c #848284",
"......................",
"......................",
"......................",
"......................",
"......................",
"....#aaaaaa...........",
"...aaaaaaaaaa......a..",
"..#aa......aaaa...aa..",
"..aa.........aaa.aaa..",
"..aa..........aaaaaa..",
"..aa...........aaaaa..",
"..aa..........aaaaaa..",
"..aa.........aaaaaaa..",
"..#aa.......aaaaaaaa..",
"...aaa#...............",
"....aaa...............",
"......................",
"......................",
"......................",
"......................",
"......................",
"......................"};


static const char* const image6_data[] = {
"22 22 3 1",
". c None",
"# c #000000",
"a c #000082",
"......................",
".......#.....#........",
".......#.....#........",
".......#.....#........",
".......#....##........",
".......##...#.........",
"........#...#.........",
"........##.##.........",
".........###..........",
".........###..........",
"..........#...........",
".........a#a..........",
"........aa.aaa........",
".......a.a.a..a.......",
"......a..a.a...a......",
".....a...a.a....a.....",
"....a....a.a....a.....",
"....a....a..a...a.....",
"....a....a..a..a......",
"....a...a....aa.......",
".....aaa..............",
"......................"};

static const char* const image7_data[] = {
"22 22 6 1",
". c None",
"# c #000000",
"b c #000082",
"c c #3c3cfd",
"d c #8b8bfd",
"a c #ffffff",
"......................",
"......................",
"########..............",
"#aaaaaa##.............",
"#a####a#a#............",
"#aaaaaa#aa#...........",
"#a####a#bbbbbbbb......",
"#aaaaaa#baaaaaabb.....",
"#a#####aba####abcb....",
"#aaaaaaabaaaaaabdcb...",
"#a#####aba####abadcb..",
"#aaaaaaabaaaaaabbbbbb.",
"#a#####aba####aaaaaab.",
"#aaaaaaabaaaaaaaaaaab.",
"#a#####aba#########ab.",
"#aaaaaaabaaaaaaaaaaab.",
"########ba#########ab.",
"........baaaaaaaaaaab.",
"........ba#########ab.",
"........baaaaaaaaaaab.",
"........bbbbbbbbbbbbb.",
"......................"};

static const char* const image8_data[] = {
"22 22 8 1",
". c None",
"# c #000000",
"e c #000084",
"c c #848200",
"b c #848284",
"d c #c6c3c6",
"a c #ffff00",
"f c #ffffff",
"......................",
".......#####..........",
"..######aaa######.....",
".######aaaaa######....",
"##bcb##a###a##bcb##...",
"#bcb#ddddddddd#bcb#...",
"#cbc#ddddddddd#cbc#...",
"#bcb###########bcb#...",
"#cbcbcbcbcbcbcbcbc#...",
"#bcbcbcbcbcbcbcbcb#...",
"#cbcbcbceeeeeeeeee#...",
"#bcbcbcbefffffffefe...",
"#cbcbcbcefeeeeefeffe..",
"#bcbcbcbefffffffefffe.",
"#cbcbcbcefeeeeefeffffe",
"#bcbcbcbefffffffeeeeee",
"#cbcbcbcefeeeeeffffffe",
"#bcbcbcbeffffffffffffe",
"#cbcbcbcefeeeeeeeeeefe",
".#######effffffffffffe",
"........eeeeeeeeeeeeee",
"......................"};



//========================================================


Script_editor::Script_editor( GsTL_project* project, 
                              QWidget* parent, const char* name )
: QMainWindow( parent, name, Qt::WType_TopLevel | Qt::WDestructiveClose ),
  project_( 0 ) { 
  
  setCaption( "S-GeMS Script Editor" ); 

  project_ = project;
  filename_ = "";
  accept_close_ = true;

  //-----------------------------

  QFrame* main_widget = new QFrame( this );
  QVBoxLayout* main_layout = new QVBoxLayout( main_widget );
  
  QSplitter* split = new QSplitter( Qt::Vertical, main_widget, "splitter" );

  script_in_ = new QTextEdit( split, "IN" );
  script_in_->setFont( QFont( "Courier", 10, QFont::Normal ) );
  script_in_->setTabStopWidth( 40 );
  script_in_->setUndoRedoEnabled( true );

  
  QFrame* bottom_frame = new QFrame( split );
  QVBoxLayout* bottom_layout = new QVBoxLayout( bottom_frame );
  script_out_ = new QTextEdit( bottom_frame, "OUT" );
  script_out_->setReadOnly( true );
  QLabel* label = new QLabel( "Script Output Messages", bottom_frame );
  bottom_layout->addSpacing( 10 );
  bottom_layout->addWidget( label );
  bottom_layout->addWidget( script_out_ );

  QValueList<int> splitter_sizes;
  splitter_sizes << 400 << 200;
  split->setSizes( splitter_sizes );


  QPushButton* run_button = new QPushButton( "Run", main_widget );

  main_layout->addWidget( split );
  main_layout->addSpacing( 8 );
  main_layout->addWidget( run_button );

  QObject::connect( run_button, SIGNAL( clicked() ),
                    this, SLOT( run_script() ) );

  menu_setup();
  setCentralWidget( main_widget );
  statusBar()->message( "Ready", 2000 );
  resize( 450, 600 );

  new Script_syntax_highlighter( script_in_ );

  script_messages_scribe_ = 
    SmartPtr<QTextedit_scribe>( new QTextedit_scribe( script_out_ ) );
  script_messages_scribe_->subscribe( *GsTLAppli_Python_cout::instance() );
  script_errors_scribe_ = 
    SmartPtr<QTextedit_scribe>( new QTextedit_scribe( script_out_, "red" ) );
  script_errors_scribe_->subscribe( *GsTLAppli_Python_cerr::instance() );


  if( !project_ ) {
    run_button->hide();
    bottom_frame->hide();
  }

  //initialize Python project
  Python_project_wrapper::set_project( project_ );
} 



Script_editor::~Script_editor() {
  script_messages_scribe_->unsubscribe( *GsTLAppli_Python_cout::instance() );
  script_errors_scribe_->unsubscribe( *GsTLAppli_Python_cerr::instance() );

  appli_message( "script editor deleted" );
}



void Script_editor::new_script() {
  script_in_->clear();
  filename_ = "";
}

void Script_editor::load_script() {
  QString fn =
    QFileDialog::getOpenFileName( QString::null, "Python scripts (*.py);;All (*)", this );
  if( fn.isEmpty() ) {
    statusBar()->message( "Loading aborted", 2000 );
    return;
  }

  script_in_->clear();
  
  QFile file( fn );
  if ( !file.open( IO_ReadOnly ) ) return;

  filename_ = fn;
  QTextStream stream( &file );
  script_in_->setText( stream.read() );
  
  setCaption( filename_ );
  QString s;
  s.sprintf( "Loaded document %s", filename_ );
  statusBar()->message( s, 2000 );
}


void Script_editor::save_script() {
  if ( filename_.isEmpty() ) {
    save_script_as();
    return;
  }

  QFile file( filename_ ); // Write the text to a file
  if( !file.open( IO_WriteOnly ) ) return;

  QTextStream stream( &file );
  stream << script_in_->text();
  script_in_->setModified( FALSE );
  
  setCaption( filename_ );
  statusBar()->message( QString( "File %1 saved" ).arg( filename_ ), 2000 );
}


void Script_editor::save_script_as() {
  QString fn = 
    QFileDialog::getSaveFileName( QString::null, QString::null, this );
  if ( !fn.isEmpty() ) {
    filename_ = fn;
    save_script();
  } 
  else {
    statusBar()->message( "Saving aborted", 2000 );
  }
}
  



// Runs a Python script
void Script_editor::run_script() {
  script_out_->clear();
  QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );

  PyRun_SimpleString(""
   "import redirect     # a module interface created by C application\n"
   "class CoutLogger:\n"
   "    def __init__(self):\n"
   "        self.buf = []\n"
   "    def write(self, data):\n"
   "        self.buf.append(data)\n"
   "        if data.endswith('\\n'):\n"
   "            redirect.sgems_cout(''.join(self.buf))\n"
   "            self.buf = []\n"
   "\n"
   "class CerrLogger:\n"
   "    def __init__(self):\n"
   "        self.buf = []\n"
   "    def write(self, data):\n"
   "        self.buf.append(data)\n"
   "        if data.endswith('\\n'):\n"
   "            redirect.sgems_cerr(''.join(self.buf))\n"
   "            self.buf = []\n"
   "\n"
   "import sys\n"
   "sys.stdout = CoutLogger()\n"
   "sys.stderr = CerrLogger()\n"
   "");
  QString q_script_text = script_in_->text();
  q_script_text += "\n";
  std::string script_text( q_script_text.ascii() );
   
  PyRun_SimpleString( (char*) script_text.c_str() );

  if( Python_project_wrapper::is_project_modified() ) {
    Python_project_wrapper::project()->update();
  }

  QApplication::restoreOverrideCursor();

}

  
void Script_editor::close_slot() {
  this->close( true );
}

void Script_editor::closeEvent( QCloseEvent* e ) {

  if( script_in_->isModified() ) {
    switch( QMessageBox::information( this, "S-GeMS", 
                  "The current script has been modified since last saved\n" 
                  "Do you want to save the changes before exiting?",
                  "&Save", "&Don't Save", "Cancel", 0, 2 ) ) {
    case 0:
      //save the changes
      save_script();
      break;

    case 1:
      // don't save, and exit
      break;

    case 2:
      // don't exit
      accept_close_ = false;
      return;
    }
  }
  
  if( accept_close_ ) 
    e->accept();
  else {
    accept_close_ = true;
    e->ignore();
  }
}



QString Script_editor::find_tmp_filename( const QString& filename ) {
  QString tmp_base_filename = filename + ".sgems_tmp";
  QFile file( tmp_base_filename );
  if( !file.exists() ) return tmp_base_filename ;

  int id = 0;
  do {
    QString id_str;
    id_str.setNum( id );
    file.setName( tmp_base_filename + id_str );
    ++id;
  }
  while( file.exists() );

  return file.name();
}


void Script_editor::go_to_line() {
  bool ok ;
  int res = QInputDialog::getInteger("GotoLine", "Enter the line to go:",
                                     0,0, script_in_->paragraphs(), 1,&ok,this);

  if(ok) {
    script_in_->setCursorPosition(res-1,0) ;
  }
}


void Script_editor::set_point_size( int s ) {
  script_in_->selectAll(true);
  script_in_->setPointSize( s );
  script_in_->selectAll(false);
  script_in_->setPointSize( s );
}


void Script_editor::menu_setup() {

  QAction *fileNewAction, *fileOpenAction, *fileSaveAction, *fileSaveAsAction, *fileQuitAction,
    *editUndoAction, *editRedoAction, *editCutAction, *editCopyAction, *editPasteAction,
    *editGoToLine, *runScriptAction;


  //-------------------
  // file actions

  fileNewAction = new QAction( "New Script",
                                QPixmap( (const char **) image0_data ), 
                                "&New", CTRL+Key_N, this, "new" );
  connect( fileNewAction, SIGNAL( activated() ) , this, SLOT( new_script() ) );

  fileOpenAction = new QAction( "Open File",
                                QPixmap( (const char **) image1_data ), 
                                "&Open", CTRL+Key_O, this, "open" );
  connect( fileOpenAction, SIGNAL( activated() ) , this, SLOT( load_script() ) );

  
  fileSaveAction = new QAction( "Save File", QPixmap( (const char **) image2_data ),
                                "&Save", CTRL+Key_S, this, "save" );
  connect( fileSaveAction, SIGNAL( activated() ) , this, SLOT( save_script() ) );
  
  fileSaveAsAction = new QAction( "Save File As", "Save &As...", 0,
                                  this, "save as" );
  connect( fileSaveAsAction, SIGNAL( activated() ) , this, SLOT( save_script_as() ) );

  
  fileQuitAction = new QAction( "Close Editor", "&Close", CTRL+Key_Q, this, "quit" );
  connect( fileQuitAction, SIGNAL( activated() ) , 
           this, SLOT( close_slot() ) );


  //-------------------
  // edit actions

  editUndoAction = new QAction( "Undo", QPixmap( (const char **) image4_data ),
                                "Undo", CTRL+Key_Z, this, "Undo" );
  connect( editUndoAction, SIGNAL( activated() ) , script_in_, SLOT( undo() ) );

  editRedoAction = new QAction( "Redo", QPixmap( (const char **) image5_data ),
                                "Redo", CTRL+Key_R, this, "redo" );
  connect( editRedoAction, SIGNAL( activated() ) , script_in_, SLOT( redo() ) );

  editCutAction = new QAction( "Cut", QPixmap( (const char **) image6_data ),
                                "Cut", CTRL+Key_X, this, "cut" );
  connect( editCutAction, SIGNAL( activated() ) , script_in_, SLOT( cut() ) );

  editCopyAction = new QAction( "Copy", QPixmap( (const char **) image7_data ),
                                "Copy", CTRL+Key_C, this, "copy" );
  connect( editCopyAction, SIGNAL( activated() ) , script_in_, SLOT( copy() ) );

  editPasteAction = new QAction( "Paste", QPixmap( (const char **) image8_data ),
                                "Paste", CTRL+Key_V, this, "Paste" );
  connect( editPasteAction, SIGNAL( activated() ) , script_in_, SLOT( paste() ) );

  editGoToLine =  new QAction( "Go To Line...", 
                                "Go To Line...", CTRL+Key_G, this, "GoToLine" );
  connect( editGoToLine, SIGNAL( activated() ) , this, SLOT( go_to_line() ) );



  //-------------------
  runScriptAction = new QAction( "Run", //QPixmap( (const char **) image4_data ),
                                "Run", Key_F5, this, "Run" );
  connect( runScriptAction, SIGNAL( activated() ) , this, SLOT( run_script() ) );


  //-------------------

  QToolBar* fileTools = new QToolBar( this, "file operations" );
  fileTools->setLabel( tr( "File Operations" ) );
  fileNewAction->addTo( fileTools );
  fileOpenAction->addTo( fileTools );
  fileSaveAction->addTo( fileTools );

  QToolBar* editTools = new QToolBar( this, "Edit operations" );
  editTools->setLabel( tr( "Edit Operations" ) );
  editUndoAction->addTo( editTools );
  editRedoAction->addTo( editTools );
  editCutAction->addTo( editTools );
  editCopyAction->addTo( editTools );
  editPasteAction->addTo( editTools );
//  QSpinBox* point_size_box = new QSpinBox( 4, 24, 1, editTools );
//  point_size_box->setValue( script_in_->pointSize() );
//  connect( point_size_box, SIGNAL( valueChanged(int) ), 
//           this, SLOT( set_point_size(int) ) );

  //-------------------

  QPopupMenu * file = new QPopupMenu( this );
  file-> insertTearOffHandle() ;
  menuBar()->insertItem( "&File", file );
  fileNewAction->addTo( file );
  fileOpenAction->addTo( file );
  file->insertSeparator();
  fileSaveAction->addTo( file );
  fileSaveAsAction->addTo( file );
  file->insertSeparator();
  fileQuitAction->addTo( file );
  
  QPopupMenu * edit = new QPopupMenu( this );
  edit-> insertTearOffHandle() ;
  menuBar()->insertItem( "&Edit", edit );
  editUndoAction->addTo( edit );
  editRedoAction->addTo( edit );
  file->insertSeparator();
  editCutAction->addTo( edit );
  editCopyAction->addTo( edit );
  editPasteAction->addTo( edit );
  editGoToLine->addTo( edit );

  QPopupMenu * run = new QPopupMenu( this );
  run->insertTearOffHandle() ;
  menuBar()->insertItem( "&Script", run );
  runScriptAction->addTo( run );

}

