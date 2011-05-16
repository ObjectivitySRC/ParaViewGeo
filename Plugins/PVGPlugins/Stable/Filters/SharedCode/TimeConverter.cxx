//written by Robert Maynard
//Mirarco Nov 2007
//converts ISODate/Time strings to UTC decimals
//Also can output the utc Decimals as MSDate integers

#include "TimeConverter.h"
#include <ctime>
#include <iostream>
#include <math.h>
#include <cstdio>

using namespace std;

#define ISO_DATE "%d-%d-%dT%d:%d:%d"

//These are needed to make
//sure all date conversion happen after ctime/unix time started
//aka 1970-1-1
#define MS_DATE_OFFSET 25569
#define MATLAB_DATE_OFFSET 719529
#define JULIAN_DATE_OFFSET 2440587.5
#define MODIFIED_JULIAN_DATE_OFFSET 40587
#define SECONDS_IN_DAY 86400
#define JULIAN_TO_MODIFIED_JULIAN  2400000.5
#define JULIAN_TO_MSDATE 2415018.5
#define JULIAN_TO_MATLAB 1721059.5

TimeConverter::TimeConverter()
{  
  this->Format = "%Y-%m-%dT%H:%M:%S";
	this->year = 0;
	this->hour = 0;
	this->min = 0;
	this->sec = 0;
	this->month = 0;
	
}

TimeConverter::TimeConverter(char* Format)
{ 
  this->Format = Format;
}

TimeConverter::~TimeConverter()
{    

}

void TimeConverter::Parse(double dateTime, int format)
{
  
  double t = 0.0;
  
  switch(format)
    {
    //the lowest date is Jan 1 1970 so we have to subtract the amount of days, that are from 
    // the time formats creation to 1970 first from each date
    case MATLAB:
      //place it here so we can drop down into a reasonable time frame
      dateTime = dateTime - (MATLAB_DATE_OFFSET-1);       
      day = (int)dateTime;          
      t = (dateTime - day) * SECONDS_IN_DAY; //the number of seconds that have passed since 00:00:00      
      sec = int(t + 0.5 ); // round the number while we convert it      
      break;
    case MSDATE:
      dateTime = dateTime - (MS_DATE_OFFSET-1);
      day = (int)dateTime;          
      t = (dateTime - day) * SECONDS_IN_DAY; //the number of seconds that have passed since 00:00:00      
      sec = int(t + 0.5 ); // round the number while we convert it                  
      //lowest day is now 1970, so we always have this bug, from lotus123             
      break;    
    case JULIAN:
      //we want to fall through to modified
      dateTime = dateTime - JULIAN_TO_MODIFIED_JULIAN;        
    case JULIAN_M:                  
      dateTime = dateTime- (MODIFIED_JULIAN_DATE_OFFSET-1);
      day = int(dateTime);       
      t = (dateTime - day) * SECONDS_IN_DAY; //the number of seconds that have passed since 00:00:00
      sec = int(t + 0.5); // round the number while we convert it      
      break;    
    }
  
  this->year = 1970;
  this->month = 1;
  this->day = day;
  this->hour=0;
  this->min=0;
  this->sec = sec;  
}

void TimeConverter::Parse(char* ISODate)
{
  //uses an ISODate format string
  //"2010-01-01T10:15:30"
  int year,month,day,hour,min,sec;
  //parse the c-string using sscanf
  sscanf(ISODate,ISO_DATE,&year,&month,&day,&hour,&min,&sec);       
  this->year = year;
  this->month = month;
  this->day = day;
  this->hour = hour;
  this->min = min;
  this->sec = sec;     
  
}

int TimeConverter::GetTime()
{
  cout << "year " << this->year <<endl;
  cout << "month " << this->month <<endl;
  cout << "day " << this->day <<endl;
  cout << "hour " << this->hour <<endl;
  cout << "min " << this->min <<endl;
  cout << "sec " << this->sec <<endl;
  return 1;
}


double TimeConverter::GetMSDate()
{
  //convert this to a ms date
  return this->GetDate(JULIAN_TO_MSDATE);
}
double TimeConverter::GetMatlabDate()
{
  //convert this to a mat lab date
  return this->GetDate( JULIAN_TO_MATLAB);
}


double TimeConverter::GetJulianDate()
{
  //convert this to a julian date
  //date is alreay in julian
  return this->GetDate( 0 );
}

double TimeConverter::GetModifiedJulianDate()
{
  //have to convert this to modified julian date
  return this->GetDate(JULIAN_TO_MODIFIED_JULIAN);
}

