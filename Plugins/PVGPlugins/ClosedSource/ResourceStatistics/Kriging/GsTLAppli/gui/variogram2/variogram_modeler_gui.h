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

#ifndef __GSTLAPPLI_GUI_VARIOGRAM_TOP_H__
#define __GSTLAPPLI_GUI_VARIOGRAM_TOP_H__


#include <GsTLAppli/math/discrete_function.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/point_set.h>
#include <GsTLAppli/grid/grid_model/rgrid.h>


#include <GsTLAppli/gui/variogram2/first_scr.h>
#include <GsTLAppli/gui/variogram2/pset_params.h>
#include <GsTLAppli/gui/variogram2/rgrid_params.h>
#include <GsTLAppli/gui/variogram2/variogram_modeling_screen.h>

#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/extra/qtplugins/selectors.h>
#include <GsTLAppli/appli/project.h>

#include <qapplication.h>
#include <qwizard.h>
#include <qerrormessage.h>
#include <qstringlist.h>


class First;
class Pset_params;
class Variogram_modeling_screen;
class Geostat_grid;
class QScrollView;


class Variogram_modeler_gui : public QWizard
{
    Q_OBJECT

  public:
    Variogram_modeler_gui( QWidget* parent = 0, const char* name = 0 );
    Variogram_modeler_gui( GsTL_project* project, 
                           QWidget* parent = 0, const char* name = 0 );

    ~Variogram_modeler_gui();


  public slots:
    void parameterChanged();
    void disable_next_button_focus();
    virtual void next();
    virtual void back();
    void message(const QString& text){}


  signals:
    void error_message(const QString& text);


  private:
    bool Error();
    bool compute_df_pset( std::vector<Discrete_function> &df,
			                    std::vector<GsTLVector<double> > &v,
                          std::vector< std::vector<int> >& pairs,
                          QStringList& parameters_list );
    
    bool compute_df_rgrid( std::vector<Discrete_function> &df,
			                     std::vector<GsTLVector<double> >&v,
                           std::vector< std::vector<int> >& pairs,
                           QStringList& parameters_list );

    void prompt_for_parameters();
    void recompute_variograms();
    void clear_variogram_modeling_screen();

  private:
    GsTL_project *project_;
    First_scr *f_;
    QScrollView* first_screen_scrollview_;

    Pset_params *pset_params_;
    RGrid_params *rgrid_params_;
    QScrollView* pset_params_screen_;
    QScrollView* rgrid_params_screen_;

    Variogram_modeling_screen *s_;

    Geostat_grid *grid_;
     
    Variogram_function_adaptor<Covariance<GsTLPoint> > *varg_;
    bool changed_;
    bool variograms_computed_;
};


#endif
