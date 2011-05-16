#ifndef __vtkDrillholeGA_h
#define __vtkDrillholeGA_h

/** \file vtkDrillholeGA.h
    \brief Header for the vtkDrillholeGA class. Handles input, output, and translating between GA parameters and vtk parameters

    \todo Extract all of the GA-related things and put them in GA.h/GA.cxx instead.
*/

#include "vtkDataSetAlgorithm.h"
#include "vtkPoints.h"

#include <vector>
#include <map>

/**
  \var NUM_GENES
  \brief This is used to set how many components any <code>Drillhole</code> is made up of.
*/
const int NUM_GENES = 6;


//BTX
class Population;
class Individual;
class Drillhole;
/**
  \typedef DoubleVector
  \brief Convenience typedef.
*/
typedef std::vector<double> DoubleVector;

/**
  \typedef DrillholeVector
  \brief Convenience typedef.
*/
typedef std::vector<Drillhole> DrillholeVector;

/**
  \typedef IndividualVector
  \brief Convenience typedef.
*/
typedef std::vector<Individual> IndividualVector;
//ETX

class vtkPointLocator;
class vtkDataSet;
class vtkPolyData;
class vtkIntArray;
class vtkSQLiteDatabase;


/**
  \class vtkDrillholeGA
  \brief vtkDrillholeGA encapsulates the input, output, and translation between vtk and GA parameters.

  vtkDrillholeGA is the culmination of research into drillhole placement optimization. It is misleading
  to call it drillhole GA since it is actually the combination of two search algorithms: a Genetic Algorithm
  (GA) and Hill Climbing (HC). We use the GA to perform global optimization, and then follow it up rounds
  of HC to perform local optimization. 

  To run this filter, you must have at least one input and possibly one source. 
  
  - The input must be any dataset that contains points. These points represent a volume of space that the drillholes should attempt to optimize exploration of. 
  - The source (if available) must be any dataset that contains points. These points represent potential locations for drillhole collar points.

  If the source is not specified, then drillhole collar points must come from a CSV file (this is specified in the Custom UI Panel). 

  To save results of a "run" of this filter, the user can specify a MIRARCO Drillhole Database file (.dhd).

  To resume results of a previous "run" of the filter, the user can click "Resume Previous Run"  and then 
  choose the appropriate run from the combo box. The refresh button must be pressed in order to fill the box with items.

  Any one drillhole is defined by its six components: X, Y, Z (the collar point), Azimuth, Dip, and Length.

  Constraints are specified by indicating the minimum and maximum values for X, Y, Z, Azimuth, Dip, and Length
  - The constraints for X, Y, and Z are currently ignored since the user can only specify a set of collar points, and not a range within which they can be optimized. 

  To define a neighbourhood in which the drillholes should be searching around, we use the search ellipsoid
  concept from Kriging. The angles are specified just as they would be in Geostatistics (e.g., in SGeMS), and not
  in standard math angles. 

  The rest of the parameters are for configuring the GA and HC.

  Genetic Algorithm

  - Number of Generations (<code>NumGenerations</code>)
    - This is the number of generations that the GA will run for (per pass)
  - Number of Hill Climbing Passes (<code>NumPasses</code>)
    - This is the number of times HC should run. Each time the GA finishes, HC is operated. On the next pass
      the GA will run again for the number of generations specified, followed by HC again.
      If HC is disabled (see below) then the GA will simply run several times without HC
  - Population Size (<code>PopulationSize</code>)
    - This is the number of individuals that will be optimized at a time. An individual is a group of drillholes.
  - Individual size (<code>IndividualSize</code>)
    - This is the number of drillholes that make up an individual. 
  - Probability of Mutation (<code>ProbMutation</code>)
    - This is the probability that during any generation of the GA, whether an individual will be mutated.
  - Probability of Gene Mutation (<code>ProbGeneMutation</code>)
    - This is the probability that during any mutation of an individual, whether a drillhole gene (x,y,z,...) will be mutated. 
  - Drillhole Segment Length (<code>SegmentLength</code>)
    - This is the space between points when generating a drillhole line in space. We only evaluate the ellipsoid at each point, not
      along the entire drillhole. Make this length smaller for a more accurate search, but a good size is roughly the smallest of
      the ellipsoid magnitudes.
  
  Hill Climbing

  - On Length of Drillhole (<code>DoHillClimbLength</code>, <code>HillClimbLength</code>)
    - The check box enables Length-based HC
    - The length specifies how much a drillhole should be lengthened or shortened. Longer length is faster, but less granular
  - On Direction of Drillhole (<code>DoHillClimbDirection</code>, <code>HillClimbAzimuthWindow</code>, <code>HillClimbDipWindow</code>)
    - The check box enables Direction-based HC (azimuth, dip)
    - The azimuth specifies how much a drillhole should be turned (in degrees). 
    - The dip specifies how much a drillhole should be raised or lowered (in degrees). 
*/
class VTK_EXPORT vtkDrillholeGA : public vtkDataSetAlgorithm
{
public:
  
