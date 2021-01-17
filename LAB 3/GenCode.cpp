#include "GenCode.h"

GenCode::GenCode(Tree&& t_synt_tree) {
    try {
        synt_tree = &t_synt_tree;
        code.open(synt_tree->GetValue() + ".S", std::ios::out | std::ios::trunc);

        if (!code.is_open())
            throw;
    }
    catch (const std::exception& exp) {
        std::string what;
        std::string lel(exp.what());
        what = "<E> GenCode: Catch exception in constructor: ";

        throw std::runtime_error(what + lel);
    }
}


GenCode::~GenCode() {
    code.close();
    clearBuffer();
}


/**
 * @brief Generate GAS code by syntax tree of program
 * @param none
 *
 * @return  EXIT_SUCCESS - if code was generated successful
 * @return -EXIT_FAILURE - if can't generate code
 */
int GenCode::GenerateAsm() {
    try {
        if (synt_tree->GetLeftNode() == nullptr &&
            synt_tree->GetRightNode() == nullptr) {
            std::cerr << "<E> GenCode: Empty tree" << std::endl;
            return -EXIT_FAILURE;
        }

        if (synt_tree->GetLeftNode() != nullptr)
            generateDeclVars();

        if (synt_tree->GetRightNode() != nullptr) {
            generateTextPart();
        }

        generateEnd();
        return EXIT_SUCCESS;
    }
    catch (const std::exception& exp) {
        std::cerr << "<E> GenCode: Catch exception in " << __func__ << ": "
            << exp.what();
        return -EXIT_FAILURE;
    }
}


/**
 * @brief Add GAS construction in the file with assembler code
 * @param[in] code_line - line of GAS code
 *
 * @return none
 */
void GenCode::addLine(std::string&& code_line) {
    code << code_line << std::endl;
}


/**
 * @brief Add GAS construction in the stream buffer
 * @param[in] code_line - line of GAS code
 *
 * @return none
 * @note Then use this buffer for writing in the file with assembler code
 */
void GenCode::buildLine(std::string&& code_line) {
    test_str << code_line << std::endl;
}


/**
 * @brief Generate variables declaration for initialized and uninitialized variables
 * @param none
 *
 * @return  EXIT_SUCCESS - generate was successful
 * @return -EXIT_FAILURE - can't generate GAS code
 */
int GenCode::generateDeclVars() {
    auto ptr = synt_tree->GetLeftNode();//var

    if (ptr->GetValue() != "var") {
        std::cerr << "<E> GenCode: Can't find declaration of variables" << std::endl;
        return -EXIT_FAILURE;
    }

    while (ptr->GetRightNode() != nullptr) {

        if (ptr->GetLeftNode()->GetLeftNode() != nullptr) { //if initialized variable
            generateDataVar(ptr->GetLeftNode());

            if (!test_str.str().empty()) { // if we have any initialized variables
                addLine(DATA_SECT);
                addLine(test_str.str());
                clearBuffer();
            }

        }
        else { //if uninitialized variable

            generateBssVaar(ptr->GetLeftNode());

            if (!test_str.str().empty()) { // if we have any uninitialized variables
                addLine(BSS_SECT);
                addLine(test_str.str());
                clearBuffer();
            }
        }

        ptr = ptr->GetRightNode();
    }

    return EXIT_SUCCESS;
}

/**
 * @brief Parse tree and generate lines only for initialized variables
 * @param[in] node - node of syntax tree with variable
 *
 * @return  EXIT_SUCCESS - if node doesn't contain error
 * @return -EXIT_FAILURE - variable doesn't have a type
 * @note Method skips any other (uninit or specific) variables
 */
