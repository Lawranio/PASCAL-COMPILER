#ifndef GENCODE_H
#define GENCODE_H

#include <algorithm>
#include <fstream>
#include <sstream>
#include <array>
#include "Tree.h"


class GenCode {
public:
    GenCode(Tree &&t_synt_tree);

    int GenerateAsm();

    virtual ~GenCode();
private:
    Tree         *synt_tree;
    std::ofstream code;
    std::ostringstream test_str;

    const std::array<std::string, 2> types  = {"integer", "boolean"};
    const std::array<std::string, 2> specif = {"array", "const"};

    static constexpr const char* DATA_SECT = ".data";
    static constexpr const char* BSS_SECT  = ".bss";

    static constexpr const char* TEXT_SECT = ".text";
    static constexpr const char* GLOB_SECT = ".globl main";
    static constexpr const char* MAIN_SECT = "main:";
    static constexpr const char* RET_SECT  = "ret";

    static constexpr const char* BYTE_TYPE = ".byte ";
    static constexpr const char* LONG_TYPE = ".long ";
    static constexpr const char* SPAC_TYPE = ".space ";

    static constexpr const char* LONG_SIZE = "4";
    static constexpr const char* BYTE_SIZE = "1";

    void addLine(std::string &&code_line);
    void buildLine(std::string &&code_line);

    int generateDeclVars();
    int generateInitVars(Tree *var_root);
    int generateDataVar (Tree *node);

    int generateUninitVars(Tree *var_root);
    int generateBssVaar   (Tree *node);

    void generateConstVars (Tree *var_root);

    void generateTextPart();
    void generateCompound();
    void generateExpressions();

    void generateLabel(const std::string &name, const std::string &type,
                       const std::string &val);
    void generateEnd();

    std::string getType(Tree *node);
    std::string getSpec(Tree *node);
    std::string getArraySize(Tree *spec_node, std::string type);

    bool checkType(const std::string &type);
    bool checkSpec(const std::string &spec);
    void clearBuffer();
};


#endif //GENCODE_H
