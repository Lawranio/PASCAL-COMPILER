#include "Lexer.h"

Lexer::Lexer(const char* file_path) {
	try {
		code.open(file_path);
	}
	catch (const std::exception& exp) {
		std::string lel(exp.what());
		std::string what = "<E> Lexer: Catch exception in constructor:";
		throw std::runtime_error(what + lel);
	}
}

std::vector<Lexem> Lexer::ScanCode()
{
    try {
        if (!code.is_open()) {
            std::cerr << "<E> Can't open file" << std::endl;
            return lex_table;
        }

        while (!code.eof()) {
            lex_table.emplace_back(GetLex());
        }

        return lex_table;
    }
    catch (const std::exception& exp) {
        std::cerr << "<E> Catch exception in " << __func__ << ": " << exp.what() << std::endl;
        return lex_table;
    }
}

Lexer::~Lexer() {
	code.close();
}

Lexem Lexer::GetLex()
{
    try {
        auto ch = GetCurrentCurs();
        while (ch == -1 || ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t') {
            if (ch == '\n') line++;

            ch = GetChar();


            if (code.eof()) return Lexem(std::move("EOF"), eof_tk, line); // if end of file
        }

        auto isId = [](char ch) {
            return std::isalpha(static_cast<unsigned char>(ch)) ||
                std::isdigit(static_cast<unsigned char>(ch));
        };


        std::string lex;
        if (std::isdigit(static_cast<unsigned char>(ch))) { // Constants (Numbers)
            while (std::isdigit(static_cast<unsigned char>(ch))) {
                lex += ch;
                ch = GetChar();
            }

            return Lexem(std::move(lex), constant_tk, line);
        }
        else if (std::isalpha(static_cast<unsigned char>(ch))) { // Identificators
            while (isId(ch)) {
                lex += ch;
                ch = GetChar();
            }

            if (lex == "program")       { return Lexem(std::move(lex), program_tk, line); } //return std::make_pair(lex, program_tk);  
            else if (lex == "var")      { return Lexem(std::move(lex), var_tk, line); }     //return std::make_pair(lex, var_tk); 
            else if (lex == "begin")    { return Lexem(std::move(lex), begin_tk, line); }   //return std::make_pair(lex, begin_tk); 
            else if (lex == "integer")  { return Lexem(std::move(lex), type_tk, line); }    //return std::make_pair(lex, type_tk);
            else if (lex == "end")      { return Lexem(std::move(lex), end_tk, line); }     //return std::make_pair(lex, end_tk);
            else if (lex == "div")      { return Lexem(std::move(lex), div_tk, line); }
            else if (lex == "mod")      { return Lexem(std::move(lex), mod_tk, line); }
            else { // it is ID
                return Lexem(std::move(lex), id_tk, line);                                  //return std::make_pair(lex, id_tk);
            }
        }
        else if (std::ispunct(static_cast<unsigned char>(ch))) { // Other symbols
            tokens tok{ unknown_tk };
            switch (ch) {
            case ',': tok = comma_tk; break;
            case '.': tok = dot_tk;   break;
            case ':': tok = ddt_tk;   break;
            case ';': tok = semi_tk;  break;
            case '=': tok = eqv_tk;   break;
            case '+': tok = plus_tk;  break;
            case '-': tok = minus_tk; break;
            case '/': tok = div_tk;   break;
            case '*': tok = mul_tk;   break;
            case '(': tok = opb_tk;   break;
            case ')': tok = cpb_tk;   break;
            default:
                std::cerr << "<E> Unknown token " << ch << std::endl;
                tok = unknown_tk;
                break;
            }
            lex += ch;

            if (tok == ddt_tk) {
                ch = GetChar();
                if (ch == '=') {
                    lex += ch;
                    tok = ass_tk;
                }
            }

            GetChar(); // some kind of k o s t y l; here we look on \n
            return Lexem(std::move(lex), tok, line); //return std::make_pair(lex, tok);
        }
        else {
            std::cerr << "<E> Unknown token " << ch << std::endl;
        }

        return Lexem(std::move(""), unknown_tk, line); //return std::make_pair("", unknown_tk);
    }
    catch (const std::exception&) {
        return Lexem(std::move(""), unknown_tk, line); //return std::make_pair("", unknown_tk);
    }
}

char Lexer::GetChar()
{
	if (code.fail()) {
		std::cerr << "<E> Can't read from the file" << std::endl;
		throw std::runtime_error("File doesn't available");
	}

	if (!code.eof()) {
		code >> std::noskipws >> cursor;
	}
	else {
		std::cerr << "<E> File is EOF early" << std::endl;
		throw std::runtime_error("File is EOF early");
	}

	return cursor;
}
