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

#include <GsTLAppli/gui/utils/script_syntax_highlighter.h>

#include <qregexp.h>
#include <qtextedit.h>

Script_syntax_highlighter::Script_syntax_highlighter( QTextEdit* edit )
: QSyntaxHighlighter( edit ) {
}


int Script_syntax_highlighter::highlightParagraph( const QString& text,
                                                   int endStateOfLastPara ) {
  int pos = 0 ;
  int current_point_size = textEdit()->pointSize();
  // keywords in bold blue  
  {
    pos = 0 ;
    QRegExp r("\\b("
              "while|"
              "do|"
              "for|"
              "in|"
              "len|"
              "print|"
              "range|"
              "def|"
              "if|"
              "elseif|"
              "else|"
              "try|"
              "catch|"
              "throw|"
              "switch|"
              "break|"
              "continue|"
              "return|"
              "del|"
              "dict|"
              "import|"
              "from|"
              "pass"
              ")\\b") ;
    r.setMinimal(true) ;
    while( (pos= r.search(text,pos)) != -1 ) {
      setFormat( pos, r.cap(1).length(), QFont( "courier", current_point_size ), blue) ;
      pos += r.matchedLength() ;
    }
  }

/*
  // variables in red
  {
    pos = 0 ;
    //QRegExp r("(\\b\\$[a-zA-Z][a-zA-Z0-9_]*\\b)") ;
    QRegExp r("(\\$[a-zA-Z][a-zA-Z0-9_]*)") ;

     while((pos= r.search(text,pos))!=-1) {
       setFormat( pos, r.cap(1).length(), QFont( "courier", 10 ), red );
       pos += r.matchedLength() ;
     }
  }


  //value of a variable in red
  {
    pos = 0 ;

    QRegExp r("(\\$\\([a-zA-Z][a-zA-Z0-9_]*\\))") ;
     while((pos= r.search(text,pos))!=-1) {
      setFormat( pos, r.cap(1).length(), QFont( "courier", 10 ), red) ;
      pos += r.matchedLength() ;
     }
  }
*/

/*
  // misc keywords in blue
  {
    pos = 0 ;
    QRegExp r("("
              "file.close|"
              "file.readln|"
              "file.writeln|"
              "file.write|"
              "file.open|"
              "file.eof|"
              "timer.new|"
              "timer.start|"
              "timer.stop|"
              "timer.release|"
              "timer.elapsed|"
              "math|"
              "tokenizer.sizeof|"
              "tokenizer.get_token|"
              "string.prepend|"
              "string.append|"
              "string.at|"
              "string.insert|"
              "string.replace|"
              "string.size|"
              "string.sub|"
              "string.erase|"
              "sgems"
              ")") ;
    r.setMinimal(true) ;
    while((pos= r.search(text,pos))!=-1) {
      setFormat( pos, r.cap(1).length(), QFont( "courier", 10 ), blue );
      pos += r.matchedLength() ;
    }
  }
*/
  // Quoted words in green
  {
    pos = 0 ;
    QRegExp r("\"([^\"]+)\"") ;
    while((pos= r.search(text,pos))!=-1) {
      setFormat( pos+1, r.cap(1).length(), QFont( "courier", current_point_size ), darkGreen );
      pos += r.matchedLength();
    }
  }

  // comments in gray
  {
    pos = 0 ;
    QRegExp r("(#)");
    while((pos= r.search(text,pos))!=-1) {
      setFormat( pos, text.length(), QFont( "courier", current_point_size ), gray );
      pos += r.matchedLength();
    }
  }


  return 0;
}

