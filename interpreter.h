#pragma once 
#include <map>
#include <string>
#include <vector>

struct Interpeter
{
public:
    using value_type = double;
    struct Identifier
    {
        value_type  value   { };
        bool    exists  { false };
    };
private:
    std::map<std::string, Identifier> variable {};
    std::string expression {};

    //  Processes numbers
    value_type number(std::string::iterator &_it);

    //  Processes "(...)", variables; 
    //  Throws: Invalid syntax, Variable already exists, Variable doesn't exists, Invalid Identifier;
    value_type variative(std::string::iterator &_it);    

    //  Processes '*', '/', '**', '%';
    //  Throws: see value_type variative(std::string::iterator &_it);
    value_type multiplicative(std::string::iterator &_it);    

    //  Processes '+', '-';
    //  Throws: see value_type multiplicative(std::string::iterator &_it);
    value_type additional(std::string::iterator &_it);          
    
    
    static void noVariableError(const std::string &_identifier);
    static void invalidIdentifier(const std::string &_identifier);
    void invalidSyntax(std::string::iterator &_it) const;   
    void throwIfNotExists(const std::string &_identifier) const;
    void throwVariableAlreadyExists(const std::string &_identifier) const;

    value_type getExistingValue(const std::string &_identifier) const;
    value_type setExistingValue(const std::string &_identifier, value_type _value);

public:
    value_type interpret(std::string _expression);
    value_type interpret(const std::vector<std::string> &_expression);
    void interpret(const std::vector<std::string> &_expression, std::vector<value_type> &_results);

    bool isVariableExists(const std::string &_identifier) const;

    void createVariable(const std::string &_identifier, value_type _value = value_type());
    void createVariable(const std::string &_identifier, const std::string &_expression);

    value_type getValue(const std::string &_identifier) const;
    value_type setValue(const std::string &_identifier, value_type _value);
    

    static bool isIdentifier(const std::string &_name);
};

