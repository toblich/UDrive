#ifndef EXCEPCIONES_H_
#define EXCEPCIONES_H_

#include <stdexcept>
#include <string.h>
#include <iostream>

using namespace std;

class KeyNotFound : public exception {
  private:
    string err_msg;
  public:
    KeyNotFound() : err_msg("") {};
    KeyNotFound(const char *msg) : err_msg(msg) {};
    KeyNotFound(string msg) { err_msg = msg; };
    KeyNotFound(const KeyNotFound& original) : err_msg(original.what()) {};
    ~KeyNotFound() throw() {};
    const char *what() const throw() { return ("KeyNotFound: " + err_msg).c_str(); };
};

class InvalidDBAction : public exception {
  private:
    string err_msg;
  public:
    InvalidDBAction() : err_msg("") {};
    InvalidDBAction(const char *msg) : err_msg(msg) {};
    InvalidDBAction(string msg) { err_msg = msg; };
    InvalidDBAction(const InvalidDBAction& original) : err_msg(original.what()) {};
    ~InvalidDBAction() throw() {};
    const char *what() const throw() { return ("InvalidDBAction: " + err_msg).c_str(); };
};

#endif /* EXCEPCIONES_H_ */
