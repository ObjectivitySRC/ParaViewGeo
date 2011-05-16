#include <GsTLAppli/utils/manager.h>

#include <iostream>
#include <string>

typedef std::pair<std::string, std::string> Prefixed_name;
static Prefixed_name split_name(const std::string& name) {
  Prefixed_name s;
  std::string::size_type i = name.find('/', 1);
  if( ( name[0] == '/' ) && ( i != string::npos ) ) {
    s.first = name.substr(0, i);
    s.second = name.substr(i);
  }
  else {
    s.first=name.substr(1);
    s.second="";
  }
  return s;
}

int main() {
  std::string name;
  while( true ) {
    cin >> name;
    Prefixed_name s = split_name(name);
    std::cout << "'" << s.first << "'    '" << s.second << "'" << std::endl;
  }
    
}
