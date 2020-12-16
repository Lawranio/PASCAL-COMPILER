#ifndef LECS_PARS_DAY2_H
#define LECS_PARS_DAY2_H

#include <fstream>      
#include <iostream>     
#include <list>
#include <string>
#include <vector>
#include "Lexer.h"
#include "Syntax.h"

/**
 * Given grammar:
 * <Soft>        ::= program <id> ; <block> .
 * <block>       ::= <var part> <state part>
 * <var part>    ::= var <var dec> : <type> ;
 * <var dec>     ::= <id> { , <var dec> }
 * <state part>  ::= <compound>
 * <compound>    ::= begin <state> { ; <state> } end
 * <state>       ::= <assign> | <compound> | <your_other_operations>
 * <assign>      ::= <id> := <exp> ;
 * <exp>         ::= <id> | <constant>
 * <type>        ::= integer
 * <id>          ::= a-z
 * <constant>    ::= 0-9
 */ 


int Parse_01(const std::string& file_path);

#endif //LECS_PARS_DAY2_H
