#include "pqPlotPropertiesOverTime.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include <QColor>
#include <QPalette>
#include <QColor>
#include <QPainter>

#include <vtkSMProxy.h>

#include <pqOutputPort.h>
#include <pqPipelineSource.h>
#include <pqRepresentation.h>
#include <pqServer.h>

#include "pqChartWidget.h"
#include "pqChartAxis.h"
#include "pqLabelFormatter.h"


pqPlotPropertiesOverTime::pqPlotPropertiesOverTime(
	const QString& viewtypemodule, const QString& group, const QString& name,
         vtkSMViewProxy* viewmodule, pqServer* server, QObject* p)
     : pqPlotView(pqPlotView::barChartType(), group, name, viewmodule, server, p)
{
	pqChartWidget *w = qobject_cast<pqChartWidget*>(this->getWidget());
	w->setWindowTitle("Plot Properties Over Time");  

	pqLabelFormatter *formatter = new pqLabelFormatter();
	w->getChartArea()->getAxis(pqChartAxis::Bottom)->setFormatter(formatter);

	

}

pqPlotPropertiesOverTime::~pqPlotPropertiesOverTime()
{
}
 

