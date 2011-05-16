#ifndef __ColorMaps_h
#define __ColorMaps_h

#include <map>
#include <string>

struct RGB
	{
	float red;
	float green;
	float blue;
	};


class InternalMap;

class GocadColorMap
{
public:
	GocadColorMap();
  ~GocadColorMap();
  		
	void GetColor(std::string color, double* rgb);		
		
protected:  
	std::map< std::string, RGB> Colors;    
	std::map< std::string, RGB>::iterator ColorIterator;

	inline void AddColor(std::string name, float r, float g, float b )
		{
		RGB rgb;
		rgb.red = r;
		rgb.green = g;
		rgb.blue = b;
		this->Colors.insert( std::make_pair( name, rgb ) );
		}
};
#endif