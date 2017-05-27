#ifndef _FOL_H
#define _FOL_H

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include <sstream>
#include <algorithm>
#include <exception>

#define MAX 100

using namespace std;

typedef string FunctionSymbol;
typedef string PredicateSymbol;
typedef string Variable;

enum axis{
    X,
    Y
};

static string apendAxis(axis a){
    if(a == X)
        return "_x";
    else
        return "_y";
}

enum optimizationLevel{
    notOptimized = 0,
    halfOptimized = 1,
    optimized = 2
};

struct extendedString {
    extendedString(){}

    extendedString(string _varName, optimizationLevel l = notOptimized)
        :varName(_varName), level(l)
    {}

    string varName;
    optimizationLevel level;

    const string str(axis a){
        switch(level){
        case notOptimized:
            return varName + apendAxis(a);
        case optimized:
            return "0";
        case halfOptimized:
            if(a == X)
                return "0";
            else
                return varName + apendAxis(a);
        }

        return "";
    }
};

enum relation {
    collinear,
    parallel,
    perpendicular,
    lengths_eq,
    is_midpoint,
    is_intersection,
    is_equal
};

static relation getRelation(string s) {
    if(s == "collinear")
        return collinear;
    else if(s == "parallel")
        return parallel;
    else if(s == "perpendicular")
        return perpendicular;
    else if(s == "lengths_eq")
        return lengths_eq;
    else if(s == "is_midpoint")
        return is_midpoint;
    else if(s == "is_intersection")
        return is_intersection;
    else //if(s == "is_equal")
        return is_equal;
}

static string openExp = "<<";
static string closeExp = ">>";

static string toCoordinateLanguage(relation rel, vector<extendedString>& points){

    stringstream ss;
    ss << openExp;

    switch(rel){
    case collinear:
        // "<<(0_x - 1_x) * (1_y - 2_y) = (0_y - 1_y) * (1_x - 2_x)>>"
        ss << "(" << points[0].str(X) << " - " << points[1].str(X) << ") * ("
                  << points[1].str(Y) << " - " << points[2].str(Y) << ") = ("
                  << points[0].str(Y) << " - " << points[1].str(Y) << ") * ("
                  << points[1].str(X) << " - " << points[2].str(X) << ")";
        break;
    case parallel:
        // "<<(0_x - 1_x) * (2_y - 3_y) = (0_y - 1_y) * (2_x - 3_x)>>"
        ss << "(" << points[0].str(X) << " - " << points[1].str(X) << ") * ("
                  << points[2].str(Y) << " - " << points[3].str(Y) << ") = ("
                  << points[0].str(Y) << " - " << points[1].str(Y) << ") * ("
                  << points[2].str(X) << " - " << points[3].str(X) << ")";
        break;
    case perpendicular:
        // "<<(0_x - 1_x) * (2_x - 3_x) + (0_y - 1_y) * (2_y - 3_y) = 0>>"
        ss << "(" << points[0].str(X) << " - " << points[1].str(X) << ") * ("
                  << points[2].str(X) << " - " << points[3].str(X) << ") + ("
                  << points[0].str(Y) << " - " << points[1].str(Y) << ") * ("
                  << points[2].str(Y) << " - " << points[3].str(Y) << ") = 0";
        break;
    case lengths_eq:
        // "<<(0_x - 1_x)^2 + (0_y - 1_y)^2 = (2_x - 3_x)^2 + (2_y - 3_y)^2>>"
        ss << "(" << points[0].str(X) << " - " << points[1].str(X) << ")^2 + ("
                  << points[0].str(Y) << " - " << points[1].str(Y) << ")^2 = ("
                  << points[2].str(X) << " - " << points[3].str(X) << ")^2 + ("
                  << points[2].str(Y) << " - " << points[3].str(Y) << ")^2";
        break;
    case is_midpoint:
        // "<<2 * 0_x = 1_x + 2_x & 2 * 0_y = 1_y + 2_y>>"
        ss << "2 * " << points[0].str(X) << " = "
           << points[1].str(X) << " + " << points[2].str(X) << " & "
           << "2 * " << points[0].str(Y) << " = "
           << points[1].str(Y) << " + " << points[2].str(Y);
        break;
    case is_intersection:
        // "<<(0_x - 1_x) * (1_y - 2_y) = (0_y - 1_y) * (1_x - 2_x) & (0_x - 3_x) * (3_y - 4_y) = (0_y - 3_y) * (3_x - 4_x)>>"
        ss << "(" << points[0].str(X) << " - " << points[1].str(X) << ") * ("
                  << points[1].str(Y) << " - " << points[2].str(Y) << ") = ("
                  << points[0].str(Y) << " - " << points[1].str(Y) << ") * ("
                  << points[1].str(X) << " - " << points[2].str(X) << ") & ("
                  << points[0].str(X) << " - " << points[3].str(X) << ") * ("
                  << points[3].str(Y) << " - " << points[4].str(Y) << ") = ("
                  << points[0].str(Y) << " - " << points[3].str(Y) << ") * ("
                  << points[3].str(X) << " - " << points[4].str(X) << ")";
        break;
    case is_equal:
        // "<<(0_x = 1_x) & (0_y = 1_y)>>"
        ss << "(" << points[0].str(X) << " = " << points[1].str(X) << ") & ("
                  << points[0].str(Y) << " = " << points[1].str(Y) << ")";
        break;
    }

    ss << closeExp;
    return ss.str();
}