  /**
    This function is used by the <code>pqDrillholeGA</code> custom panel to cause the server to set the <code>Modified()</code> flag.
  */
  void DoSomething();

  /**
    Static constructor. Use this one when instantiating a <code>vtkDrillholeGA</code>.
  */
  static vtkDrillholeGA *New();

  /**
    VTK macro to add CollectRevisions to the class. Ignore it.
  */
  vtkTypeRevisionMacro(vtkDrillholeGA,vtkDataSetAlgorithm);

  /**
    Typically prints out information about a vtk class instance, but we haven't put anything useful here.

    \todo Put some useful output in this method.
  */
  void PrintSelf(ostream& os, vtkIndent indent);

  /**
    This function is needed because we accept an Input and a Source in the filter.

    We set the source connection to be port 1. (Input is port 0)
  */
  void SetSourceConnection(vtkAlgorithmOutput* algOutput);

  /**
    Standard VTK accessor macro for <code>EMajor</code>.
    \see vtkDrillholeGA::EMajor
  */
  vtkGetMacro(EMajor, double);

  /**
    Standard VTK accessor macro for <code>EMedium</code>.
    \see vtkDrillholeGA::EMedium
  */
  vtkGetMacro(EMedium, double);

  /**
    Standard VTK accessor macro for <code>EMinor</code>.
    \see vtkDrillholeGA::EMinor
  */
  vtkGetMacro(EMinor, double);

  /**
    Standard VTK accessor macro for <code>EAzimuth</code>.
    \see vtkDrillholeGA::EAzimuth
  */
  vtkGetMacro(EAzimuth, double);

  /**
    Standard VTK accessor macro for <code>EDip</code>.
    \see vtkDrillholeGA::EDip
  */
  vtkGetMacro(EDip, double);
  
  /**
    Standard VTK accessor macro for <code>ERake</code>.
    \see vtkDrillholeGA::EMajor
  */
  vtkGetMacro(ERake, double);

  /**
    Sets <code>EMajor</code> to <code>value</code>.
    \param value The value that <code>EMajor</code> should be.

    Manually calls <code>this->Modified()</code>.
    \see vtkDrillholeGA::EMajor
  */
  void SetEMajor(double value);

  /**
    Sets <code>EMedium</code> to <code>value</code>.
    \param value The value that <code>EMedium</code> should be.

    Manually calls <code>this->Modified()</code>.
    \see vtkDrillholeGA::EMedium
  */
  void SetEMedium(double value);

  /**
    Sets <code>EMinor</code> to <code>value</code>.
    \param value The value that <code>EMinor</code> should be.

    Manually calls <code>this->Modified()</code>.
    \see vtkDrillholeGA::EMinor
  */
  void SetEMinor(double value);

  /**
    Sets <code>EAzimuth</code> to <code>value</code>.
    \param value The value that <code>EAzimuth</code> should be.

    Manually calls <code>this->Modified()</code>.
    \see vtkDrillholeGA::EAzimuth
  */
  void SetEAzimuth(double value);

  /**
    Sets <code>EDip</code> to <code>value</code>.
    \param value The value that <code>EDip</code> should be.

    Manually calls <code>this->Modified()</code>.
    \see vtkDrillholeGA::EDip
  */
  void SetEDip(double value);

