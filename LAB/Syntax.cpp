#include "Syntax.h"


Syntax::Syntax(std::vector<Lexem>&& t_lex_table) {
    if (t_lex_table.empty())
        throw std::runtime_error("<E> Syntax: Lexemes table is empty");
    if (t_lex_table.at(0).GetToken() == eof_tk)
        throw std::runtime_error("<E> Syntax: Code file is empty");
    lex_table = t_lex_table;
    cursor = lex_table.begin();

    operations.emplace(":=", 0);

    operations.emplace("=", 1);
    operations.emplace("<>", 1);
    operations.emplace("<", 1);
    operations.emplace(">", 1);
    operations.emplace("<=", 1);
    operations.emplace(">=", 1);

    operations.emplace("+", 2);
    operations.emplace("-", 2);
    operations.emplace("or", 2);
    operations.emplace("xor", 2);

    operations.emplace("*", 3);
    operations.emplace("/", 3);
    operations.emplace("div", 3);
    operations.emplace("mod", 3);
}

Syntax::~Syntax() {
    Tree::FreeTree(root_tree); // ERROR Tree.h 129 line !!!!!!!!!!
}


int Syntax::ParseCode() {
    std::cout << "Code contains " << lex_table.size() << " lexemes" << std::endl;
    auto& it = cursor;
    if (programParse(it) != 0)
        return -EXIT_FAILURE;

    while (it != lex_table.end() && it->GetToken() != eof_tk)
        blockParse(it);
    std::cout << std::endl;
    std::cout << std::setfill('*') << std::setw(50);
    std::cout << "\r\n";

    root_tree->PrintTree(0);
    std::cout << "EOF" << std::endl;

    return EXIT_SUCCESS;
}

Syntax::lex_it Syntax::getNextLex(lex_it& iter) {
    try {
        if (iter != lex_table.end())
            iter++;
    }
    catch (const std::exception & exp) {
        std::cerr << "<E> Syntax: Catch exception in " << __func__ << ": "
            << exp.what() << std::endl;
    }

    return iter;
}

Syntax::lex_it Syntax::peekLex(int N, lex_it t_iter) {
    try {
        auto iter = t_iter;
        while (iter != lex_table.end()) {
            if (N == 0) return iter;
            iter++; N--;
        }

        return iter;
    }
    catch (const std::exception &) {
        std::cerr << "<E> Syntax: Can't peek so forward" << std::endl;
        return t_iter;
    }
}

int Syntax::programParse(lex_it &t_iter) {
    if (!checkLexem(t_iter, program_tk)) {
        printError(MUST_BE_PROG, *t_iter);
        return -EXIT_FAILURE;
    }

    auto iter = getNextLex(t_iter);
    if (!checkLexem(iter, id_tk)) {
        if (iter->GetToken() == eof_tk) {
            printError(EOF_ERR, *iter);
            return -EXIT_FAILURE;
        }
        else {
            printError(MUST_BE_ID, *iter);
            return -EXIT_FAILURE;
        }
    }

    auto root_name = iter->GetName(); // save the name of program

    iter = getNextLex(t_iter);
    if (!checkLexem(iter, semi_tk)) {
        if (iter->GetToken() == eof_tk) {
            printError(EOF_ERR, *iter);
            return -EXIT_FAILURE;
        }
        else {
            printError(MUST_BE_SEMI, *iter);
            return -EXIT_FAILURE;
        }
    }

    root_tree = Tree::CreateNode(root_name);

    return EXIT_SUCCESS;
}

