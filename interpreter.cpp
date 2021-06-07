#include "interpreter.h"
#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <iostream>

Interpeter::value_type Interpeter::number(std::string::iterator &_it) 
{
    value_type number = 0;
    while(isdigit(*_it)) {
        number = number * 10 + (*_it++ - 48);
    }
    if(*_it == '.') {
        ++_it;
        value_type divider = 10;
        while(isdigit(*_it)) {
            number += (*_it++ - 48) / divider;
            divider *= 10;
        }
    }
    return number;
}

Interpeter::value_type Interpeter::variative(std::string::iterator &_it) 
{
    if(isdigit(*_it))
        return number(_it);
    if(*_it == '-')
        return -variative(++_it);
    if(*_it == '(') {
        value_type res = additional(++_it);
        if(*_it != ')')
            invalidSyntax(_it);
        ++_it;
        return res;
    }
    if(!isalpha(*_it) && *_it != '_')
        invalidSyntax(_it);
    auto end = std::find_if(_it, expression.end(), [](auto c){ return !(isalpha(c) || isdigit(c) || c == '_'); });
    auto identifier = std::string(_it, end);
    bool isBegin = _it == expression.begin();
    bool isVarExists = isVariableExists(identifier);
    _it = end;
    if(*end == '=') {
        ++_it;
        if(!isVarExists && isBegin)
            createVariable(identifier, additional(_it));
        else
            setExistingValue(identifier, additional(_it));
    }
    else if(isBegin && identifier == expression)
        createVariable(identifier, 0); 
    else if(!isVarExists)
        noVariableError(identifier);
    return getExistingValue(identifier);
}

Interpeter::value_type Interpeter::multiplicative(std::string::iterator &_it) 
{
    value_type res = variative(_it);
    while(*_it == '/' || *_it == '*' || *_it == '%') {
        if(*_it == '/') {
            ++_it;
            auto num = variative(_it);
            
            res /= num;
        }
        else if(*_it++ == '*') {
            if(*_it == '*')
                res = pow(res, multiplicative(++_it));
            else
                res *= variative(_it);
        }
        else {
            auto num = variative(_it);
            if(num == 0)
                throw std::logic_error("Mod by zero");
            res = fmod(res, num); 
        }
    }
    return res;
}

Interpeter::value_type Interpeter::additional(std::string::iterator &_it) 
{
    value_type res = multiplicative(_it);
    while(*_it == '-' || *_it == '+') {
        if(*_it++ == '-') {
            res -= multiplicative(_it);
        }
        else {
            res += multiplicative(_it);
        }
    }
    return res;
}

Interpeter::value_type Interpeter::interpret(std::string _expression)
{ 
    expression = { _expression.begin(), std::remove(_expression.begin(), _expression.end(), ' ') };
    auto it = expression.begin();
    return expression.empty() ? 0 :  additional(it);
}

Interpeter::value_type Interpeter::interpret(const std::vector<std::string> &_code)
{
    auto it = _code.begin();
    while(it < _code.end() - 1) {
        interpret(*it++);
    }
    return it == _code.end() - 1 ? interpret(*it) : 0;
}

void Interpeter::interpret(const std::vector<std::string> &_code, std::vector<Interpeter::value_type> &_results)
{
    _results.resize(_code.size());
    for(size_t i = 0; i < _code.size(); ++i) {
        _results[i] = interpret(_code[i]);
    }
}


bool Interpeter::isVariableExists(const std::string &_identifier) const
{
    return variable.find(_identifier) != variable.end();
}

void Interpeter::createVariable(const std::string &_identifier, Interpeter::value_type _value)
{
    if(isVariableExists(_identifier))
        throwVariableAlreadyExists(_identifier);
    if(!isIdentifier(_identifier))
        invalidIdentifier(_identifier);
    variable[_identifier] = { _value, true };
}

void Interpeter::createVariable(const std::string &_identifier, const std::string &_expression)
{
    createVariable(_identifier, interpret(_expression));
}


Interpeter::value_type Interpeter::getValue(const std::string &_identifier) const
{
    if(!isVariableExists(_identifier))
        noVariableError(_identifier);
    return getExistingValue(_identifier);
}

Interpeter::value_type Interpeter::getExistingValue(const std::string &_identifier) const
{
    return variable.at(_identifier).value;
}

Interpeter::value_type Interpeter::setValue(const std::string &_identifier, Interpeter::value_type _value)
{ 
    throwIfNotExists(_identifier);
    return setExistingValue(_identifier, _value);
}

Interpeter::value_type Interpeter::setExistingValue(const std::string &_identifier, Interpeter::value_type _value)
{
    return variable[_identifier].value = _value;
}


bool Interpeter::isIdentifier(const std::string &_name)
{
    return  !_name.empty() && 
            (std::isalpha(_name.front()) || _name.front() == '_') && 
            std::find_if(_name.begin(), _name.end(), [](auto c){ return !(isalpha(c) || isdigit(c) || c == '_'); }) == _name.end();
}


//                          THROWS

void Interpeter::throwIfNotExists(const std::string &_identifier) const
{
    if(!isVariableExists(_identifier))
        noVariableError(_identifier);
}

void Interpeter::noVariableError(const std::string &_identifier)
{
    throw std::invalid_argument("No variable with name '" + _identifier + "'.");
}

void Interpeter::invalidSyntax(std::string::iterator &_it) const
{
    throw std::invalid_argument("Invalid syntax at index " + std::to_string(_it - expression.begin()) + ".");
}

void Interpeter::invalidIdentifier(const std::string &_identifier) 
{
    throw std::invalid_argument("Invalid vaariable identifier: " + _identifier + ".");
}

void Interpeter::throwVariableAlreadyExists(const std::string &_identifier) const 
{ 
    throw std::runtime_error("Variable with identifier '" + _identifier + "' already exists.");
}

//                          THROWS