  /**
    Sets <code>ERake</code> to <code>value</code>.
    \param value The value that <code>ERake</code> should be.

    Manually calls <code>this->Modified()</code>.
    \see vtkDrillholeGA::ERake
  */
  void SetERake(double value);

   /**
    Standard VTK accessor macro for <code>PopulationSize</code>.
    \see vtkDrillholeGA::PopulationSize
  */
  vtkGetMacro(PopulationSize, int);

   /**
    Standard VTK accessor macro for <code>NumGenerations</code>.
    \see vtkDrillholeGA::NumGenerations
  */
  vtkGetMacro(NumGenerations, int);

   /**
    Standard VTK accessor macro for <code>IndividualSize</code>.
    \see vtkDrillholeGA::IndividualSize
  */
  vtkGetMacro(IndividualSize, int);

   /**
    Standard VTK accessor macro for <code>MutationProb</code>.
    \see vtkDrillholeGA::MutationProb
  */
  vtkGetMacro(MutationProb, double);

   /**
    Standard VTK accessor macro for <code>MutationGeneProb</code>.
    \see vtkDrillholeGA::MutationGeneProb
  */
  vtkGetMacro(MutationGeneProb, double);

  /**
    Standard VTK mutator macro for <code>PopulationSize</code>.
    \see vtkDrillholeGA::PopulationSize
  */
  vtkSetMacro(PopulationSize, int);

  /**
    Standard VTK mutator macro for <code>NumGenerations</code>.
    \see vtkDrillholeGA::NumGenerations
  */
  vtkSetMacro(NumGenerations, int);

  /**
    Standard VTK mutator macro for <code>IndividualSize</code>.
    \see vtkDrillholeGA::IndividualSize
  */
  vtkSetMacro(IndividualSize, int);

  /**
    Sets <code>MutationProb</code> to <code>value</code>.
    \param value The value that <code>MutationProb</code> should be.

    Manually calls <code>this->Modified()</code>.
    \see vtkDrillholeGA::MutationProb
  */
  void SetMutationProb(double value);

  /**
    Sets <code>MutationGeneProb</code> to <code>value</code>.
    \param value The value that <code>MutationGeneProb</code> should be.

    Manually calls <code>this->Modified()</code>.
    \see vtkDrillholeGA::MutationGeneProb
  */
  void SetMutationGeneProb(double value);

   /**
    Standard VTK accessor macro for <code>DAzimuthMin</code>.
    \see vtkDrillholeGA::DAzimuthMin
  */
  vtkGetMacro(DAzimuthMin, double);

   /**
    Standard VTK accessor macro for <code>DAzimuthMax</code>.
    \see vtkDrillholeGA::DAzimuthMax
  */
  vtkGetMacro(DAzimuthMax, double);

   /**
    Standard VTK accessor macro for <code>DDipMin</code>.
    \see vtkDrillholeGA::DDipMin
  */
  vtkGetMacro(DDipMin, double);

   /**
    Standard VTK accessor macro for <code>DDipMax</code>.
    \see vtkDrillholeGA::DDipMax
  */
  vtkGetMacro(DDipMax, double);

   /**
    Standard VTK accessor macro for <code>DLengthMin</code>.
    \see vtkDrillholeGA::DLengthMin
  */
  vtkGetMacro(DLengthMin, double);

   /**
    Standard VTK accessor macro for <code>DLengthMax</code>.
    \see vtkDrillholeGA::DLengthMax
  */
  vtkGetMacro(DLengthMax, double);

  /**
    Sets <code>DAzimuthMin</code> to <code>value</code>.
    \param value The value that <code>DAzimuthMin</code> should be.

    Manually calls <code>this->Modified()</code>.
    \see vtkDrillholeGA::DAzimuthMin
  */
  void SetDAzimuthMin(double value);

  /**
    Sets <code>DAzimuthMax</code> to <code>value</code>.
    \param value The value that <code>DAzimuthMax</code> should be.

    Manually calls <code>this->Modified()</code>.
    \see vtkDrillholeGA::DAzimuthMax
  */
  void SetDAzimuthMax(double value);

