#include "lab_02.h"


int Parse_02(const std::string& file_path) {

	Lexer lex(file_path.c_str());
	auto table = lex.ScanCode(); //table of lexemes

	Syntax syntx(std::move(table));
	auto tree = syntx.ParseCode();

	if (tree == nullptr) {
		std::cerr << "Error: Invalid syntax tree" << std::endl;
		return -EXIT_FAILURE;
	}

	//GenCode gencod(std::move(*tree));
	//gencod.GenerateAsm();

	return EXIT_SUCCESS;
}