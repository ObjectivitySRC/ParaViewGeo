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

#ifndef _pqDOTReader_h
#define _pqDOTReader_h

#include "pqLoadedFormObjectPanel.h"
#include "pqComponentsExport.h"
#include "pqPickHelper.h"

#include <QMap>

class pqDOTWidgetInternal;

class QLineEdit;
class QPushButton;
class QTableWidget;

class pqDOTReaderPointWidget;
class QString;
class pqServer;
class pqPipelineSource;
class pqDataRepresentation;



class pqDOTReader : public pqLoadedFormObjectPanel  {
  Q_OBJECT
public:
  /// constructor
  pqDOTReader(pqProxy* proxy, QWidget* p = NULL);
  /// destructor
  ~pqDOTReader();

  virtual void accept();
  virtual void reset();


protected slots:
	void createPointsWidgets(const QString& points);
	void sendPointsToServer();

	// update the table when a widget is moved
	void onWidgetMoved(QString& group, double x, double y, double z);

	void pick(double x, double y, double z);

	void moveWidget(double x, double y, double z);

	void showWidgets(bool visible);

	void onActiveViewchanged(pqView* view);

	void addWidgetsRepresentations(pqPipelineSource* source, 
									pqDataRepresentation* repr, int srcOutputPort);

	void removeWidgetsRepresentations(pqPipelineSource* source, 
									pqDataRepresentation* repr, int srcOutputPort);

	void onAddWidget();
	void onRemoveWidget();

protected:
  /// populate widgets with properties from the server manager
  virtual void linkServerManagerProperties();


	QLineEdit *FromServer;
	QLineEdit *ToServer;
	QTableWidget *table;
	pqServer *activeServer;

	pqPickHelper pickHelper;
	QShortcut *pickShortcut;
	pqPickHelper moveHelper;
	QShortcut *moveShortcut;

	QPushButton *AddWidget;
	QPushButton *RemoveWidget;

private:
	QMap< QString, pqDOTWidgetInternal* > Widgets;
	pqDOTWidgetInternal* target;

};

#endif