  /**
    Sets <code>DDipMin</code> to <code>value</code>.
    \param value The value that <code>DDipMin</code> should be.

    Manually calls <code>this->Modified()</code>.
    \see vtkDrillholeGA::DDipMin
  */
  void SetDDipMin(double value);

  /**
    Sets <code>DDipMax</code> to <code>value</code>.
    \param value The value that <code>DDipMax</code> should be.

    Manually calls <code>this->Modified()</code>.
    \see vtkDrillholeGA::DDipMax
  */
  void SetDDipMax(double value);

  /**
    Sets <code>DLengthMin</code> to <code>value</code>.
    \param value The value that <code>DLengthMin</code> should be.

    Manually calls <code>this->Modified()</code>.
    \see vtkDrillholeGA::DLengthMin
  */
  void SetDLengthMin(double value);

  /**
    Sets <code>DLengthMax</code> to <code>value</code>.
    \param value The value that <code>DLengthMax</code> should be.

    Manually calls <code>this->Modified()</code>.
    \see vtkDrillholeGA::DLengthMax
  */
  void SetDLengthMax(double value);

   /**
    Standard VTK accessor macro for <code>SegmentLength</code>.
    \see vtkDrillholeGA::SegmentLength
  */
  vtkGetMacro(SegmentLength, double);

   /**
    Standard VTK accessor macro for <code>DoHillClimbLength</code>.
    \see vtkDrillholeGA::DoHillClimbLength
  */
  vtkGetMacro(DoHillClimbLength, bool);

   /**
    Standard VTK accessor macro for <code>DoHillClimbDirection</code>.
    \see vtkDrillholeGA::DoHillClimbDirection
  */
  vtkGetMacro(DoHillClimbDirection, bool);

   /**
    Standard VTK accessor macro for <code>HillClimbLength</code>.
    \see vtkDrillholeGA::HillClimbLength
  */
  vtkGetMacro(HillClimbLength, double);

   /**
    Standard VTK accessor macro for <code>HillClimbAzimuthWindow</code>.
    \see vtkDrillholeGA::HillClimbAzimuthWindow
  */
  vtkGetMacro(HillClimbAzimuthWindow, double);

   /**
    Standard VTK accessor macro for <code>HillClimbDipWindow</code>.
    \see vtkDrillholeGA::HillClimbDipWindow
  */
  vtkGetMacro(HillClimbDipWindow, double);

   /**
    Standard VTK accessor macro for <code>NumPasses</code>.
    \see vtkDrillholeGA::NumPasses
  */
  vtkGetMacro(NumPasses, int);

   /**
    Standard VTK String accessor macro for <code>DrillholeDatabase</code>.
    \see vtkDrillholeGA::DrillholeDatabase
  */
  vtkGetStringMacro(DrillholeDatabase);

   /**
    Standard VTK accessor macro for <code>Resume</code>.
    \see vtkDrillholeGA::Resume
  */
  vtkGetMacro(Resume, bool);

   /**
    Standard VTK accessor macro for <code>CollarLocation</code>.
    \see vtkDrillholeGA::CollarLocation
  */
  vtkGetMacro(CollarLocation, int);
  
  /**
    Standard VTK mutator macro for <code>SegmentLength</code>.
    \see vtkDrillholeGA::SegmentLength
  */
  vtkSetMacro(SegmentLength, double);

  /**
    Standard VTK mutator macro for <code>DoHillClimbLength</code>.
    \see vtkDrillholeGA::DoHillClimbLength
  */
  vtkSetMacro(DoHillClimbLength, bool);

  /**
    Standard VTK mutator macro for <code>DoHillClimbDirection</code>.
    \see vtkDrillholeGA::DoHillClimbDirection
  */
  vtkSetMacro(DoHillClimbDirection, bool);

  /**
    Standard VTK mutator macro for <code>HillClimbLength</code>.
    \see vtkDrillholeGA::HillClimbLength
  */
  vtkSetMacro(HillClimbLength, double);

  /**
    Standard VTK mutator macro for <code>HillClimbAzimuthWindow</code>.
    \see vtkDrillholeGA::HillClimbAzimuthWindow
  */
  vtkSetMacro(HillClimbAzimuthWindow, double);

