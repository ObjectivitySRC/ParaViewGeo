/*=========================================================================
MIRARCO MINING INNOVATION
Author: Chris Cameron (ccameron@mirarco.org)
===========================================================================*/

#ifndef _pqDrillholeGA_h
#define _pqDrillholeGA_h

/** 
    \file pqDrillholeGA.h
    \brief Header for pqDrillholeGA.
    \see vtkDrillholeGA, pqDrillholeGA
*/

#include "pqLoadedFormObjectPanel.h"
#include "pqComponentsExport.h"

class QComboBox;
class QLineEdit;
class QCheckBox;
class QDoubleSpinBox;
class QPushButton;
class QString;

/**
  \class pqDrillholeGA
  \brief pqDrillholeGA represents all functionality related to the custom UI panel that vtkDrillholeGA makes use of.
  \see vtkDrillholeGA

*/
class pqDrillholeGA : public pqLoadedFormObjectPanel {
  Q_OBJECT
public:

  /** 
    A quick tutorial on how to hackishly send data to/from the server
    This is the way I do it (adapted from Nehme, Arolde, Rob...).

    If you want to send data FROM the server TO the client during RequestInformation:
    -# Make an information_only property in your xml file, do not make it a widget in your custom ui
    -# During RequestInformation set the value of that property.
    -# In your custom UI's constructor, retrieve the property using (e.g.)
       vtkSMStringVectorProperty* Some_Property = vtkSMStringVectorProperty::SafeDownCast(
	     this->proxy()->GetProperty("Name_Of_The_Property"));
    -# For example, if it is a string, you then do this:
       QString text(Some_Property->GetElement(0));

    If you want to send data TO the server FROM the client when the user clicks Apply
    -# Make a regular property in your xml file, and make a corresponding widget in your custom ui
    -# In your custom ui's constructor, obtain the widget (this->findChild<WidgetType*>("Widget_Name");
    -# In your custom ui's constructor, hide the widget (setVisible(false))
      -# If you only need to set the info once, set the widget's value now
      -# If you need to change the info, set up a signal-slot connection so that the widget's value
        is updated as necessary on the client side. The example of this file is everytime I choose a
        value in a QComboBox, the text in active QComboBox item is placed in a QLineEdit

    Note in my example that you don't have to bother implementing accept(), reset(), linkServerManagerProperties() or 
    any other virtual functions that you aren't going to ACTUALLY re-implement. 
  */
  pqDrillholeGA(pqProxy* proxy, QWidget* p = NULL);

  /**
    Overridden. Before calling <code>pqLoadedFormObjectPanel::accept()</code>, unchecks the <code>RefreshPlease</code> check box. This way the server knows the difference between pressing Apply and pressing Refresh.
  */
  virtual void accept();
protected slots:

  /**
    Whenever the <code>CollarLocation</code> <code>QComboBox</code> is modified, the <code>CollarFile</code>'s enabled status is toggled. 
    
    If <code>index</code> is 0, then <code>CollarFile</code> is enabled. 
    Otherwise <code>CollarFile</code> is disabled.

    \param index The new index in the <code>QComboBox</code>.
  */
  void CollarLocationChanged(const int);

  /**
    Deals with preparation and updating of the <code>ResumeFrom</code> <code>QComboBox</code>.

    Calls <code>FillComboBox</code>

    This takes a <code>bool</code> parameter in case you want to receive the <code>boolean</code> 
    sent by the <code>toggled(bool)</code> signal.

  */
  void OnRefreshButtonClicked(bool);

private:
  /** 
    Retrieves a list of items from the server side and adds them to the <code>ResumeFrom</code> <code>QComboBox</code>.

    \return The first item in the <code>ResumeFrom</code>, or an empty <code>QString</code> if <code>ResumeFrom</code> is empty.
  */
  QString FillComboBox();

  /**
    <code>QComboBox</code> that contains available options for where to find the input set of collar points.
  */
  QComboBox* CollarLocation;

  /**
    <code>QComboBox</code> that contains available options for resuming from previous elite lists.
  */
  QComboBox* ResumeFrom;

  /**
    This is a hidden <code>QLineEdit</code> used to hackishly send (from client to server) which elite list will be used to resume.
  */
  QLineEdit* ResumeFromC2S;
  /**
    This is a hidden <code>QCheckBox</code> used to indicate whether a previous run should be resumed from.
  */
  QCheckBox* ResumePreviousRun;
  
  /**
    This <code>QCheckBox</code> sets whether hill climbing should be performed on drillhole length.
  */
  QCheckBox* HCLength;
  /**
    This <code>QCheckBox</code> sets whether hill climbing should be performed on drillhole direction (az, dip).
  */
  QCheckBox* HCDirection;
  /**
    This sets by what amount hill climbing should modify the length of a drillhole.
  */
  QDoubleSpinBox* HCLengthAmount;
  /**
    This sets by what amount hill climbing should modify the azimuth of a drillhole.
  */
  QDoubleSpinBox* HCDirectionAmountAz;
  /**
    This sets by what amount hill climbing should modify the dip of a drillhole.
  */
  QDoubleSpinBox* HCDirectionAmountDip;

  /**
    This hidden <code>QCheckBox</code> is used to hackishly send (from client to server) that the <code>ResumeFrom</code>
    <code>QComboBox</code> should be updated, instead of running the code in <code>vtkDrillholeGA::RequestData()</code>
  */
  QCheckBox* RefreshPlease;

  /**
    This is the <code>QPushButton</code> that can trigger a refresh of the <code>ResumeFrom</code> <code>QComboBox</code>.
  */
  QPushButton* RefreshButton;
  

};

#endif

