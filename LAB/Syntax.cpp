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

    operations.emplace("(", 4);
    operations.emplace(")", 4);
}

Syntax::~Syntax() {
    Tree::FreeTree(root_tree);
}


int Syntax::ParseCode() {
    std::cout << "Code contains " << lex_table.size() << " lexemes" << std::endl;
    auto& it = cursor;
    if (programParse(it) != 0)
        return -EXIT_FAILURE;

    while (it != lex_table.end() && it->GetToken() != eof_tk)
        blockParse(it);

    if (!error) {
        std::cout << std::endl;
        std::cout << std::setfill('*') << std::setw(50);
        std::cout << "\r\n";

        root_tree->PrintTree(0);
    }
    
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

Syntax::lex_it Syntax::getPrevLex(lex_it& iter) {
    try {
        if (iter != lex_table.begin())
            iter--;
    }
    catch (const std::exception& exp) {
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

Syntax::lex_it Syntax::peekPrevLex(int N, lex_it t_iter) {
    try {
        auto iter = t_iter;
        while (iter != lex_table.end()) {
            if (N == 0) return iter;
            iter--; N--;
        }

        return iter;
    }
    catch (const std::exception&) {
        std::cerr << "<E> Syntax: Can't peek so backward" << std::endl;
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
    auto* tree_value = Tree::CreateNode("");
    bool isArray{ false };

    if (!checkLexem(t_iter, ddt_tk)) {
        printError(MUST_BE_COMMA, *t_iter);
    }

    auto type_iter = getNextLex(t_iter);

    if (t_iter->GetToken() == arr_tk) {
        tree_value->ChangeValue(t_iter->GetName());
        tree_value->AddLeftNode("range");
        getNextLex(t_iter);

        if (!checkLexem(t_iter, osb_tk)) {
            printError(MUST_BE_ARRBRACKET, *t_iter);
        }

        getNextLex(t_iter);

        if (!checkLexem(t_iter, constant_tk)) {
            printError(MUST_BE_ID, *t_iter);
        }

        tree_value->GetLeftNode()->AddLeftNode(t_iter->GetName());
        getNextLex(t_iter);

        if (!checkLexem(t_iter, dot_tk)) {
            printError(MUST_BE_DOT, *t_iter);
        }

        getNextLex(t_iter);
        tree_value->GetLeftNode()->AddRightNode(t_iter->GetName());
        getNextLex(t_iter);

        if (!checkLexem(t_iter, csb_tk)) {
            printError(MUST_BE_ARRBRACKET_END, *t_iter);
        }

        getNextLex(t_iter);

        if (!checkLexem(t_iter, of_tk)) {
            printError(MUST_BE_OF, *t_iter);
        }

        type_iter = getNextLex(t_iter);
        isArray = true;
    }


    if (!checkLexem(t_iter, type_tk)) {
        printError(MUST_BE_TYPE, *t_iter);
    }

    getNextLex(t_iter);
    if (!checkLexem(t_iter, semi_tk)) {
        printError(MUST_BE_SEMI, *t_iter);
    }

    if (isArray) {
        std::pair<int, int> range = { std::stoi(tree_value->GetLeftNode()->GetLeftNode()->GetValue()),
                                        std::stoi(tree_value->GetLeftNode()->GetRightNode()->GetValue()) };
        updateVarTypes(var_list, type_iter->GetName(), range);
    }
    else {
        updateVarTypes(var_list, type_iter->GetName());
    }

    if (isArray) {
        while (t_tree->GetLeftNode() != nullptr)
            t_tree = t_tree->GetRightNode();
        buildVarTree(var_list, t_tree, tree_value);
    }
    else {
        if (t_tree->GetValue() == "var") {
            while (t_tree->GetLeftNode() != nullptr)
                t_tree = t_tree->GetRightNode();
            buildVarTree(var_list, t_tree);
        }
        else {
            while (t_tree->GetLeftNode() != nullptr)
                t_tree = t_tree->GetRightNode();
            buildVarTree(var_list, t_tree);
        }
        Tree::FreeTree(tree_value);
    }

    
    if (checkLexem(peekLex(1, t_iter), id_tk) || checkLexem(peekLex(1, t_iter), var_tk)) {
        if (checkLexem(peekLex(1, t_iter), var_tk))
            getNextLex(t_iter);
        vardpParse(t_iter, t_tree->GetRightNode());
    } else {
        if (t_tree->GetRightNode()->GetRightNode())
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

        expressionParse(t_iter, tree_exp, 0);

        if (!checkLexem(t_iter, semi_tk)) { // we exit from expression on the ';'
            printError(MUST_BE_SEMI, *t_iter);
            return nullptr;
        }

        //tree_exp->PrintTree(0);
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
    case if_tk: {
        break;
    }
    case while_tk: {
        break;
    }
    case for_tk: {
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
    int local_lvl = c_count; // save current compound level
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

int Syntax::expressionParse(lex_it& t_iter, Tree *tree, int t_lvl) {
    lex_it var_iter;
    Tree *subTree;
    

    auto iter = getNextLex(t_iter);
    switch (iter->GetToken()) {
    case id_tk: { // like a := b;
        if (!isVarExist(iter->GetName()))
            printError(UNKNOWN_ID, *t_iter);
        var_iter = iter;;
        subTree = simplExprParse(var_iter, t_iter, tree, t_lvl);
        break;
    }
    case constant_tk: { // like a := 3 ...
        var_iter = iter;
        subTree = simplExprParse(var_iter, t_iter, tree, t_lvl);
        break;
    }
    case minus_tk: { // like a := -3;
        var_iter = t_iter;
        
        if (getNextLex(t_iter)->GetToken() != constant_tk && !checkLexem(t_iter, id_tk)) {
            printError(MUST_BE_ID, *t_iter);
            return -EXIT_FAILURE;
        }

        tree->AddRightNode(var_iter->GetName());
        tree->GetRightNode()->AddLeftNode("0");
        var_iter = t_iter;
        subTree = simplExprParse(var_iter, t_iter, tree->GetRightNode(), t_lvl);

        break;
    }
    case opb_tk: {
        t_lvl += 3;
        expressionParse(t_iter, tree, t_lvl);
        break;
    }
    case cpb_tk: {
        if (getNextLex(t_iter)->GetToken() != semi_tk) {
            t_lvl -= 3;;
            t_iter = getPrevLex(iter);
            lex_table.erase(getNextLex(iter));
            getPrevLex(t_iter);
            expressionParse(t_iter, tree, t_lvl);
        }
        else {
            t_lvl -= 3;;
            var_iter = getPrevLex(iter);
            t_iter = var_iter;
            getNextLex(iter);
            lex_table.erase(iter);
            simplExprParse(var_iter, t_iter, tree, t_lvl);
        }
        break;
    }
    case semi_tk: {
        if (t_lvl > 0) {
            printError(MUST_BE_BRACKET_END, *t_iter);
            return -EXIT_FAILURE;
        }
        if (t_lvl < 0) {
            printError(MUST_BE_BRACKET, *t_iter);
            return -EXIT_FAILURE;
        }
        break;
    }
    default: {
        printError(MUST_BE_ID, *t_iter);
        return -EXIT_FAILURE;
    }
    }

    return EXIT_SUCCESS;
}

Tree* Syntax::simplExprParse(const lex_it& var_iter, lex_it& t_iter, Tree* tree, int t_lvl)
{
    Tree* subTree;
    auto iter = getNextLex(t_iter);
    switch (iter->GetToken()) {
    case plus_tk:
    case minus_tk:
    case mul_tk:
    case div1_tk:
    case div2_tk: {
        if (operations.at(iter->GetName()) + t_lvl <= (tree->GetPriority())) {    // Priority of current <=
            tree->AddRightNode(var_iter->GetName());
            subTree = tree->GetParentNode();

            while (operations.at(iter->GetName()) + t_lvl <= operations.at(subTree->GetValue())) // go through parents
                subTree = subTree->GetParentNode();

            subTree = createLowestOpTree(subTree, iter->GetName(), operations.at(iter->GetName()) + t_lvl);
        }
        else { // if Priority of current >
         /******* Create a new node of subexpression ************/
            tree->AddRightNode(iter->GetName(), operations.at(iter->GetName()) + t_lvl);   //     <oper> <- subTree
            subTree = tree->GetRightNode();                                                //      /  /
            subTree->AddLeftNode(var_iter->GetName());                                     //    val  nullptr
         /********************************************************/
        }
        expressionParse(t_iter, subTree, t_lvl);
        break;
    }
    default: { // any other lexem, expression is over
        if (iter->GetToken() == cpb_tk) {
            getPrevLex(t_iter);
            expressionParse(t_iter, tree, t_lvl);
        }
        else {
            if (t_lvl != 0)
            {
                getPrevLex(t_iter);
                expressionParse(t_iter, tree, t_lvl);
            }
            tree->AddRightNode(var_iter->GetName());
        }
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
    case MUST_BE_BRACKET: {
        std::cerr << "<E> Syntax: Missing '(' in expression on " 
            << lex.GetLine() << " line" << std::endl;
        break;
    }
    case MUST_BE_BRACKET_END: {
        std::cerr << "<E> Syntax: Must be ')' instead '" << lex.GetName()
            << "' on " << lex.GetLine() << " line" << std::endl;
        break;
    }
    case MUST_BE_ARRBRACKET: {
        std::cerr << "<E> Syntax: Must be '[' instead '" << lex.GetName()
            << "' on " << lex.GetLine() << " line" << std::endl;
        break;
    }
    case MUST_BE_ARRBRACKET_END: {
        std::cerr << "<E> Syntax: Must be ']' instead '" << lex.GetName()
            << "' on " << lex.GetLine() << " line" << std::endl;
        break;
    }
    case MUST_BE_DO: {
        std::cerr << "<E> Syntax: Must be 'do' instead '" << lex.GetName()
            << "' on " << lex.GetLine() << " line" << std::endl;
        break;
    }
    case MUST_BE_TO: {
        std::cerr << "<E> Syntax: Must be 'to' instead '" << lex.GetName()
            << "' on " << lex.GetLine() << " line" << std::endl;
        break;
    }
    case MUST_BE_THEN: {
        std::cerr << "<E> Syntax: Must be 'then' instead '" << lex.GetName()
            << "' on " << lex.GetLine() << " line" << std::endl;
        break;
    }
    case MUST_BE_OF: {
        std::cerr << "<E> Syntax: Must be 'of' instead '" << lex.GetName()
            << "' on " << lex.GetLine() << " line" << std::endl;
        break;
    }
    case MUST_BE_TYPE: {
        std::cerr << "<E> Syntax: Must be type instead '" << lex.GetName()
            << "' on " << lex.GetLine() << " line" << std::endl;
        break;
    }
    case MUST_BE_ASS: {
        std::cerr << "<E> Syntax: Must be ':=' instead '" << lex.GetName()
            << "' on " << lex.GetLine() << " line" << std::endl;
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

void Syntax::updateVarTypes(const std::list<std::string>& t_var_list, const std::string& t_type_name,
                                const std::pair<int, int>& range) {
    try {
        for (auto& el : t_var_list) {
            id_map.at(el).type = t_type_name;
            id_map.at(el).isarray = true;
            id_map.at(el).range.first = range.first;
            id_map.at(el).range.second = range.second;
        }
    }
    catch (const std::exception& exp) {
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

void Syntax::buildVarTree(const std::list<std::string>& t_var_list, Tree* t_tree, Tree* array_tree) {
    try {
        auto i = 0;
        
        for (auto& el : t_var_list) {
            auto* tmp_tree = Tree::CreateNode(el);
            tmp_tree->AddRightTree(array_tree);
            array_tree->AddRightNode(id_map.at(el).type, 0);
            createVarTree(t_tree, tmp_tree, i++);
        }
    }
    catch (const std::exception& exp) {
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

Tree* Syntax::createLowestOpTree(Tree* t_parent_tree, std::string value, int priority_) {
    auto* lowest_tree = Tree::CreateNode(t_parent_tree, value, priority_);
    lowest_tree->AddLeftTree(t_parent_tree->GetRightNode());
    t_parent_tree->AddRightTree(lowest_tree);

    return lowest_tree;
}


