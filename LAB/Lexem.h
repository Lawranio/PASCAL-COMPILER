#ifndef LEXEM_H
#define LEXEM_H

#include <string>

enum tokens {
    unknown_tk = -1,    // we get unknown token
    program_tk = 0,     // 'program'
    var_tk,             // 'var'
    begin_tk,           // 'begin'
    end_tk,             // 'end'
    type_tk,            // 'type'
    id_tk = 8,          // any [aA-zZ][0-9]
    constant_tk = 9,    // 0-9
    dot_tk,             // '.'
    comma_tk,           // ','
    ddt_tk,             // ':'
    semi_tk,            // ';'
    eqv_tk,             // '='
    ass_tk,             // ':='
    plus_tk,            // '+'
    minus_tk,           // '-'
    mul_tk,             // '*'
    div_tk,             // 'div'
    mod_tk,             // 'mod'
    opb_tk,             // '('
    cpb_tk,             // ')'
    eof_tk              // 'end of file'
};

enum errors {
    UNKNOWN_LEXEM,   // Unknown lexeme
    EOF_ERR,         // End of file error
    MUST_BE_ID,      // Error: Must be identifier
    MUST_BE_SEMI,    // Error: Must be ';'
    MUST_BE_PROG,    // Error: Must be 'program'
    MUST_BE_COMMA,   // Error: Must be ','
    MUST_BE_TYPE,    // Error: Must be type of identifier
    MUST_BE_DOT,     // Error: Must be '.'
    MUST_BE_ASS,     // Error: Must be ':='
    MUST_BE_BRACKET, // Error: Must be ')' in expression
    DUPL_ID_ERR,     // Error: Duplicate declaration on identifier
    UNKNOWN_ID,      // Error: Undefined identifier
    // TODO: Add other types of error
};

//typedef std::pair<std::string, tokens> lexem;

class Lexem
{
public:
    Lexem() = default;
    Lexem(const std::string &t_name, tokens token_tk, int t_line) : name(t_name), token(token_tk), line(t_line) {};

    int                 GetLine() { return line; }
    tokens              GetToken() { return token; }
    const std::string&  GetName() { return name; }

private:
    std::string     name{ "" };
    tokens          token{ unknown_tk };
    int             line{ 0 };
};


#endif // !LEXEM_H

