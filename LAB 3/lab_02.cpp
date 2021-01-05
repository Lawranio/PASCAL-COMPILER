#include "lab_02.h"


int Parse_02(const std::string& file_path) {

	Lexer lex(file_path.c_str());
	auto table = lex.ScanCode(); //table of lexemes

	Syntax syntx(std::move(table));
	syntx.ParseCode(); 
	   	

	return EXIT_SUCCESS;
}