#include "lab_01.h"


int Parse_01(const std::string& file_path) {

	Lexer lex(file_path.c_str());
	auto table = lex.ScanCode(); //table of lexemes

	Syntax syntx(std::move(table));
	syntx.ParseCode(); 
	   	

	return EXIT_SUCCESS;
}