/*
   ParaView is a free software; you can redistribute it and/or modify it
   under the terms of the ParaView license version 1.2. 

   See License_v1.2.txt for the full ParaView license.
   A copy of this license can be obtained by contacting
   Kitware Inc.
   28 Corporate Drive
   Clifton Park, NY 12065
   USA

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*=========================================================================
MIRARCO MINING INNOVATION
Author: Nehme Bilal (nehmebilal@gmail.com)
===========================================================================*/

#ifndef _pqPlotPropertiesOverTime_h
#define _pqPlotPropertiesOverTime_h

#include "pqPlotView.h"
#include <QMap>
#include "pqChartArea.h"

class QLabel;
class pqOutputPort;
class pqPipelineSource;
class pqRepresentation;
class pqServer;
class pqUndoStack;
class pqViewInternal;
class QWidget;
class vtkImageData;
class vtkSMViewProxy;
class QColor;

 /// a simple view that shows a QLabel with the display's name in the view
 class pqPlotPropertiesOverTime : public pqPlotView
 {
  Q_OBJECT


 public:

  pqPlotPropertiesOverTime(
		const QString& viewtypemodule, 
		const QString& group, const QString& name,
         vtkSMViewProxy* viewmodule, pqServer* server, QObject* p);
     
  ~pqPlotPropertiesOverTime();

  
  bool saveImage(int width, int height, const QString& filename) 
	{ 
		return pqPlotView::saveImage(width,height,filename); 
	}

  vtkImageData* captureImage(int magnification) 
	{ 
		return pqPlotView::captureImage(magnification); 
	}
 
  /// return the QWidget to give to ParaView's view manager
	virtual QWidget* getWidget()
	{
		return pqPlotView::getWidget();
	}
 
  /// This method returns is any pqPipelineSource can be dislayed in this
  /// view. This is a convenience method, it gets
  /// the pqDisplayPolicy object from the pqApplicationCore
  /// are queries it.
  virtual bool canDisplay(pqOutputPort* opPort) const
	{
		return pqPlotView::canDisplay(opPort);
	}


 protected:

 
 };


#endif