int GenCode::generateDataVar(Tree * node) {
    if (node->GetRightNode() == nullptr) {
        std::cerr << "<E> GenCode: Variable doesn't have a type node" << std::endl;
        return -EXIT_FAILURE;
    }

    if (!checkType(getType(node))) // get label variable
        return EXIT_SUCCESS;

    if (checkSpec(getSpec(node))) { // get specific variable
        if (getSpec(node) == specif.at(0)) {
            // XXX: if you implement array initialization in tree, add here
            //   generation of GAS code for array

            return EXIT_SUCCESS; // because array can't be initialized
        }
        else { // const
            return EXIT_SUCCESS;
        }
    }

    std::string val;

    if (node->GetLeftNode() != nullptr) {
        if ((node->GetLeftNode()->GetLeftNode() != nullptr) ||
            (node->GetLeftNode()->GetRightNode() != nullptr)) { //some a = 4+1+1+...
        /**code for it**/
        }
        else {
            val = node->GetLeftNode()->GetValue();

            if (getType(node) == types.at(1)) { // if type is boolean
                if (val == "false") val = "0";
                else                val = "1";
            }
        }



        std::string type = (getType(node) == types.at(0)) ? LONG_TYPE : BYTE_TYPE;
        generateLabel(node->GetValue(), type, val);
    }

    return EXIT_SUCCESS;
}

/**
 * @brief Parse tree and generate lines only for uninitialized variables
 * @param[in] node - node of syntax tree with variable
 *
 * @return  EXIT_SUCCESS - if node doesn't contain error
 * @return -EXIT_FAILURE - variable doesn't have a type
 * @note Method skips any other (init or specific) variables
 */
int GenCode::generateBssVaar(Tree * node) {
    //*node - name of variable
    if (node->GetRightNode() == nullptr) {
        std::cerr << "<E> GenCode: Variable doesn't have a type node" << std::endl;
        return -EXIT_FAILURE;
    }

    if (!checkType(getType(node)))
        return EXIT_SUCCESS;

    if (node->GetLeftNode() != nullptr && !checkSpec(getSpec(node)))
        return EXIT_SUCCESS;

    std::string val;

    if (getSpec(node) == specif.at(0))
        val = getArraySize(node->GetLeftNode(), getType(node));
    else
        val = (getType(node) == types.at(0)) ? LONG_SIZE : BYTE_SIZE;

    std::string type = SPAC_TYPE;
    generateLabel(node->GetValue(), type, val);
    return EXIT_SUCCESS;
}


/**
 * @brief Generate GAS code for constant variables
 * @param[in] var_root - pointer to the root of subtree of variables
 *
 * @return none
 * @note Work for code like:
 *   const a : integer = 5;
 *   const b = 1;
 *   const c : boolean = false;
 */
void GenCode::generateConstVars(Tree * var_root) {
    // for .set variables
}


/**
 * @brief Generate GAS code for code section (right node from root)
 * @param none
 *
 * @return none
 */
void GenCode::generateTextPart() {
    addLine(TEXT_SECT);
    addLine(GLOB_SECT);
    addLine(" ");
    addLine(MAIN_SECT);
    addLine(" ");
    addLine(EAX_ZERO);
    addLine(EBX_ZERO);
    addLine(" ");


    if (generateCompound(synt_tree->GetRightNode()->GetRightNode()))
        std::cerr << "<E> GenCode error in begin/end operation" << std::endl;
}


/**
 * @brief Generate GAS code for 'begin/end' operator
 * @param[in] Tree* node (*(begin))
 *
 * @return -EXIT_FAILURE/EXIT_SUCCESS
 */