static extendedString extend(string varName,
                             bool optimizationIndicator,
                             string& optimizedVar,
                             string& halfOptimizedVar)
{
    if(optimizationIndicator && optimizedVar.size() == 0){
        optimizedVar = varName;
    }

    if(optimizationIndicator && optimizedVar.size() > 0 &&
            varName != optimizedVar && halfOptimizedVar.size() == 0){
        halfOptimizedVar = varName;
    }

    optimizationLevel level = notOptimized;
    if(varName == optimizedVar){
        level = optimized;
    }
    else if (varName == halfOptimizedVar){
        level = halfOptimized;
    }

    return extendedString(varName, level);
}

class BaseTerm;
typedef shared_ptr<BaseTerm> Term;


class BaseTerm : public enable_shared_from_this<BaseTerm> {

public:
    enum Type { TT_VARIABLE, TT_FUNCTION };
    virtual Type getType() const = 0;
    virtual void printTerm(ostream & ostr) const = 0;
    virtual ~BaseTerm() {}
};

class VariableTerm : public BaseTerm {
private:
    Variable _v;
public:
    VariableTerm(const Variable & v)
        :_v(v)
    {}

    virtual Type getType() const
    {
        return TT_VARIABLE;
    }

    const Variable & getVariable() const
    {
        return _v;
    }
    virtual void printTerm(ostream & ostr) const
    {
        ostr << _v;
    }
};

class FunctionTerm : public BaseTerm {
private:
    FunctionSymbol _f;
    vector<Term> _ops;

public:
    FunctionTerm(const FunctionSymbol & f,
                 const vector<Term> & ops = vector<Term> ())
        :_f(f),
          _ops(ops)
    {}

    virtual Type getType() const
    {
        return TT_FUNCTION;
    }

    const FunctionSymbol & getSymbol() const
    {
        return _f;
    }

    const vector<Term> & getOperands() const
    {
        return _ops;
    }

    virtual void printTerm(ostream & ostr) const
    {
        ostr << _f;

        for(unsigned i = 0; i < _ops.size(); i++)
        {
            if(i == 0)
                ostr << "(";
            _ops[i]->printTerm(ostr);
            if(i != _ops.size() - 1)
                ostr << ",";
            else
                ostr << ")";
        }
    }
};

class BaseFormula;
typedef shared_ptr<BaseFormula> Formula;

class BaseFormula : public enable_shared_from_this<BaseFormula> {

public:

    enum Type { T_TRUE, T_FALSE, T_ATOM, T_NOT,
                T_AND, T_OR, T_IMP, T_IFF, T_FORALL, T_EXISTS };

    void doTheMagic(bool optInd, ostream& ostr){
        optimizedVar = "";
        halfOptimizedVar = "";

        optimizationIndicator = optInd;
        printConvertedFormula(ostr);
        ostr << endl;
    }

    virtual void printConvertedFormula(ostream & ostr) = 0;
    virtual Type getType() const = 0;
    virtual ~BaseFormula() {}

    bool optimizationIndicator;

    string optimizedVar;
    string halfOptimizedVar;
};



class AtomicFormula : public BaseFormula {
public:
};


class LogicConstant : public AtomicFormula {

public:
};


class True : public LogicConstant {

public:
    virtual void printConvertedFormula(ostream & ostr)
    {
        ostr << "true";
    }

    virtual Type getType() const
    {
        return T_TRUE;
    }

};


