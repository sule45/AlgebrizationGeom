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

class extendedString {
public:    
    extendedString(){}

    extendedString(string _varName, optimizationLevel l = notOptimized)
        :varName(_varName), level(l)
    {}

    string varName;
    optimizationLevel level;

    const string str(axis a);
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
    relation par = parallel;
    if(s == "collinear") {
        return collinear;
    }
    else if(s == "parallel") {
        return par;
    }
    else if(s == "perpendicular") {
        return perpendicular;
    }
    else if(s == "lengths_eq") {
        return lengths_eq;
    }
    else if(s == "is_midpoint") {
        return is_midpoint;
    }
    else if(s == "is_intersection") {
        return is_intersection;
    }
    else {//if(s == "is_equal") 
        return is_equal;
    }
}

static string openExp = "<<";
static string closeExp = ">>";

static void toCoordinateLanguage(relation rel, vector<extendedString>& points);

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
                T_AND, T_OR, T_IMP, T_IFF, T_FORALL, T_EXISTS, T_PLUS, T_MINUS, T_EQ, T_TIMES, T_POW, T_LIT };

    void doTheMagic(bool optInd, ostream& ostr);

    virtual void printConvertedFormula(ostream & ostr) = 0;
    virtual Type getType() const = 0;
    //virtual ~BaseFormula() {}
    virtual Formula simple() = 0;
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

    virtual Formula simple(){
        return shared_from_this();
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

    Formula simple(){
        return shared_from_this();
    }
};

class Lit : public BaseFormula {
protected:
    PredicateSymbol _p;

public:
    Lit(const PredicateSymbol & p)
        :_p(p)
    {}

    const PredicateSymbol & getSymbol() const
    {
        return _p;
    }

    void printConvertedFormula(ostream & ostr)
    {
        ostr << _p;
    }

    Type getType() const
    {
        return T_LIT;
    }