int GenCode::generateCompound(Tree * node) {
    try {
        while (node->GetValue() != "end" && node->GetValue() != "end.") {

            if (node->GetValue() == "end" || node->GetValue() == "end.")
                return EXIT_SUCCESS;

            std::string st = node->GetValue() + ":";//print label
            addLine(st.data());
            addLine(" ");

            /*** if we have empty label in tree*///

            if (node->GetLeftNode() == nullptr) {
                node = node->GetRightNode();
                continue;
            }

            /****** operation if *******/
            if (node->GetLeftNode()->GetValue() == "if") { //operator if
                auto ptr = node->GetLeftNode();//ptr = *if

                if (ptr->GetLeftNode() == nullptr) {
                    std::cerr << "<E> GenCode: need condition in if" << std::endl;
                    throw std::out_of_range("error in if");
                }

                if (ptr->GetRightNode()->GetValue() != "then") {
                    std::cerr << "<E> GenCode: need then" << std::endl;
                    throw std::out_of_range("error in if");
                }

                /*if (ptr->GetLeftNode()->GetLeftNode() == nullptr) {
                    std::cerr << "<E> GenCode: need expression" << std::endl;
                    throw std::out_of_range("error in if");
                }*/

                num_if++;
                auto num = num_if;

                /***  left part if   ***/
                std::string str;
                //left part after > < <> =
                // if (a) then...

                if (ptr->GetLeftNode()->GetValue() == "true" ||
                    ptr->GetLeftNode()->GetValue() == "false" ||
                    (checkVariable(ptr->GetLeftNode()->GetValue()) != nullptr)) {

                    if (ptr->GetLeftNode()->GetValue() == "true") addLine("movl $1, %eax");
                    else if (ptr->GetLeftNode()->GetValue() == "false") addLine("movl $0, %eax");
                    else {
                        str = "movl " + ptr->GetLeftNode()->GetValue() + ", %eax";
                        addLine(str.data());
                    }
                    addLine("movl $0, %ebx");
                    addLine("cmp %ebx, %eax");
                    str = "jle ";

                }
                else {
                    generateAfterCondition(ptr->GetLeftNode()->GetLeftNode());
                    //left part in stack
                    generateAfterCondition(ptr->GetLeftNode()->GetRightNode());
                    //right part in stack
                    addLine("popl %ebx");
                    addLine("popl %eax");
                    addLine("cmp %ebx, %eax");

                    if (ptr->GetLeftNode()->GetValue() == ">") str = "jle ";
                    else if (ptr->GetLeftNode()->GetValue() == "<") str = "jge";
                    else if (ptr->GetLeftNode()->GetValue() == "=") str = "jgl";
                    else if (ptr->GetLeftNode()->GetValue() == "<>") str = "je";
                    else if (ptr->GetLeftNode()->GetValue() == ">=") str = "jl";//
                    else if (ptr->GetLeftNode()->GetValue() == "<=") str = "jg";//

                    else {
                        std::cerr << "Undefined condition";
                        throw std::out_of_range("error in if");
                    }
                }

                str += " _nope" + std::to_string(num) + "_";
                addLine(str.data());
                /***  right part if   ***/
                //ptr->GetRightNode() -- *then
                if (ptr->GetRightNode()->GetLeftNode() != nullptr)
                    generateThenElseExpr(ptr->GetRightNode());
                ////


                //after then
                if (ptr->GetRightNode()->GetRightNode() != nullptr) {
                    str = "jmp _end" + std::to_string(num) + "_";
                    addLine(str.data());
                }

                str = " _nope" + std::to_string(num) + "_:";
                addLine(str.data());

                if (ptr->GetRightNode()->GetRightNode() != nullptr) {
                    if (ptr->GetRightNode()->GetRightNode()->GetLeftNode() != nullptr) {
                        generateThenElseExpr(ptr->GetRightNode()->GetRightNode());
                    }
                    str = " _end" + std::to_string(num) + "_:";
                    addLine(str.data());
                }

                /****** operation goto *******/
            }
            else if (node->GetLeftNode()->GetValue() == "goto") {
                std::string str = "jmp " + node->GetLeftNode()->GetRightNode()->GetValue();
                addLine(str.data());

                /****** operation := *******/

            }
            else if (node->GetLeftNode()->GetValue() == ":=") {

                if ((checkVariable(node->GetLeftNode()->GetLeftNode()->GetValue())) ==
                    nullptr) { //if undefined variable
                    throw std::out_of_range("undefined variable");
                }

                if (node->GetLeftNode()->GetRightNode()->GetLeftNode() ==
                    nullptr) { //for d:=1 optimization(d:=value)

                    std::string str = "movl ";
                    (checkVariable(node->GetLeftNode()->GetRightNode()->GetValue()) == nullptr) ? str +=
                        "$" : "";
                    if (node->GetLeftNode()->GetRightNode()->GetValue() == "true") str += "1";
                    else if (node->GetLeftNode()->GetRightNode()->GetValue() == "false") str += "0";
                    else str += node->GetLeftNode()->GetRightNode()->GetValue();
                    str += ", " + node->GetLeftNode()->GetLeftNode()->GetValue();

                    addLine(str.data());

                }
                else {/***for d:= 1+2...(d:=expression)***/

                    generateExpressions(node->GetLeftNode()->GetRightNode());
                    addLine("popl %eax");
                    std::string str = "movl %eax, " + node->GetLeftNode()->GetLeftNode()->GetValue();
                    addLine(str.data());
                }

                /****** operation begin *******/

            }
            else if (node->GetLeftNode()->GetValue() == "begin") {

                if (generateCompound(node->GetLeftNode()->GetRightNode())) {
                    return -EXIT_FAILURE;
                }

            }
            else throw std::out_of_range("need some end for begin");

            node = node->GetRightNode();
        }

        return EXIT_SUCCESS;
    }
    catch (const std::exception& exp) {
        std::cerr << "<E> GenCode: Catch exception in " << __func__ << ": "
            << exp.what();
        return -EXIT_FAILURE;
    }
}

