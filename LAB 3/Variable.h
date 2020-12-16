#ifndef VARIABLE_H
#define VARIABLE_H

#include <string>

class Variable {
public:
	explicit Variable(const std::string& t_type, const std::string& t_value) : type(t_type), value(t_value) {};
	~Variable() = default;
	std::string type;
	std::string value;
};

#endif // !VARIABLE_H