  /**
    Standard VTK mutator macro for <code>HillClimbDipWindow</code>.
    \see vtkDrillholeGA::HillClimbDipWindow
  */
  vtkSetMacro(HillClimbDipWindow, double);

  /**
    Standard VTK mutator macro for <code>NumPasses</code>.
    \see vtkDrillholeGA::NumPasses
  */
  vtkSetMacro(NumPasses, int);

  /**
    Standard VTK String mutator macro for <code>DrillholeDatabase</code>.
    \see vtkDrillholeGA::DrillholeDatabase
  */
  vtkSetStringMacro(DrillholeDatabase); 

  /**
    Standard VTK mutator macro for <code>Resume</code>.
    \see vtkDrillholeGA::Resume
  */
  vtkSetMacro(Resume, bool);

  /**
    Standard VTK mutator macro for <code>CollarLocation</code>.
    \see vtkDrillholeGA::CollarLocation
  */
  vtkSetMacro(CollarLocation, int);

   /**
    Standard VTK String accessor macro for <code>ResumeFromS2C</code>.
    \see vtkDrillholeGA::ResumeFromS2C
    \todo REFACTOR: Verify this method is necessary. (Does the server need to "Get" this variable?)
  */
  vtkGetStringMacro(ResumeFromS2C); 
  
   /**
    Standard VTK String accessor macro for <code>ResumeFromC2S</code>.
    \see vtkDrillholeGA::ResumeFromC2S
  */
  vtkGetStringMacro(ResumeFromC2S);
  /**
    Standard VTK accessor macro for <code>RefreshPlease</code>.
    \see vtkDrillholeGA::RefreshPlease
  */
  vtkGetMacro(RefreshPlease, int);
  /**
    Standard VTK String accessor macro for <code>CollarFile</code>.
    \see vtkDrillholeGA::CollarFile
  */
  vtkGetStringMacro(CollarFile);

  /**
    Standard VTK String mutator macro for <code>ResumeFromS2C</code>.
    \see vtkDrillholeGA::ResumeFromS2C
  */
  vtkSetStringMacro(ResumeFromS2C);

  /**
    Standard VTK String mutator macro for <code>ResumeFromC2S</code>.
    \see vtkDrillholeGA::ResumeFromC2S
  */
  vtkSetStringMacro(ResumeFromC2S); 

  /**
    Standard VTK mutator macro for <code>RefreshPlease</code>.
    \see vtkDrillholeGA::RefreshPlease
  */
  vtkSetMacro(RefreshPlease, int);

  /**
    Standard VTK mutator macro for <code>CollarFile</code>.
    \see vtkDrillholeGA::CollarFile
  */
  vtkSetStringMacro(CollarFile);
  
protected:
  /**
    Constructor - do not use.
  */
  vtkDrillholeGA();

  /**
    Destructor - do not use.
  */ 
  ~vtkDrillholeGA();


  /**
    Detects whether <code>RefreshPlease</code> is set. If it is set, and there is a valid database,
    <code>ResumeFromS2C</code> will be filled with a list of items that should be added to the 
    appropriate <code>QComboBox</code> on the custom ui panel.
  */
  virtual int RequestInformation(vtkInformation*, vtkInformationVector**, vtkInformationVector*);
  
  /**
    Handles preparing of all input and output. Initializes the Metric Tensor, initializes the 
    Point Locator, gathers input collar points, creates volume neighbours, runs <code>MainLoop</code>,
    writes the Elite List, prints some output to the error console, then cleans up after itself.

    If <code>RefreshPlease</code> is set, <code>RequestData</code> returns immediately.
  */
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

  /**
    Sets the number of input ports and their data types.
  */
  virtual int FillInputPortInformation (int, vtkInformation*);

  /**
    Sets the number of output ports and their data types.
  */
  virtual int FillOutputPortInformation(int, vtkInformation*);

  /**
    Search Ellipsoid Major Axis Magnitude
  */
	double EMajor;

  /**
    Search Ellipsoid Medium Axis Magnitude
  */
  double EMedium;

