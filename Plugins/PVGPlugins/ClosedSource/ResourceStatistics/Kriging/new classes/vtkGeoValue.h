#ifndef _vtkGeoValue_h
#define _vtkGeoValue_h

class vtkGeoValue 
	{
	public:
		typedef float property_type;
    typedef std::vector<float> location_type;

  public:
    vtkGeoValue(property_type value, location_type location) : value(value), location(location) {};

  private:
    property_type value;
    location_type location;

	// GsTL requirements for concept GeoValue
  public:
		bool isInformed() const
			{
      return value != -99;
			}

		property_type property_value() const
			{
			return value;
			}

		void setPropertyValue (const property_type& val)
			{
			value = val;
			}

		const location_type& location() const
			{
			return location;
			}
	};

#endif