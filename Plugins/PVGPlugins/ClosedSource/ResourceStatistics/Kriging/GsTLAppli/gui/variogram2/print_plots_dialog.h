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

#ifndef __GSTLAPPLI_VARIOGRAM_PRINT_PLOTS_DIALOG_H__
#define __GSTLAPPLI_VARIOGRAM_PRINT_PLOTS_DIALOG_H__



#include <GsTLAppli/gui/variogram2/variogram_print_plots_dialog.h>
#include <GsTLAppli/extra/qwt/qwt_plot.h>

#include <vector>

class FileChooser;


class PrintPlotsDialog : public PrintPlotsDialogBase{
  Q_OBJECT

public:
  PrintPlotsDialog( const std::vector< QwtPlot* >& plot_widgets, 
                    QWidget* parent = 0, const char* name = 0, 
                    bool modal = false );
protected slots:
  virtual void accept();

private:
  std::vector<QwtPlot*> plots_;
  FileChooser* filechooser_;

  void save_one_plot( QwtPlot* plot, const QString& filename,
                      const QString& format, bool show_grid ) ;

};



#endif
