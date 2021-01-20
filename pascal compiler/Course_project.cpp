#include "Course_project.h"


int Compile(const std::string& file_path) {

	Lexer lex(file_path.c_str());
	auto table = lex.ScanCode(); //table of lexemes

	Syntax syntx(std::move(table));
	auto tree = syntx.ParseCode(); // syntax tree

	if (tree == nullptr) {
		std::cerr << "Error: Invalid syntax tree" << std::endl;
		return -EXIT_FAILURE;
	}

	//GenCode gencod(std::move(*tree));
	//gencod.GenerateAsm(); // final code file

	return EXIT_SUCCESS;
}