  /**
    Search Ellipsoid minor Axis Magnitude
  */
  double EMinor;

  /**
    Search Ellipsoid Azimuth Angle
  */
  double EAzimuth;

  /**
    Search Ellipsoid Dip Angle
  */
  double EDip;

  /**
    Search Ellipsoid Rake Angle
  */
  double ERake;

  /**
    Indicates where the input collar points should come from.

    - 0 = CSV File
    - 1 = From Filter Source
  */
  int CollarLocation;

  /**
    Path to the CSV file that has collar points. Only matters if <code>CollarLocation</code> is set to 0.
    \see vtkDrillholeGA::CollarLocation
  */
  char *CollarFile;

  /**
    The number of <code>Individual</code>s that will be created and optimized.
  */
  int PopulationSize;

  /**
    The number of generations the genetic algorithm should run for. The GA will actually run for <code>PopulationSize</code> * <code>NumPasses</code> iterations.
    \see vtkDrillholeGA::NumPasses
  */
  int NumGenerations;

  /**
    The number of <code>Drillhole</code>s in each <code>Individual</code>.
  */
  int IndividualSize;

  /**
    The probability of a mutation occurring to a <code>Drillhole</code>.
    \see vtkDrillholeGA::StagnancyMutationProb
  */
  double MutationProb;

  /**
    Amount that the probability of mutation goes up by when stagnancy is detected.
    \see vtkDrillholeGA::MutationProb
  */
  double StagnancyMutationProb;

  /**
    The probability of a mutation occurring to a specific gene (component) in a <code>Drillhole</code>.
    \see vtkDrillholeGA::StagnancyMutationGeneProb
  */
  double MutationGeneProb;

  /**
    Amount that the probability of a gene mutation goes up by when stagnancy is detected.
    \see vtkDrillholeGA::MutationGeneProb
  */
  double StagnancyMutationGeneProb;

  /**
    The length between points in the generated <code>Drillhole</code>s. 
    
    E.g., if the <code>Drillhole</code> is 500 long and <code>SegmentLength</code> is 50, there will be 11 points.
    \see vtkDrillholeGA::HillClimbLength
  */
  double SegmentLength;

  /**
    Indicates if the hill climbing algorithm will try to optimize <code>Drillhole</code> length.
  */
  bool DoHillClimbLength;

  /**
    Indicates if the hill climbing algorithm will try to optimize <code>Drillhole</code> direction (azimuth, dip).
  */
  bool DoHillClimbDirection;

  /**
    How much a <code>Drillhole</code> will be shortened or lengthened during hill climbing.
    \see vtkDrillholeGA::SegmentLength
  */
  double HillClimbLength;

  /**
    How many degrees a <code>Drillhole</code>'s Azimuth will be modified by during hill climbing.
  */
  double HillClimbAzimuthWindow;

  /**
    How many degrees a <code>Drillhole</code>'s Dip will be modified by during hill climbing.
  */
  double HillClimbDipWindow;

  /**
    The number of hill climbing passes that should occur. Each hill climbing pass happens after the GA finishes. The number of total iterations will then be <code>NumPasses</code> * <code>NumGenerations</code>.
    
    If hill climbing is not turned on, this will simply re-run the GA <code>NumPasses</code> times.
    \see vtkDrillholeGA::NumGenerations
  */
  int NumPasses;

  /**
    Minimum value constraint on <code>Drillhole</code> Azimuth. 
  */
  double DAzimuthMin;

  /**
    Maximum value constraint on <code>Drillhole</code> Azimuth. 
  */
  double DAzimuthMax;

  /**
    Minimum value constraint on <code>Drillhole</code> Dip. 
  */
  double DDipMin;

  /**
    Maximum value constraint on <code>Drillhole</code> Dip. 
  */
  double DDipMax;

  /**
    Minimum value constraint on <code>Drillhole</code> Length. 
  */
  double DLengthMin;

  /**
    Maximum value constraint on <code>Drillhole</code> Length. 
  */
  double DLengthMax;

  /**
    Path to the Drillhole Database that saves previous elite list runs.
  */
  char* DrillholeDatabase;

