#include "pqLabelFormatter.h"
#include "pqChartValue.h"
#include "TimeConverter.h"

#include <QString>




//-----------------------------------------------------------------------------
pqLabelFormatter::pqLabelFormatter(QObject* _parent) :
  pqChartValueFormatter(_parent)
{
}


//-----------------------------------------------------------------------------
QString pqLabelFormatter::format(const pqChartValue& value,
    int precision, pqChartValue::NotationType type) const
{
	double val = value.getDoubleValue();

	TimeConverter timeC = TimeConverter("%d/%m/%Y");
	timeC.Parse(val, TimeConverter::MSDATE);
	char* date = timeC.GetISODate();

	return QString(date);
}