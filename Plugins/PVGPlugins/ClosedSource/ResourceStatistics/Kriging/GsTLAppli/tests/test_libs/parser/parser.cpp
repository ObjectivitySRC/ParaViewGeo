#include "parser.h"
#include <GsTLAppli/utils/manager.h>
#include <GsTLAppli/appli/manager_repository.h>

#include <utility>
#include <iostream>

typedef std::pair<std::string,std::string> Prefixed_name;

static Prefixed_name space_split(const std::string& name) {
  Prefixed_name s;
  std::string::size_type i = name.find(':', 1);
  if(  i != string::npos  ) {
    s.first = name.substr(0, i);
    s.second = name.substr(i+1);
  }
  else {
    s.first=name.substr(0);
    s.second="";
  }
  return s;
}


void new_interf(std::string str){
  Prefixed_name s = space_split(str);
  Root::instance()->new_interface(s.first, s.second);
}

void list(std::string str) {
  Root::instance()->list_all(std::cout);
}

void remove(std::string str) {
  Root::instance()->delete_interface(str);
}

Parser::Parser() {
  map_.insert(pair<std::string,Command>("new", new_interf));
  map_.insert(pair<std::string,Command>("ls", list));
  map_.insert(pair<std::string,Command>("rm", remove));
}

void Parser::execute(std::string str) {
  Prefixed_name s = space_split(str);
  //cout << "'" << s.first << "'   '" << s.second << "'" << endl;
  Map::iterator it = map_.find(s.first);
  if( it != map_.end() )
    (it->second)(s.second);
  else
    cerr << "invalid command" << endl << endl;
}