/**
 * @brief Generate Gas for int expresion
 * @param[in] tree* node (start of expresion)
 *
 * @return none
 * @note result left subtree in eax; right in ebx
 * result in stack;
 */
void GenCode::generateExpressions(Tree * node) {

    if (node->GetRightNode() == nullptr && node->GetLeftNode() == nullptr) {

        if (node->GetParentNode()->GetLeftNode() == node) {

            //push $12 or push a;
            std::string str = "pushl ";
            (checkVariable(node->GetValue()) == nullptr) ? str += "$" : "";
            if (node->GetValue() == "true") str += "1";
            else if (node->GetValue() == "false") str += "0";
            else str += node->GetValue();
            str += node->GetValue();
            addLine(str.data());

        }
        else {

            //movl $12, %ebx or movl a, %ebx;
            std::string str = "movl ";
            (checkVariable(node->GetValue()) == nullptr) ? str += "$" : "";
            if (node->GetValue() == "true") str += "1";
            else if (node->GetValue() == "false") str += "0";
            else str += node->GetValue();
            str += ", %ebx";
            addLine(str.data());

        }

        return;
    }

    if (node->GetLeftNode() != nullptr)
        generateExpressions(node->GetLeftNode());

    if (node->GetRightNode() != nullptr)
        generateExpressions(node->GetRightNode());

    if (node->GetRightNode() != nullptr) {
        if ((node->GetRightNode()->GetRightNode() != nullptr) &&
            (node->GetRightNode()->GetLeftNode() != nullptr))
            addLine("popl %ebx");
    }

    addLine("popl %eax");

    switch (GetOperation(node->GetValue())) {
    case 1:
        addLine("addl %ebx, %eax");
        break;

    case 2:
        addLine("subl  %ebx, %eax");
        break;

    case 3:
        addLine("xorl %edx, %edx");
        addLine("mull  %ebx");
        break;

    case 4:
        addLine("xorl %edx, %edx");
        addLine("divl  %ebx");
        break;

    case 5:
        addLine("andl %ebx, %eax");
        break;

    case 6:
        addLine("xorl %ebx, %eax");
        break;

    case 7:
        addLine("orl %ebx, %eax");
        break;

    default:
        throw std::out_of_range("invalid operation");
    }

    addLine("pushl %eax");
}

/**
 * @brief Converting string operation on int
 * @param[in] str - string with operation
 * @return int number operation ( 1:+; 2:-; 3:*; 4:div; 5:and; 6:xor; 7:or;
 * else exception
 */
int GenCode::GetOperation(const std::string str) {
    if (str == "+") return 1;

    if (str == "-") return 2;

    if (str == "*") return 3;

    if (str == "div") return 4;

    if (str == "and") return 5;

    if (str == "xor") return 6;

    if (str == "or") return 7;
    else return -1;
}
/**
 * @brief Check variable in var
 * @param[in] string variable (name of variable)
 * @return Tree* node if found, else nullptr
 */
Tree* GenCode::checkVariable(std::string && variable) {
    auto ptr = synt_tree->GetLeftNode();

    if (ptr == nullptr)
        return nullptr;

    while (ptr->GetRightNode() != nullptr) {
        if (ptr->GetLeftNode()->GetValue() == variable)
            return ptr->GetLeftNode();

        ptr = ptr->GetRightNode();
    }

    return nullptr;
}


/**
 * @brief Generate line with label (variable) information
 * @param[in] name - name of the label
 * @param[in] type - type of the label
 * @param[in] val  - value of the label
 *
 * @return none
 * @note For line of 'a : integer = 12' will be generate 'a: .long 12'
 */
void GenCode::generateLabel(const std::string & name, const std::string & type,
    const std::string & val) {
    std::string decl = name + ": " + type + val;
    buildLine(std::move(decl));
}