char* TimeConverter::GetISODate()
{
  //big hack since we moved to julian time, this will show a string as long 
  // as the time is epoc okay
  //start year is 1970
  int year=70,month=0,day=0,hour=0,min=0,sec=0;
  double t = 0.0;
  //create the time object using the localtime, 
  //have to do this to set allot of extra info that is needed.
  time_t rawTime;
  struct tm * timeInfo;

  time ( &rawTime );
  timeInfo = localtime ( &rawTime );
  
  double dateTime = this->GetMSDate();
  dateTime = dateTime - (MS_DATE_OFFSET-1);
  day = (int)dateTime;          
  t = (dateTime - day) * SECONDS_IN_DAY; //the number of seconds that have passed since 00:00:00      
  sec = int(t + 0.5 ); // round the number while we convert it                  
  //lowest day is now 1970, so we always have this bug, from lotus123 
  
              
  timeInfo->tm_year = year;
  timeInfo->tm_mon = month;
  timeInfo->tm_mday = day;        
  timeInfo->tm_hour = hour;
  timeInfo->tm_min = min;
  timeInfo->tm_sec = sec;             
  rawTime = mktime ( timeInfo );    
  
	//going to ignore DST
  

  //do sanity check
  if (rawTime != -1)
    {
    char* buffer = new char[24];
    strftime (buffer,24,this->Format,timeInfo);
    return buffer;
    }
  else
    {
    return "No Time";
    }    
}

double TimeConverter::GetDate(double offset)
{  
  double UT= this->hour +this->min/60 + this->sec/3600;
  
  int sig = -1;
  
  if ( (100 * this->year + this->month - 190002.5) > 0 )
    {
    sig = 1;
    }
  
  double JD = 367 * this->year - int ( 7 * (this->year + int (( this->month + 9) / 12 )) /4) + int(275 * this->month/9) + this->day +1721013.5 + UT/24 - 0.5 *sig +0.5;
            
  //convert to the proper time format
  return JD - offset;
}


int main(int argc, const char* argv[])
{  
  TimeConverter x = TimeConverter();
  
  //dont round or do other shit with the 
  //date we need the pure form to make sure everything is right
  cout << fixed << showpoint;  
  
  //sample isoDate test time
  char* ISODate = "2005-09-11T20:59:59";
  x.Parse(ISODate);
  cout << "Local Time " << x.GetTime() << endl;
  cout << "MSDate Time "<< x.GetMSDate() << endl;
  cout << "MatlabDate Time "<< x.GetMatlabDate() << endl;
  cout << "Julian Time "<< x.GetJulianDate() << endl;
  cout << "Modified Julian Time "<< x.GetModifiedJulianDate() << endl;
  
  //sample ms Date
  x.Parse(39423.354166666,0); //2007-12-07 8:30
      
  cout << "Local Time " << x.GetTime() << endl;  
  cout << "MSDate Time "<< x.GetMSDate() << endl;
  cout << "MatlabDate Time "<< x.GetMatlabDate() << endl;
  cout << "Julian Time "<< x.GetJulianDate() << endl;
  cout << "Modified Julian Time "<< x.GetModifiedJulianDate() << endl;
  
   //sample mat lab date
  x.Parse(733388.5,3); // Dec 12 2007 12:00
      
  cout << "Local Time " << x.GetTime() << endl;  
  cout << "MSDate Time "<< x.GetMSDate() << endl;
  cout << "MatlabDate Time "<< x.GetMatlabDate() << endl;
  cout << "Julian Time "<< x.GetJulianDate() << endl;
  cout << "Modified Julian Time "<< x.GetModifiedJulianDate() << endl;
  
   //sample mat lab date
  x.Parse(54446.35416666651,2); // Dec 12 2007 8:30
  cout << " Modified Julian Dec 12 2007 8:30 " << endl;   
  cout << "Local Time " << x.GetTime() << endl;  
  
  cout << "MSDate Time "<< x.GetMSDate() << endl;
  cout << "MatlabDate Time "<< x.GetMatlabDate() << endl;
  cout << "Julian Time "<< x.GetJulianDate() << endl;
  cout << "Modified Julian Time "<< x.GetModifiedJulianDate() << endl;
  
  x.Parse(732566.3541666666, 3); //11 Sep 2005 08:30:00
  cout << "MatLab 11 Sep 2005 08:30:00" << endl;
  cout << "Local Time " << x.GetTime() << endl;  
  
  cout << "MSDate Time "<< x.GetMSDate() << endl;
  cout << "MatlabDate Time "<< x.GetMatlabDate() << endl;
  cout << "Julian Time "<< x.GetJulianDate() << endl;
  cout << "Modified Julian Time "<< x.GetModifiedJulianDate() << endl;
  
  cout << " " << endl;
  x.Parse(38606.5,0); //11 Sep 2005 12:00:00
  cout << "MSDATE 11 Sep 2005 12:00:00" << endl; 
  cout << "MSDate Time "<< x.GetMSDate() << endl;
  cout << "MatlabDate Time "<< x.GetMatlabDate() << endl;
  cout << "ISODate Time" <<x.GetISODate() <<endl;
  
 
  return 0;
}

#undef ISO_DATE 
#undef ISO_STR_FORMAT 
#undef MS_DATE_OFFSET
#undef JULIAN_DATE_OFFSET
#undef MODIFIED_JULIAN_DATE_OFFSET
#undef SECONDS_IN_DAY
#undef JULIAN_TO_MODIFIED_JULIAN
#undef MATLAB_TO_MSDATE