  /**
    Hackish variable used to send data from the server to the client. 

    Used to send the list of elite list possibilities in the drillhole database.
  */
  char* ResumeFromS2C;

  /**
    Hackish variable used to send data from the client to the server. 

    Used to send the elite list to resume from.
  */
  char* ResumeFromC2S;

  /**
    When true, indicates that when <code>RequestInformation</code> and <code>RequestData</code> run, 
    only fill the server-to-client variable with data so it can update the <code>QComboBox</code>
    on the custom panel, then return and do no processing.
  */
  int RefreshPlease;

  /**
    Indicates that the GA should resume from an elite list stored in the Drillhole database.
  */
  bool Resume;
  

private:
  //HAH! I figured out why vtk disables these. It's so that implicit copying is disabled.
  //The compiler won't autogenerate default copy constructor and copy assignment operators
  //So users of these classes will have to explicitly copy data themselves.
  
  /**
    Essentially disabled copy constructor. Do not use.
  */
  vtkDrillholeGA(const vtkDrillholeGA&);

  /**
    Essentially disabled copy assignment operator. Do not use.
  */
  void operator = (const vtkDrillholeGA&);

  /**
    Meant to be a platform-independent way of getting the number of milliseconds since some previous time.

    Currently only supports Windows.
  */
  unsigned int GetMS();

  /**
    Writes the elite list into the Drillhole Database file.
  */
  int WriteElite();

  /**
    Determines if <code>DrillholeDatabase</code> is a valid file path.
    \see vtkDrillholeGA::DrillholeDatabase
    \return 0 if <code>DrillholeDatabase</code> is not a valid file path
    \return 1 if <code>DrillholeDatabase</code> is a valid file path
  */  
  int FileNameValid();

  /**
    Determines if <code>DrillholeDatabase</code> is a valid MIRARCO .dhd file.
    \see vtkDrillholeGA::DrillholeDatabase
    \return 0 if the database is not valid
    \return 1 if the database is valid
  */
  int DatabaseValid();

  /**
    Tries to open an SQLite Database.
    \return 0 if the database could not be opened.
    \return 1 if the database was opened successfully.
  */
  int OpenDatabase(vtkSQLiteDatabase* db);

  /**
    When the input collar points come from the Filter's Source, this is where they are stored.
  */
	vtkDataSet *InputCollarPoints;

  /**
    This is the grid of points representing the volume that should be explored. Comes from the Filter's Input. 
  */
	vtkDataSet *InputGrid;

  /**
    The magnitudes (major, medium, minor) of the search ellipsoid.
  */
  double Ranges[3];

  /**
    The angles (azimuth, dip, rake) of the search ellipsoid.
  */
  double Angles[3];

  /**
    The maximum value constraints for the six genes (components) of a <code>Drillhole</code>.
  
    In order of (x, y, z, azimuth, dip, length).

    \todo REFACTOR: ChromosomeRangeMax should be abstracted out like into a Component class.
  */
  double ChromosomeRangeMax[6];

  /**
    The minimum value constraints for the six genes (components) of a <code>Drillhole</code>.
  
    In order of (x, y, z, azimuth, dip, length).

    \todo REFACTOR: ChromosomeRangeMin should be abstracted out like into a Component class.
  */
  double ChromosomeRangeMin[6];

  /**
    Stores the elite list.
  */
  Population *Elite;

  /**
    Generates an initial random <code>Population</code>.
  */
  Population InitialPopulation();

  /**
    Grabs the best drillhole from the drillhole database.

    \todo FEATURE: In some future version, maybe we should store the whole 
          elite list instead of just the best one from the elite list!

    \return An <code>Individual</code> that represents the best drillhole in a previous run.
    \return If there was an error, the <code>Individual</code> will not be initialized.
  */
  Individual ResumeElite();

  /**
    Obtains the collar points from wherever they were specified (CSV file or otherwise).

    \return true if the collar points could be retrieved.
    \return false if the collar points could not be retrieved for some reason.
  */
  bool ReadCollarPoints();

  /**
    Evaluates a <code>Population</code>'s fitness.
  */
  void Fitness(Population&);

  /**
    Evaluates an <code>Individual</code>'s fitness. 
  */
  void IndividualFitness(Individual&);