int Syntax::blockParse(lex_it& t_iter) {
    try {
        auto iter = getNextLex(t_iter);
        switch (iter->GetToken()) {
        case var_tk: {
            root_tree->AddLeftNode("var");
            vardpParse(t_iter, root_tree->GetLeftNode()); 
            break;
        }
        case begin_tk: {
            // TODO: add check on nullptr from compoundParse
            root_tree->AddRightTree(compoundParse(t_iter, 0));
            break;
        }
        case dot_tk: {
            if (!error) std::cout << "Program was parse successfully" << std::endl;
            break;
        }
        default: {
            break;
        }
        }
    }
    catch (const std::exception & exp) {
        std::cerr << "<E> Syntax: Catch exception in " << __func__ << ": "
            << exp.what() << std::endl;
        return -EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

std::list<std::string> Syntax::vardParse(lex_it& t_iter) {
    auto iter = getNextLex(t_iter);
    if (!checkLexem(iter, id_tk)) {
        printError(MUST_BE_ID, *iter);
        return std::list<std::string>();
    }

    if (isVarExist(iter->GetName())) printError(DUPL_ID_ERR, *iter);
    else id_map.emplace(iter->GetName(), Variable("?", "?"));

    std::list<std::string> var_list;
    var_list.push_back(t_iter->GetName());

    iter = getNextLex(t_iter);
    if (checkLexem(iter, comma_tk))
        var_list.splice(var_list.end(), vardParse(t_iter));

    return var_list;
}

int Syntax::vardpParse(lex_it& t_iter, Tree *t_tree) {
    auto var_list = vardParse(t_iter);
    if (!checkLexem(t_iter, ddt_tk)) {
        printError(MUST_BE_COMMA, *t_iter);
    }

    auto type_iter = getNextLex(t_iter);
    if (!checkLexem(t_iter, type_tk)) {
        printError(MUST_BE_TYPE, *t_iter);
    }

    getNextLex(t_iter);
    if (!checkLexem(t_iter, semi_tk)) {
        printError(MUST_BE_SEMI, *t_iter);
    }

    updateVarTypes(var_list, type_iter->GetName());
    buildVarTree(var_list, t_tree);
    
    if (checkLexem(peekLex(1, t_iter), id_tk) || checkLexem(peekLex(1, t_iter), var_tk)) {
        if (checkLexem(peekLex(1, t_iter), var_tk))
            getNextLex(t_iter);
        vardpParse(t_iter, t_tree->GetRightNode());
    } else {
        t_tree->GetRightNode()->FreeRightNode();
    }
   
    return EXIT_SUCCESS;
}

Tree* Syntax::stateParse(lex_it& t_iter, int c_count) {
    Tree *result_tree = nullptr;
    auto iter = getNextLex(t_iter);
    switch (iter->GetToken()) {
    case id_tk: {
        if (!isVarExist(iter->GetName())) {
            printError(UNKNOWN_ID, *t_iter);
            return nullptr;
        }

        auto var_iter = iter;
        getNextLex(t_iter);
        if (!checkLexem(t_iter, ass_tk)) {
            printError(MUST_BE_ASS, *t_iter);
            return nullptr;
        }

        auto *tree_exp = Tree::CreateNode(t_iter->GetName());
        tree_exp->AddLeftNode(var_iter->GetName());

        expressionParse(t_iter, tree_exp);
        if (!checkLexem(t_iter, semi_tk)) { // we exit from expression on the ';'
            printError(MUST_BE_SEMI, *t_iter);
            return nullptr;
        }

        tree_exp->PrintTree(0);
        //tree_exp->PrintTree_2();

        result_tree = tree_exp;
        break;
    }
    case begin_tk: {
        auto *tree_comp = compoundParse(t_iter, c_count);
        if (!checkLexem(peekLex(1, t_iter), semi_tk)) {
            printError(MUST_BE_SEMI, *t_iter);
            return nullptr;
        }
        else getNextLex(t_iter);

        if (tree_comp != nullptr)
            result_tree = tree_comp;
        break;
    }
    // TODO: Add if/while/for statements
    default: {
        break;
    }
    }
    return result_tree;
}

Tree* Syntax::compoundParse(lex_it& t_iter, int c_count) {
    //static int compound_count = 0; 
    c_count++;
    int local_lvl = c_count; // save current compond level
    int sec_prm = 0;

    auto label = [&]() -> std::string {
        return "_*op" + std::to_string(local_lvl) + "." +
            std::to_string(sec_prm);
    };

    auto is_end = [&]() -> bool {
        return (checkLexem(peekLex(1, t_iter), end_tk)
            || checkLexem(peekLex(1, t_iter), eof_tk));
    };

    Tree *tree = Tree::CreateNode(t_iter->GetName()); // 'begin' node
    auto* root_compound_tree = tree; // save pointer of start of subtree

    while (t_iter->GetToken() != end_tk) {
        if (t_iter->GetToken() == eof_tk) {
            printError(EOF_ERR, *t_iter);
            return nullptr;
        }
        if (t_iter->GetToken() != dot_tk)
        {
            auto *subTree = stateParse(t_iter, c_count);
            if (subTree != nullptr) {
                tree->AddRightNode(label());
                tree->GetRightNode()->AddLeftTree(subTree);
                tree = tree->GetRightNode();

                if (!is_end()) sec_prm++;
            }
        }
        else break;
    }
    
    if (c_count == 1) {
        if (checkLexem(peekLex(1, t_iter), unknown_tk) ||
            checkLexem(peekLex(1, t_iter), eof_tk) ||
            !checkLexem(peekLex(1, t_iter), dot_tk)) {
            printError(MUST_BE_DOT, *t_iter);
            return nullptr;
        }
        tree->AddRightNode(t_iter->GetName() + ".");
    } else
        tree->AddRightNode(t_iter->GetName());
    return root_compound_tree;
}

int Syntax::expressionParse(lex_it& t_iter, Tree *tree) {
    lex_it var_iter;
    Tree *subTree;

    auto iter = getNextLex(t_iter);
    switch (iter->GetToken()) {
    case id_tk: {
        if (!isVarExist(iter->GetName()))
            printError(UNKNOWN_ID, *t_iter);
    }
    case constant_tk: { // like a := 3 ...
        var_iter = iter;
        subTree = simplExprParse(var_iter, t_iter, tree);
        break;
    }
    case minus_tk: { // like a := -3;

        if (getNextLex(t_iter)->GetToken() != constant_tk) {
            printError(MUST_BE_ID, *t_iter);
            return -EXIT_FAILURE;
        }

        var_iter = t_iter;
        subTree = simplExprParse(var_iter, t_iter, tree);
        break;
    }
    case cpb_tk:
        if (checkLexem(peekLex(1, t_iter), semi_tk)) {
            expressionParse(t_iter, tree);
        }
        else {
            simplExprParse(var_iter, t_iter, tree);
        }
        break;
    case opb_tk: { // like a := ( ... );
        expressionParse(t_iter, tree);
        break;
    }
    default: {
        printError(MUST_BE_ID, *t_iter);
        return -EXIT_FAILURE;
    }
    }

    return EXIT_SUCCESS;
}

Tree* Syntax::simplExprParse(const lex_it& var_iter, lex_it& t_iter, Tree* tree)
{
    Tree* subTree;
    auto iter = getNextLex(t_iter);
    switch (iter->GetToken()) {
    case plus_tk:
    case minus_tk:
    case mul_tk:
    case mod_tk:
    case div_tk: {
        if (operations.at(iter->GetName()) <=
            operations.at(tree->GetValue())) {       // Priority of current <=
            tree->AddRightNode(var_iter->GetName());
            subTree = tree->GetParentNode();

            while (operations.at(iter->GetName()) <= // go through parents
                   operations.at(subTree->GetValue()))
                subTree = subTree->GetParentNode();

            subTree = createLowestOpTree(subTree, iter->GetName());
        }
        else { // if Priority of current >
         /******* Create a new node of subexpression ************/
            tree->AddRightNode(iter->GetName());            //     <oper> <- subTree
            subTree = tree->GetRightNode();                 //      /  /
            subTree->AddLeftNode(var_iter->GetName());      //    val  nullptr
         /********************************************************/
        }
        expressionParse(t_iter, subTree);
        break;
    }
    default: { // any other lexem, expression is over
        tree->AddRightNode(var_iter->GetName());
        break;
    }
    }
    return tree;
}

void Syntax::printError(errors t_err, Lexem lex) {
    error = true;
    switch (t_err) {
    case UNKNOWN_LEXEM: {
        std::cerr << "<E> Lexer: Get unknown lexem '" << lex.GetName()
            << "' on " << lex.GetLine() << " line" << std::endl;
        break;
    }
    case EOF_ERR: {
        std::cerr << "<E> Syntax: Premature end of file" << std::endl;
        break;
    }
    case MUST_BE_ID: {
        std::cerr << "<E> Syntax: Must be identifier instead '" << lex.GetName()
            << "' on " << lex.GetLine() << " line" << std::endl;
        break;
    }
    case MUST_BE_SEMI: {
        std::cerr << "<E> Syntax: Must be ';' instead '" << lex.GetName()
            << "' on " << lex.GetLine() << " line" << std::endl;
        break;
    }
    case MUST_BE_PROG: {
        std::cerr << "<E> Syntax: Program must start from lexem 'program' ("
            << lex.GetLine() << ")" << std::endl;
        break;
    }
    case MUST_BE_COMMA: {
        std::cerr << "<E> Syntax: Must be ',' instead '" << lex.GetName()
            << "' on " << lex.GetLine() << " line" << std::endl;
        break;
    }
    case DUPL_ID_ERR: {
        std::cerr << "<E> Syntax: Duplicate identifier '" << lex.GetName()
            << "' on " << lex.GetLine() << " line" << std::endl;
        break;
    }
    case UNKNOWN_ID: {
        std::cerr << "<E> Syntax: Undefined variable '" << lex.GetName()
            << "' on " << lex.GetLine() << " line" << std::endl;
        break;
    }
    case MUST_BE_DOT: {
        std::cerr << "<E> Syntax: Program must be end by '.'" << std::endl;
        break;
    }
                    // TODO: Add remaining error types
    default: {
        std::cerr << "<E> Syntax: Undefined type of error" << std::endl;
        break;
    }
    }
}

bool Syntax::checkLexem(const lex_it& t_iter, const tokens& t_tok) {
    if (t_iter == lex_table.end())   return false;
    if (t_iter->GetToken() != t_tok) return false;

    return true;
}

bool Syntax::isVarExist(const std::string& t_var_name) {
    auto map_iter = id_map.find(t_var_name);
    return !(map_iter == id_map.end());
}

void Syntax::updateVarTypes(const std::list<std::string>& t_var_list, const std::string& t_type_name) {
    try {
        for (auto& el : t_var_list)
            id_map.at(el).type = t_type_name;
    }
    catch (const std::exception & exp) {
        std::cerr << "<E> Syntax: Catch exception in " << __func__ << ": "
            << exp.what() << std::endl;
    }
}

void Syntax::buildVarTree(const std::list<std::string>& t_var_list, Tree* t_tree) {
    try {
        auto i = 0;
        for (auto& el : t_var_list) {
            auto* tmp_tree = Tree::CreateNode(el);
            tmp_tree->AddRightNode(id_map.at(el).type);
            createVarTree(t_tree, tmp_tree, i++);
        }
    }
    catch (const std::exception & exp) {
        std::cerr << "<E> Syntax: Catch exception in " << __func__ << ": "
            << exp.what() << std::endl;
    }
}

void Syntax::createVarTree(Tree* t_tree, Tree* t_donor_tree, int lvl) {
    if (lvl > 0) {
        lvl--;
        createVarTree(t_tree->GetRightNode(), t_donor_tree, lvl);
    }
    else {
        t_tree->AddLeftTree(t_donor_tree);
        t_tree->AddRightNode("$");
    }
}

Tree* Syntax::createLowestOpTree(Tree* t_parent_tree, std::string value) {
    auto* lowest_tree = Tree::CreateNode(t_parent_tree, value);
    lowest_tree->AddLeftTree(t_parent_tree->GetRightNode());
    t_parent_tree->AddRightTree(lowest_tree);

    return lowest_tree;
}


