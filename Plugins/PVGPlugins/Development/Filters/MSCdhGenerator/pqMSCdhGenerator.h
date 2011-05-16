#ifndef _PQ_MSC_DH_GENERATOR_H
#define _PQ_MSC_DH_GENERATOR_H

#include "pqLoadedFormObjectPanel.h"

class pqMSCdhGenerator : public pqLoadedFormObjectPanel
{
	Q_OBJECT
public:
	pqMSCdhGenerator(pqProxy* proxy, QWidget*p = NULL);
	virtual ~pqMSCdhGenerator();

  virtual void accept();
  virtual void reset();

protected:
  /// populate widgets with properties from the server manager
	virtual void linkServerManagerProperties();

protected slots:
	void onBrowseCollarFile();
	void onBrowseOutputFile();

private:

};


#endif