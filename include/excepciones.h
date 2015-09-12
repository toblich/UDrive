#ifndef EXCEPCIONES_H_
#define EXCEPCIONES_H_

#include <stdexcept>
#include <string.h>
#include <iostream>

//class KeyNotFound : public exception {};

class KeyNotFound : public std::exception {
  private:
    std::string err_msg;

  public:
    KeyNotFound() : err_msg("") {};
    KeyNotFound(const char *msg) : err_msg(msg) {};
    KeyNotFound(std::string msg) { err_msg = msg; };
    KeyNotFound(const KeyNotFound& original) : err_msg(original.what()) {};
    ~KeyNotFound() throw() {};
    const char *what() const throw() { return this->err_msg.c_str(); };
};

#endif /* EXCEPCIONES_H_ */