/**
 * @brief Generate 'ret' operator and add empty line in the end of file
 * @param none
 *
 * @return none
 */
void GenCode::generateEnd() {
    addLine(" ");
    addLine("leave");
    addLine(RET_SECT);
    addLine("");
}


/**
 * @brief Get type of variable from syntax tree
 * @param[in] node - node with variable
 *
 * @return "" - if doesn't have node with type
 * @return type of variable
 * @note       root
 *             / \
 *          var  begin
 *          / \     \
 * node -> a   $    ...
 *          \  ...
 *        <type>
 */
std::string GenCode::getType(Tree * node) {
    if (node->GetRightNode() == nullptr)
        return "";
    else
        return node->GetRightNode()->GetValue();
}


/**
 * @brief Get specific field of variable from syntax tree
 * @param[in] node - node with variable
 *
 * @return "" - if doesn't have node with specific (parameters)
 * @return parameter of variable
 * @note       root
 *             / \
 *          var  begin
 *          / \     \
 * node -> a   $    ...
 *        / \  ...
 *  <spec>  ...
 */
std::string GenCode::getSpec(Tree * node) {
    if (node->GetLeftNode() == nullptr)
        return "";
    else
        return node->GetLeftNode()->GetValue();
}


/**
 * @brief Get size of array
 * @param[in] spec_node - node of array field of variable
 * @param[in] type      - type of variable
 *
 * @return calculated size of array
 */
std::string GenCode::getArraySize(Tree * spec_node, std::string type) {
    int max = std::stoi(spec_node->GetRightNode()->GetValue());
    int min = std::stoi(spec_node->GetLeftNode()->GetValue());

    int type_size = (type == "integer") ? 4 : 1;
    return std::to_string((max - min + 1) * type_size);
}


/**
 * @brief Compare type of variable with know types
 * @param[in] type - type of variable
 *
 * @return true  - if type of variable is matched with known types
 * @return false - if doesn't match
 */
bool GenCode::checkType(const std::string & type) {
    auto res = std::find_if(types.begin(), types.end(), [&](const std::string& t) {
        return (t == type);
        });

    return res != types.end();
}


/**
 * @brief Compare specific field of variable with known specific types
 * @param[in] spec - specific field of variable
 *
 * @return true  - if specific field of variable is matched
 * @return false - if doesn't match
 */
bool GenCode::checkSpec(const std::string & spec) {
    auto res = std::find_if(specif.begin(), specif.end(), [&](const std::string& t) {
        return (t == spec);
        });

    return res != specif.end();
}


/**
 * @brief Clear inner string stream buffer
 * @param none
 *
 * @return none
 */
void GenCode::clearBuffer() {
    test_str.str("");
    test_str.clear();
}

void GenCode::generateAfterCondition(Tree * node) {
    if ((node->GetLeftNode() == nullptr) && //variable
        (node->GetRightNode() == nullptr)) {

        std::string str = "pushl ";
        (checkVariable(node->GetValue()) == nullptr) ? str += "$" : "";
        if (node->GetValue() == "true") str += "1";
        else if (node->GetValue() == "false") str += "0";
        else str += node->GetValue();
        addLine(str.data());
    }
    else {//expression
        generateExpressions(node);
    }
}

