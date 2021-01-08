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
    arr_tk,             // 'array'
    of_tk,              // 'of'
    if_tk,              // 'if'
    then_tk,            // 'then'
    else_tk,            // 'else'
    do_tk,              // 'do'
    for_tk,             // 'for'
    to_tk,              // 'to'
    downto_tk,          // 'downto'
    bool_false_tk,      // 'false'
    bool_true_tk,       // 'true'
    break_tk,           // 'break'
    dot_tk,             // '.'
    dots_arr_tk,        // '..'
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
    osb_tk,             // '['
    csb_tk,             // ']'
    or_tk,              // 'or'
    and_tk,             // 'and'
    xor_tk,             // 'xor'
    bool_eqv_tk,        // '=='
    bool_noneqv_tk,     // '<>'
    bool_bigger_tk,     // '>'
    bool_less_tk,       // '<'
    bool_bigeqv_tk,     // '>='
    bool_leseqv_tk,     // '<='
    comp_tk,            // 'comparison signs'
    eof_tk              // 'end of file'
};

enum errors {
    UNKNOWN_LEXEM,          // Unknown lexeme
    EOF_ERR,                // End of file error
    MUST_BE_ID,             // Error: Must be identifier
    MUST_BE_SEMI,           // Error: Must be ';'
    MUST_BE_PROG,           // Error: Must be 'program'
    MUST_BE_COMMA,          // Error: Must be ','
    MUST_BE_CONST,          // Error: Must be constant
    MUST_BE_COMP,           // Error: Must be comparison sign
    MUST_BE_TYPE,           // Error: Must be type of identifier
    MUST_BE_DOT,            // Error: Must be '.'
    MUST_BE_DOTS_ARR,       // Error: Must be '..'
    MUST_BE_ASS,            // Error: Must be ':='
    MUST_BE_BRACKET,        // Error: Must be '('
    MUST_BE_BRACKET_END,    // Error: Must be ')'
    MUST_BE_ARRBRACKET,     // Error: Must be '['
    MUST_BE_ARRBRACKET_END, // Error: Must be ']'
    MUST_BE_DO,             // Error: Must be 'do'
    MUST_BE_TO,             // Error: Must be 'to'
    MUST_BE_THEN,           // Error: Must be 'then'
    MUST_BE_OF,             // Error: Must be 'of'
    DUPL_ID_ERR,            // Error: Duplicate declaration on identifier
    UNKNOWN_ID,             // Error: Undefined identifier
    INCORRECT_TYPE,         // Error: Identifier is not an array
    INCORRECT_RANGE,        // Error: Out of array range
    // TODO: Add other types of error
};

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