class False : public LogicConstant {

public:
    virtual void printConvertedFormula(ostream & ostr)
    {
        ostr << "false";
    }

    virtual Type getType() const
    {
        return T_FALSE;
    }
};



class Atom : public AtomicFormula {
protected:
    PredicateSymbol _p;
    vector<Term> _ops;

public:
    Atom(const PredicateSymbol & p,
         const vector<Term> & ops = vector<Term>())
        :_p(p),
          _ops(ops)
    {}

    const PredicateSymbol & getSymbol() const
    {
        return _p;
    }

    const vector<Term> & getOperands() const
    {
        return _ops;
    }

    virtual void printConvertedFormula(ostream & ostr)
    {
        vector<extendedString> extendedOps;
        for (auto it : _ops)
               extendedOps.push_back(extend(
                                            ((FunctionTerm*)(it.get()))->getSymbol(),
                                            optimizationIndicator,
                                            optimizedVar,
                                            halfOptimizedVar));

        ostr << toCoordinateLanguage(getRelation(_p), extendedOps);
    }

    virtual Type getType() const
    {
        return T_ATOM;
    }
private :
    static std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
        size_t start_pos = 0;
        while((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
        }
        return str;
    }
};

class Equality : public Atom {
public:
    Equality(const Term & lop, const Term & rop)
        :Atom("=", vector<Term> ())
    {
        _ops.push_back(lop);
        _ops.push_back(rop);
    }

    const Term & getLeftOperand() const
    {
        return _ops[0];
    }

    const Term & getRightOperand() const
    {
        return _ops[1];
    }

    virtual void printConvertedFormula(ostream & ostr)
    {
        _ops[0]->printTerm(ostr);
        ostr << " = ";
        _ops[1]->printTerm(ostr);
    }
};

class Disequality : public Atom {
public:
    Disequality(const Term & lop, const Term & rop)
        :Atom("~=", vector<Term> ())
    {
        _ops.push_back(lop);
        _ops.push_back(rop);
    }

    const Term & getLeftOperand() const
    {
        return _ops[0];
    }

    const Term & getRightOperand() const
    {
        return _ops[1];
    }

    virtual void printConvertedFormula(ostream & ostr)
    {

        _ops[0]->printTerm(ostr);
        ostr << " ~= ";
        _ops[1]->printTerm(ostr);
    }
};


class UnaryConjective : public BaseFormula {
protected:
    Formula _op;
public:
    UnaryConjective(const Formula & op)
        :_op(op)
    {}

    const Formula & getOperand() const
    {
        return _op;
    }
};

class Not : public UnaryConjective {
public:
    Not(const Formula & op)
        :UnaryConjective(op)
    {}

    virtual void printConvertedFormula(ostream & ostr)
    {
        ostr << "~";
        Type op_type = _op->getType();

        if(op_type == T_AND || op_type == T_OR ||
                op_type == T_IMP || op_type == T_IFF)
            ostr << "(";

        _op->printConvertedFormula(ostr);

        if(op_type == T_AND || op_type == T_OR ||
                op_type == T_IMP || op_type == T_IFF)
            ostr << ")";
    }

    virtual Type getType() const
    {
        return T_NOT;
    }
};


class BinaryConjective : public BaseFormula {
protected:
    Formula _op1, _op2;
public:
    BinaryConjective(const Formula & op1, const Formula & op2)
        :_op1(op1),
          _op2(op2)
    {}

    const Formula & getOperand1() const
    {
        return _op1;
    }

    const Formula & getOperand2() const
    {
        return _op2;
    }
};


class And : public BinaryConjective {
public:
    And(const Formula & op1, const Formula & op2)
        :BinaryConjective(op1, op2)
    {}

    virtual void printConvertedFormula(ostream & ostr)
    {
        Type op1_type = _op1->getType();
        Type op2_type = _op2->getType();

        if(op1_type == T_OR || op1_type == T_IMP ||
                op1_type == T_IFF)
            ostr << "(";

        _op1->printConvertedFormula(ostr);

        if(op1_type == T_OR || op1_type == T_IMP ||
                op1_type == T_IFF)
            ostr << ")";

        ostr << " & ";

        if(op2_type == T_OR || op2_type == T_IMP ||
                op2_type == T_IFF || op2_type == T_AND)
            ostr << "(";

        _op2->printConvertedFormula(ostr);

        if(op2_type == T_OR || op2_type == T_IMP ||
                op2_type == T_IFF || op2_type == T_AND)
            ostr << ")";
    }