  /**
    Evaluates an <code>Individual</code>'s fitness.

    This version also returns a <code>vtkIntArray</code> that represents which of 
    the points in the volume were explored by the <code>Individual</code>.
  */
  void IndividualFitness(Individual&, vtkIntArray*);

  /**
    Takes an input <code>Population</code> and uses the <code>Individual</code>s as 
    parents to form children <code>Individual</code>s.
  */
  void Crossover(Population&);

  /**
    Takes an input <code>Population</code> and selects a number of 
    <code>Individuals</code> based on roulette-selection.

    \return The selected <code>Population</code>.
  */
  Population Select(Population &population);

  /**
    Mutates the members of a <code>Population</code> using probabilities.
  */
  void Mutation(Population&);

  /**
    Modifies conditions of operation when stagnancy is detected (e.g., 
    increasing chance of Mutation).
  */
  void Stagnancy(Population&);

  /**
    This is where the main algorithm is run for the GA.

    \todo REFACTOR: This MainLoop algorithm may make better sense in GA.h

    \todo TUNING: The ordering of hill climbing (length then direction) has an 
                  impact on performance, consider testing alternate arrangements
  */
  int MainLoop(vtkPolyData*, vtkDataSet*);

  /**
    Runs length hill climbing algorithm.
  */
  void LengthHillClimb();

  /**
    Runs direction hill climbing algorithm.
  */
  void DirectionHillClimb();

  /**
    Gets the endpoint of a <code>Drillhole</code>. 
  */
  void GetEndPoint(Drillhole &drillhole, double *xyz);

  /**
    Gets the azimuth, dip, and length when only start point and end point are available.

    Useful when reading drillholes back from the database.
  */
  void GetAzimuthDipLength(double *start_point, double *end_point, double *az, double *dip, double *length);

  /**
    Gets the points that make up the <code>Drillhole</code>s in <code>individual</code>.
  */
  void GetDrillholePoints(Individual &individual, vtkPoints *points);

  /**
    Gets the end points that make up the <code>Drillhole</code>s in <code>p</code>.
  */
  void GetDrillholeEndPoints(Population &p, vtkPoints *points);

  /**
    Gets the end points that make up the <code>Drillhole</code>s in <code>i</code>.
  */
  void GetDrillholeEndPoints(Individual &i, vtkPoints *points);


  //BTX
  /**
    When the input collar points come from a file, this is where they are stored.
  */
  std::vector<std::vector<double>> CollarPoints;
  //ETX

  /**
    Produces a pseudo-random number between 0 and 1
  */
  double Rand01();

  /**
    Pseudo-randomly produces either 0 or 1.
  */
  double Rand0Or1();

  /**
    Returns a random (constraint-satisfying) value for the gene indicated by <code>index</code>.

  */
  double RandGene(int);

  /**
    Initializes the Metric Tensor
  */
  void SetMetric();
  /**
    Computes the anisotropic squared distance between 2 points
  */
  double EvaluateDist2(double* P, double* Q);

  /**
    Constructs a rotation matrix for the metric tensor.
  */
  void MakeRotMatrix(double ax, double ay, double az, double *m);

  /**
    The Metric Tensor
  */
  double* MetricTensor;

  /**
    This point locator is used to search for neighbours inside the input volume.
  */
  vtkPointLocator *Locator;

  //BTX
  /**
    Method that optimizes neighbour-finding by finding the volume's own neighbours. Then the <code>Drillhole</code>'s
    points only need to find the nearest point in the volume and add its neighbours.
  */
  void InitialNeighbors();

  /**
    Sets whether any two points in the input volume are neighbours.
  */
  void SetNeighbor(int row, int col, int is_neighbor);

  /**
    Returns whether any two points in the input volume are neighbours.
  */
  bool IsNeighbor(int row, int col);

  /**
    Stores the neighbours for the volume of points. 

    Note that this used to be in matrix form, and we found out really quickly that while it was fast, 
    it took up way too much memory in PVG and so we switched to a map of <code>vectors</code>.
  */
  std::map<int, std::vector<int>> neighbors;
  //ETX
};

#endif