    virtual Formula simple(){
        return shared_from_this();
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

    virtual void printConvertedFormula(ostream & ostr);

    virtual Type getType() const
    {
        return T_ATOM;
    }

    virtual Formula simple(){
        return shared_from_this();
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

    extendedString extend(string varName);
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

    virtual Formula simple()
    {
    /* Negacija se uproscava prema pravilima: ~True === False,
                                              ~False === True.
        Kod svih formula, simplifikacija se prvo primeni
        rekurzivno na podformule, pa se zatim primenjuju pravila. */
        Formula simp_op = _op->simple();
  
        if(simp_op->getType() == T_TRUE)
            return make_shared<False>();
        else if(simp_op->getType() == T_FALSE)
            return make_shared<True>();
        else
            return make_shared<Not>(simp_op);
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

    virtual Formula simple(){
        /* Simplifikacija konjukcije po pravilima A /\ True === A, 
                                                  A /\ False === False i sl. */
        
        const Formula simp_op1 = _op1->simple();
        const Formula simp_op2 = _op2->simple();
  
        if(simp_op1->getType() == T_TRUE)
            return simp_op2;
        else if(simp_op2->getType() == T_TRUE)
            return simp_op1;
        else if(simp_op1->getType() == T_FALSE ||
                simp_op2->getType() == T_FALSE)
            return make_shared<False>();
        else
        return make_shared<And>(simp_op1, simp_op2);
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

    virtual Formula simple(){
        
          /* Simplifikacija disjunkcije po pravilima: A \/ True === True,
                                                      A \/ False === A, i sl. */
        const Formula simp_op1 = _op1->simple();
        const Formula simp_op2 = _op2->simple();

        if(simp_op1->getType() == T_FALSE) 
            return simp_op2;
        else if(simp_op2->getType() == T_FALSE)
            return simp_op1;
        else if(simp_op1->getType() == T_TRUE ||
                simp_op2->getType() == T_TRUE)
        return make_shared<True>();
        else
        return make_shared<Or>(simp_op1, simp_op2);
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

    virtual Formula simple(){
        /* Simplifikacija implikacije po pravilima: A ==> True === True,
                                                    A ==> False === ~A,
                                                    True ==> A === A,
                                                    False ==> A === True */
        Formula simp_op1 = _op1->simple();
        Formula simp_op2 = _op2->simple();
          
        if(simp_op1->getType() == T_TRUE)
            return simp_op2;
        else if(simp_op2->getType() == T_TRUE)
            return make_shared<True>();
        else if(simp_op1->getType() == T_FALSE) 
            return make_shared<True>();
        else if(simp_op2->getType() == T_FALSE)
            return make_shared<Not>(simp_op1);
        else
            return make_shared<Imp>(simp_op1, simp_op2);
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

    virtual Formula simple(){
        
        /* Ekvivalencija se simplifikuje pomocu pravila: True <=> A === A,
                                                         False <=> A === ~A i sl. */
  
        Formula simp_op1 = _op1->simple();
        Formula simp_op2 = _op2->simple();
      
        if( simp_op1->getType() == T_FALSE && 
            simp_op2->getType() == T_FALSE)
            return make_shared<True>();
        else if(simp_op1->getType() == T_TRUE)
            return simp_op2;
        else if(simp_op2->getType() == T_TRUE)
            return simp_op1;
        else if(simp_op1->getType() == T_FALSE) 
            return make_shared<Not>(simp_op2);
        else if(simp_op2->getType() == T_FALSE)
            return make_shared<Not>(simp_op1);
        else
            return make_shared<Iff>(simp_op1, simp_op2);
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

    virtual Formula simple(){
        return shared_from_this();
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

class Plus : public BinaryConjective {
public:
    Plus(const Formula & op1, const Formula & op2)
        :BinaryConjective(op1, op2)
    {}

    virtual Type getType() const
    {
        return T_PLUS;
    }

    virtual void printConvertedFormula(ostream & ostr) {
        ostr << "(";

        _op1->printConvertedFormula(ostr);

        ostr << " + ";

        _op2->printConvertedFormula(ostr);

        ostr << ")";
    }

    virtual Formula simple() {
        const Formula simp_op1 = _op1->simple();
        const Formula simp_op2 = _op2->simple();
        
        if (simp_op1->getType() == T_LIT) // onda je i simp_op2 isto T_LIT
        {
            if(((Lit *)simp_op1.get())->getSymbol() == "0"){
                if(simp_op2->getType() == T_LIT && ((Lit *)simp_op2.get())->getSymbol() == "0")
                {
                    // (0 + 0) = 0
                    Formula f1 = make_shared<Lit>("0");
                    return f1;    
                }
                else
                {
                    // (0 + x) = x;
                    Formula f1 = simp_op2;
                    return f1;    
                }
            }
        }
        
        if (simp_op2->getType() == T_LIT && ((Lit *)simp_op2.get())->getSymbol() == "0"){
            
                Formula f2 = simp_op1; //(x + 0) = x
                return f2;    
        }

        return make_shared<Plus>(simp_op1, simp_op2);
    }
};

class Minus : public BinaryConjective {
public:
    Minus(const Formula & op1, const Formula & op2)
        :BinaryConjective(op1, op2)
    {}

    virtual Type getType() const
    {
        return T_MINUS;
    }

    virtual void printConvertedFormula(ostream & ostr) {
        ostr << "(";

        _op1->printConvertedFormula(ostr);

        ostr << " - ";

        _op2->printConvertedFormula(ostr);

        ostr << ")";
    }

    virtual Formula simple() {
        
        const Formula simp_op1 = _op1->simple();
        const Formula simp_op2 = _op2->simple();
        if (simp_op1->getType() == T_LIT) // onda je i simp_op2 isto T_LIT
        {
            if(((Lit *)simp_op1.get())->getSymbol() == "0"){

                PredicateSymbol minusSymbol;
            
                if(((Lit *)_op2.get())->getSymbol() == "0") {
                    minusSymbol = "0"; // (0 - 0) = 0
                }
                else {
                    minusSymbol = "-" + ((Lit *)_op2.get())->getSymbol(); // (0 - x) = x
                }
            
                Formula f1 = make_shared<Lit>(minusSymbol);
                return f1;    
            }
        }
        
        if (_op2->getType() == T_LIT)
        {
            if(((Lit *)_op2.get())->getSymbol() == "0"){
            
                Formula f2 = make_shared<Lit>(((Lit *)_op1.get())->getSymbol()); //(x - 0) = x
                return f2;    
            }
        }

        return make_shared<Minus>(simp_op1, simp_op2);
    }
};

class Times : public BinaryConjective {
public:
    Times(const Formula & op1, const Formula & op2)
        :BinaryConjective(op1, op2)
    {}

    virtual Type getType() const
    {
        return T_TIMES;
    }

    virtual void printConvertedFormula(ostream & ostr)  {
        ostr << "(";

        _op1->printConvertedFormula(ostr);

        ostr << " * ";

        _op2->printConvertedFormula(ostr);

        ostr << ")";
    }

    virtual Formula simple() {
        
        const Formula simp_op1 = _op1->simple();
        const Formula simp_op2 = _op2->simple();
        
        if (simp_op1->getType() == T_LIT)
        {

            if(((Lit *)simp_op1.get())->getSymbol() == "0"){
                
                Formula f1 = make_shared<Lit>("0");
                return f1;    
            }
        }
        if (simp_op2->getType() == T_LIT)
        {
            if(((Lit *)simp_op2.get())->getSymbol() == "0"){
                
                Formula f2 = make_shared<Lit>("0");
                return f2;    
            }
        }

        return make_shared<Times>(simp_op1, simp_op2);
    }

};

class Pow : public UnaryConjective {
public:
    Pow(const Formula & op1)
        :UnaryConjective(op1)
    {}
        

    virtual Type getType() const
    {
        return T_POW;
    }

    virtual void printConvertedFormula(ostream & ostr)  {
        
        if(_op->getType() == T_LIT){
            ostr << "("; 
            _op->printConvertedFormula(ostr);
            ostr << ")";
        }
        else {
            _op->printConvertedFormula(ostr);
        }
        ostr << "^2";
    }

    virtual Formula simple() {
        
        const Formula simp_op1 = _op->simple();
        
        if (simp_op1->getType() == T_LIT)
        {
            if(((Lit *)simp_op1.get())->getSymbol() == "0"){
                Formula f1 = make_shared<Lit>("0");
                return f1;    
            }
        }
        return make_shared<Pow>(simp_op1);
    }

};

class Equal : public BinaryConjective {
public:
    Equal(const Formula & op1, const Formula & op2)
        :BinaryConjective(op1, op2)
    {}

    virtual Type getType() const
    {
        return T_EQ;
    }

    virtual void printConvertedFormula(ostream & ostr) {
        ostr << "(";

        _op1->printConvertedFormula(ostr);

        ostr << " = ";

        _op2->printConvertedFormula(ostr);

        ostr << ")";
    }

    virtual Formula simple() {
        const Formula simp_op1 = _op1->simple();
        const Formula simp_op2 = _op2->simple();

        if (simp_op1->getType() == T_LIT && simp_op2->getType() == T_LIT) {
            if(((Lit *)simp_op1.get())->getSymbol() == "0" && ((Lit *)simp_op2.get())->getSymbol() == "0"){
                Formula f1 = make_shared<True>();
                return f1;    
            }
        }

        return make_shared<Equal>(simp_op1, simp_op2);
    }
};

Formula createMinus(const string str1, const string str2);

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
