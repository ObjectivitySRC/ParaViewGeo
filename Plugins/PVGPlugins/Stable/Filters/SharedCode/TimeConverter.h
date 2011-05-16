#ifndef __TimeConverter__h
#define __TimeConverter__h
#include <time.h>
class TimeConverter
{
  public:
    TimeConverter();                
    TimeConverter(char*);           
    ~TimeConverter();             
    void Parse(char*);
    void Parse(double,int);
    int GetTime();
        
    char* GetISODate();
    double GetMSDate();
    double GetMatlabDate();
    double GetJulianDate();
    double GetModifiedJulianDate();

		// MLivingstone
		// These were missing from the PlotPropertiesOverTime version
		// Added back in so ApplySchedule would work
		void setYear(int y)
		{
			this->year = y;
		}
		void setMonth(int m)
		{
			this->month = m;
		}
		void setDay(int d)
		{
			this->day = d;
		}
		void setTime(double h, double m, double s)
		{
			this->hour = h;
			this->min = m;
			this->sec = s;
		}
		void setDate(int y, int m, int d)
		{
			this->year = y;
			this->month = m;
			this->day = d;
		}
		

		enum Format{MSDATE,JULIAN,JULIAN_M,MATLAB}; 
  private:    
    char* Format;
    int day;
    int month;
    int year;
    double hour;
    double min;
    double sec;
    
       
    double GetDate(double offset);    
        
};

#endif
