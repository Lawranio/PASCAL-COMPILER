#ifndef LECS_PARS_DAY2_H
#define LECS_PARS_DAY2_H

#include <fstream>      
#include <iostream>     
#include <list>
#include <string>
#include <vector>
#include "Lexer.h"
#include "Syntax.h"
#include "GenCode.h"

/*
 * Given grammar:
 * <Soft>			::= program <id> ; <block> .
 * <block>			::= <var part> <state part>
 * <var part>		::= var <var dec> : ( <type> | <array>);
 * <var dec>		::= <id> { , <var dec> }
 * <array>			::= array <range> of <type>
 * <range>			::= [ <constant> .. <constant> ]
 * <state part>		::= <compound>
 * <compound>		::= begin <state> { ; <state> } end
 * <state>			::= <assign> | <compound> | <for_op> | <if_op>
 * <for_op>			::= for <id> := <constant> (to | downto) <constant> do <state>
 * <if_op>			::= if <comp> then <state> { ; else <state> }
 * <assign>			::= <id> := <exp> ;
 * <exp>			::= <id> | <constant> | <comp> | <bool_exp> | <arith_exp>
 * <comp>			::= <eq> | <noneq> | <big> | <less> | <bigeq> | <leseq>  
 * <bool_exp>		::= <or> | <and> | <xor>
 * <eq>				::= <exp> == <exp>
 * <noneq>			::= <exp> <> <exp>
 * <big>			::= <exp> > <exp>
 * <less>			::= <exp> < <exp>
 * <bigeq>			::= <exp> >= <exp>
 * <leseq>			::= <exp> <= <exp>
 * <or>				::= <exp> or <exp>
 * <and>			::= <exp> and <exp>
 * <xor>			::= <exp> xor <exp>
 * <arith_exp>		::= <add> | <sub> | <mul> | <div> | <mod> 
 * <add>			::= <exp> + <exp>
 * <sub>			::= <exp> - <exp>
 * <mul>			::= <exp> * <exp>
 * <div>			::= <exp> div <exp>
 * <mod>			::= <exp> mod <exp>
 * <type>			::= integer
 * <id>				::= a-z
 * <constant>		::= 0-9
 */ 


int Parse_02(const std::string& file_path);

#endif //LECS_PARS_DAY2_H