    virtual Type getType() const
    {
        return T_AND;
    }
};

class Or : public BinaryConjective {
public:
    Or(const Formula & op1, const Formula & op2)
        :BinaryConjective(op1, op2)
    {}

    virtual void printConvertedFormula(ostream & ostr)
    {

        Type op1_type = _op1->getType();
        Type op2_type = _op2->getType();

        if(op1_type == T_IMP || op1_type == T_IFF)
            ostr << "(";

        _op1->printConvertedFormula(ostr);

        if(op1_type == T_IMP || op1_type == T_IFF)
            ostr << ")";

        ostr << " | ";

        if(op2_type == T_IMP ||
                op2_type == T_IFF || op2_type == T_OR)
            ostr << "(";

        _op2->printConvertedFormula(ostr);

        if(op2_type == T_IMP ||
                op2_type == T_IFF || op2_type == T_OR)
            ostr << ")";
    }

    virtual Type getType() const
    {
        return T_OR;
    }
};

class Imp : public BinaryConjective {
public:
    Imp(const Formula & op1, const Formula & op2)
        :BinaryConjective(op1, op2)
    {}

    virtual void printConvertedFormula(ostream & ostr)
    {

        Type op1_type = _op1->getType();
        Type op2_type = _op2->getType();

        if(op1_type == T_IFF)
            ostr << "(";

        _op1->printConvertedFormula(ostr);

        if(op1_type == T_IFF)
            ostr << ")";

        ostr << " => ";

        if(op2_type == T_IMP || op2_type == T_IFF)
            ostr << "(";

        _op2->printConvertedFormula(ostr);

        if(op2_type == T_IMP || op2_type == T_IFF)
            ostr << ")";
    }

    virtual Type getType() const
    {
        return T_IMP;
    }
};

class Iff : public BinaryConjective {

public:
    Iff(const Formula & op1, const Formula & op2)
        :BinaryConjective(op1, op2)
    {}

    virtual void printConvertedFormula(ostream & ostr)
    {

        Type op1_type = _op1->getType();
        Type op2_type = _op2->getType();

        _op1->printConvertedFormula(ostr);

        ostr << " => ";

        if(op1_type == T_IFF)
            ostr << "(";

        _op2->printConvertedFormula(ostr);

        if(op2_type == T_IFF)
            ostr << ")";

    }

    virtual Type getType() const
    {
        return T_IFF;
    }
};

class Quantifier : public BaseFormula {
protected:
    Variable _v;
    Formula  _op;
public:
    Quantifier(const Variable & v, const Formula & op)
        :_v(v),
          _op(op)
    {}

    const Variable & getVariable() const
    {
        return _v;
    }

    const Formula & getOperand() const
    {
        return _op;
    }

};

class Forall : public Quantifier {
public:
    Forall(const Variable & v, const Formula & op)
        :Quantifier(v, op)
    {}

    virtual Type getType() const
    {
        return T_FORALL;
    }
    virtual void printConvertedFormula(ostream & ostr)
    {
        cout << "![" << _v << "] : ";

        Type op_type = _op->getType();

        if(op_type == T_AND || op_type == T_OR ||
                op_type == T_IMP || op_type == T_IFF)
            ostr << "(";

        _op->printConvertedFormula(ostr);

        if(op_type == T_AND || op_type == T_OR ||
                op_type == T_IMP || op_type == T_IFF)
            ostr << ")";
    }
};

class Exists : public Quantifier {
public:
    Exists(const Variable & v, const Formula & op)
        :Quantifier(v, op)
    {}

    virtual Type getType() const
    {
        return T_EXISTS;
    }

    virtual void printConvertedFormula(ostream & ostr)
    {
        cout  << "?[" << _v << "] : ";

        Type op_type = _op->getType();

        if(op_type == T_AND || op_type == T_OR ||
                op_type == T_IMP || op_type == T_IFF)
            ostr << "(";

        _op->printConvertedFormula(ostr);

        if(op_type == T_AND || op_type == T_OR ||
                op_type == T_IMP || op_type == T_IFF)
            ostr << ")";
    }
};

inline
ostream & operator << (ostream & ostr, const Term & t)
{
    t->printTerm(ostr);
    return ostr;
}

inline
ostream & operator << (ostream & ostr, const Formula & f)
{
    f->printConvertedFormula(ostr);
    return ostr;
}

extern Formula parsed_formula;

#endif // _FOL_H
