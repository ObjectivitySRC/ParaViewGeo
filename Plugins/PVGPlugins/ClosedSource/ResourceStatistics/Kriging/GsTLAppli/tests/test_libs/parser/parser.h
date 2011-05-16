#ifndef __TEST_PARSER_H__
#define __TEST_PARSER_H__

#include <string>
#include <map>

typedef void (*Command)(std::string str);

class Parser{
  public:
    Parser();
    ~Parser() {};
    void execute(std::string str);

  private:
    typedef std::map<std::string, Command> Map;
    std::map<std::string, Command> map_;

};

void new_interf(std::string str);
void remove(std::string str);
void list(std::string str);

#endif
