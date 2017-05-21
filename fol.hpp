#ifndef _FOL_H
#define _FOL_H

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <sstream>
#include <map>

using namespace std;

typedef string FunctionSymbol;
typedef string PredicateSymbol;
typedef string Variable;

static map<string, string> toCoordinateLanguage = {
    {"collinear", "<<(1_x - 2_x) * (2_y - 3_y) = (1_y - 2_y) * (2_x - 3_x)>>"},
    {"parallel", "<<(1_x - 2_x) * (3_y - 4_y) = (1_y - 2_y) * (3_x - 4_x)>>"},
    {"perpendicular", "<<(1_x - 2_x) * (3_x - 4_x) + (1_y - 2_y) * (3_y - 4_y) = 0>>"},
    {"lengths_eq", "<<(1_x - 2_x)^2 + (1_y - 2_y)^2 = (3_x - 4_x)^2 + (3_y - 4_y)^2>>"},
    {"is_midpoint", "<<2 * 1_x = 2_x + 3_x & 2 * 1_y = 2_y + 3_y>>"},
    {"is_intersection", "<<(1_x - 2_x) * (2_y - 3_y) = (1_y - 2_y) * (2_x - 3_x) & (1_x - 4_x) * (4_y - 5_y) = (1_y - 4_y) * (4_x - 5_x)>>"},
    {"is_equal", "<<(1_x = 2_x) & (1_y = 2_y)>>"}
};

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

    virtual void printFormula(ostream & ostr) const = 0;
    virtual Type getType() const = 0;
    virtual ~BaseFormula() {}
};



class AtomicFormula : public BaseFormula {
public:
};


class LogicConstant : public AtomicFormula {

public:
};


class True : public LogicConstant {

public:
    virtual void printFormula(ostream & ostr) const
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
    virtual void printFormula(ostream & ostr) const
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

    virtual void printFormula(ostream & ostr) const
    {
        string transformed = toCoordinateLanguage[_p];

        for (size_t i = 1; i <= _ops.size(); i++)
        {
            transformed = ReplaceAll(transformed, to_string(i), ((FunctionTerm*)_ops[i-1].get())->getSymbol());
        }

        ostr << transformed;
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

    virtual void printFormula(ostream & ostr) const
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

    virtual void printFormula(ostream & ostr) const
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

    virtual void printFormula(ostream & ostr) const
    {
        ostr << "~";
        Type op_type = _op->getType();

        if(op_type == T_AND || op_type == T_OR ||
                op_type == T_IMP || op_type == T_IFF)
            ostr << "(";

        _op->printFormula(ostr);

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

    virtual void printFormula(ostream & ostr) const
    {
        Type op1_type = _op1->getType();
        Type op2_type = _op2->getType();

        if(op1_type == T_OR || op1_type == T_IMP ||
                op1_type == T_IFF)
            ostr << "(";

        _op1->printFormula(ostr);

        if(op1_type == T_OR || op1_type == T_IMP ||
                op1_type == T_IFF)
            ostr << ")";

        ostr << " & ";

        if(op2_type == T_OR || op2_type == T_IMP ||
                op2_type == T_IFF || op2_type == T_AND)
            ostr << "(";

        _op2->printFormula(ostr);

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

    virtual void printFormula(ostream & ostr) const
    {

        Type op1_type = _op1->getType();
        Type op2_type = _op2->getType();

        if(op1_type == T_IMP || op1_type == T_IFF)
            ostr << "(";

        _op1->printFormula(ostr);

        if(op1_type == T_IMP || op1_type == T_IFF)
            ostr << ")";

        ostr << " | ";

        if(op2_type == T_IMP ||
                op2_type == T_IFF || op2_type == T_OR)
            ostr << "(";

        _op2->printFormula(ostr);

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

    virtual void printFormula(ostream & ostr) const
    {

        Type op1_type = _op1->getType();
        Type op2_type = _op2->getType();

        if(op1_type == T_IFF)
            ostr << "(";

        _op1->printFormula(ostr);

        if(op1_type == T_IFF)
            ostr << ")";

        ostr << " => ";

        if(op2_type == T_IMP || op2_type == T_IFF)
            ostr << "(";

        _op2->printFormula(ostr);

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

    virtual void printFormula(ostream & ostr) const
    {

        Type op1_type = _op1->getType();
        Type op2_type = _op2->getType();

        _op1->printFormula(ostr);

        ostr << " => ";

        if(op1_type == T_IFF)
            ostr << "(";

        _op2->printFormula(ostr);

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
    virtual void printFormula(ostream & ostr) const
    {
        cout << "![" << _v << "] : ";

        Type op_type = _op->getType();

        if(op_type == T_AND || op_type == T_OR ||
                op_type == T_IMP || op_type == T_IFF)
            ostr << "(";

        _op->printFormula(ostr);

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

    virtual void printFormula(ostream & ostr) const
    {
        cout  << "?[" << _v << "] : ";

        Type op_type = _op->getType();

        if(op_type == T_AND || op_type == T_OR ||
                op_type == T_IMP || op_type == T_IFF)
            ostr << "(";

        _op->printFormula(ostr);

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
    f->printFormula(ostr);
    return ostr;
}

extern Formula parsed_formula;

#endif // _FOL_H