void GenCode::generateThenElseExpr(Tree * node) {

    /*** := in if ***/
    if (node->GetLeftNode()->GetValue() == ":=") {

        if (checkVariable((node->GetLeftNode()->GetLeftNode()->GetValue())) ==
            nullptr) {//if undefined variable
            throw std::out_of_range("undefined variable");
        }

        //node->GetLeftNode() -- *:=

        if (node->GetLeftNode()->GetRightNode()->GetLeftNode() ==
            nullptr) {//for d:=1 optimization(d:=value)

            std::string str = "movl ";
            (checkVariable(node->GetLeftNode()->GetRightNode()->GetValue()) == nullptr) ? str +=
                "$" : "";
            if (node->GetLeftNode()->GetRightNode()->GetValue() == "true") str += "1";
            else if (node->GetLeftNode()->GetRightNode()->GetValue() == "false") str += "0";
            else str += node->GetLeftNode()->GetRightNode()->GetValue();
            str += ", " + node->GetLeftNode()->GetLeftNode()->GetValue();
            addLine(str.data());

        }
        else {/***for d:= 1+2...(d:=expression)***/

            generateExpressions(node->GetLeftNode()->GetRightNode());
            addLine("popl %eax");
            std::string str = "movl %eax, " + node->GetLeftNode()->GetLeftNode()->GetValue();
            addLine(str.data());
        }

        /*** if in if ***/

    }
    else if (node->GetLeftNode()->GetValue() == "if") {
        num_if++;
        auto num = num_if;
        auto ptr = node->GetLeftNode();

        if (ptr->GetLeftNode() == nullptr) {
            std::cerr << "<E> GenCode: need condition in if" << std::endl;
            throw std::out_of_range("error in if");
        }

        if (ptr->GetRightNode()->GetValue() != "then") {
            std::cerr << "<E> GenCode: need then" << std::endl;
            throw std::out_of_range("error in if");
        }

        /*if (ptr->GetLeftNode()->GetLeftNode() == nullptr) {
            std::cerr << "<E> GenCode: need expression" << std::endl;
            throw std::out_of_range("error in if");
        }*/

        /***  left part if   ***/
        //left part after > < <> =

        std::string str;
        if (ptr->GetLeftNode()->GetValue() == "true" ||
            ptr->GetLeftNode()->GetValue() == "false" ||
            (checkVariable(ptr->GetLeftNode()->GetValue()) != nullptr)) {

            if (ptr->GetLeftNode()->GetValue() == "true") addLine("movl $1, %eax");
            else if (ptr->GetLeftNode()->GetValue() == "false") addLine("movl $0, %eax");
            else {
                str = "movl " + ptr->GetLeftNode()->GetValue() + ", %eax";
                addLine(str.data());
            }
            addLine("movl $0, %ebx");
            addLine("cmp %ebx, %eax");
            str = "jle ";

        }
        else {
            generateAfterCondition(ptr->GetLeftNode()->GetLeftNode());
            //left part in stack
            generateAfterCondition(ptr->GetLeftNode()->GetRightNode());
            //right part in stack
            addLine("popl %ebx");
            addLine("popl %eax");
            addLine("cmp %ebx, %eax");
            std::string str;

            if (ptr->GetLeftNode()->GetValue() == ">") str = "jle ";
            else if (ptr->GetLeftNode()->GetValue() == "<") str = "jge";
            else if (ptr->GetLeftNode()->GetValue() == "=") str = "jgl";
            else if (ptr->GetLeftNode()->GetValue() == "<>") str = "je";
            else if (ptr->GetLeftNode()->GetValue() == ">=") str = "jl";//
            else if (ptr->GetLeftNode()->GetValue() == "<=") str = "jg";//

            else {
                std::cerr << "Undefined condition";
                throw std::out_of_range("error in if");
            }
        }

        str += " _nope" + std::to_string(num) + "_";
        addLine(str.data());
        /***  right part if   ***/
        //ptr->GetRightNode() -- *then
        if (ptr->GetRightNode()->GetLeftNode() != nullptr)
            generateThenElseExpr(ptr->GetRightNode());

        ////
        //after then
        if (ptr->GetRightNode()->GetRightNode() != nullptr) {
            str = "jmp _end" + std::to_string(num) + "_";
            addLine(str.data());
        }

        str = " _nope" + std::to_string(num) + "_:";
        addLine(str.data());

        if (ptr->GetRightNode()->GetRightNode() != nullptr) {
            if (ptr->GetRightNode()->GetRightNode()->GetLeftNode() != nullptr) {
                generateThenElseExpr(ptr->GetRightNode()->GetRightNode());
            }
            str = " _end" + std::to_string(num) + "_:";
            addLine(str.data());
        }

        /*** goto in if ***/

    }
    else if (node->GetLeftNode()->GetValue() == "goto") {
        std::string str = "jmp " + node->GetLeftNode()->GetRightNode()->GetValue();
        addLine(str.data());
    }
    else if (node->GetLeftNode()->GetValue() == "begin") {
        /*** begin end ***/
        generateCompound(node->GetLeftNode()->GetRightNode());//for begin/end
    }
    else {
        std::cerr << "<E> GenCode in if can be :=/if/goto or begin/end" << std::endl;
        throw std::out_of_range("");
    }
}