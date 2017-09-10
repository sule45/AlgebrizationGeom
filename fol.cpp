#include "fol.hpp"
#include "optimization.h"

void BaseFormula::doTheMagic(bool optInd, ostream& ostr){
    optimization::optimizedVar = "";
    optimization::halfOptimizedVar = "";

    optimization::optimizationIndicator = optInd;
    printConvertedFormula(ostr);
    ostr << endl;
}

void Atom::printConvertedFormula(ostream & ostr)
{
    vector<extendedString> extendedOps;
    for (auto it : _ops)
           extendedOps.push_back(extend(((FunctionTerm*)(it.get()))->getSymbol()));

    toCoordinateLanguage(getRelation(_p), extendedOps);
}

extendedString Atom::extend(string varName) {
    if(optimization::optimizationIndicator && optimization::optimizedVar.size() == 0){
        optimization::optimizedVar = varName;
    }

    if(optimization::optimizationIndicator && optimization::optimizedVar.size() > 0 &&
            varName != optimization::optimizedVar && optimization::halfOptimizedVar.size() == 0){
        optimization::halfOptimizedVar = varName;
    }

    optimizationLevel level = notOptimized;
    if(varName == optimization::optimizedVar){
        level = optimized;
    }
    else if (varName == optimization::halfOptimizedVar){
        level = halfOptimized;
    }

    return extendedString(varName, level);
}

const string extendedString::str(axis a){

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

Formula createMinus(const string str1, const string str2) {
    
    PredicateSymbol p1 = str1;
    PredicateSymbol p2 = str2;
    Formula lit1 = make_shared<Lit>(p1);
    Formula lit2 = make_shared<Lit>(p2);
    Formula minus = make_shared<Minus>(lit1, lit2);
    return minus;
}
static void toCoordinateLanguage(relation rel, vector<extendedString>& points){

    Formula minus1, minus2, minus3, minus4, minus5, minus6, minus7, minus8;
    Formula times1, times2, times3, times4;
    Formula plus1, plus2;
    Formula equal, equal1, equal2;
    Formula f1;
    Formula and1;

    switch(rel){
        case collinear:
        {
        // "<<(0_x - 1_x) * (1_y - 2_y) = (0_y - 1_y) * (1_x - 2_x)>>"
        minus1 = createMinus(points[0].str(X), points[1].str(X));
        minus2 = createMinus(points[1].str(Y), points[2].str(Y));
        minus3 = createMinus(points[0].str(Y), points[1].str(Y));
        minus4 = createMinus(points[1].str(X), points[2].str(X));
        
        times1 = make_shared<Times>(minus1, minus2); 
        times2 = make_shared<Times>(minus3, minus4);
        equal = make_shared<Equal>(times1, times2);
        f1 = ((Equal *)equal.get())->simple();
        f1->printConvertedFormula(cout); 
        }
        break;
        case parallel:
        {
        // "<<(0_x - 1_x) * (2_y - 3_y) = (0_y - 1_y) * (2_x - 3_x)>>"
        minus1 = createMinus(points[0].str(X), points[1].str(X));
        minus2 = createMinus(points[2].str(Y), points[3].str(Y));
        minus3 = createMinus(points[0].str(Y), points[1].str(Y));
        minus4 = createMinus(points[2].str(X), points[3].str(X));
        
        times1 = make_shared<Times>(minus1, minus2); 
        times2 = make_shared<Times>(minus3, minus4);
        equal = make_shared<Equal>(times1, times2);
        f1 = ((Equal *)equal.get())->simple();
        f1->printConvertedFormula(cout); 
        }
        break;
    case perpendicular:
        {
        // "<<(0_x - 1_x) * (2_x - 3_x) + (0_y - 1_y) * (2_y - 3_y) = 0>>"
        minus1 = createMinus(points[0].str(X), points[1].str(X));
        minus2 = createMinus(points[2].str(X), points[3].str(X));
        minus3 = createMinus(points[0].str(Y), points[1].str(Y));
        minus4 = createMinus(points[2].str(Y), points[3].str(Y));
        
        times1 = make_shared<Times>(minus1, minus2); 
        times2 = make_shared<Times>(minus3, minus4);
        plus1 = make_shared<Plus>(times1, times2);
        equal = make_shared<Equal>(plus1, make_shared<Lit>("0"));
        f1 = ((Equal *)equal.get())->simple();
        f1->printConvertedFormula(cout); 
        }   
        break;
     /*   
    case lengths_eq:
        // "<<(0_x - 1_x)^2 + (0_y - 1_y)^2 = (2_x - 3_x)^2 + (2_y - 3_y)^2>>"
        ss << "(" << points[0].str(X) << " - " << points[1].str(X) << ")^2 + ("
                  << points[0].str(Y) << " - " << points[1].str(Y) << ")^2 = ("
                  << points[2].str(X) << " - " << points[3].str(X) << ")^2 + ("
                  << points[2].str(Y) << " - " << points[3].str(Y) << ")^2";
        break;    
    */
    case is_midpoint:
        {
        // "<<2 * 0_x = 1_x + 2_x & 2 * 0_y = 1_y + 2_y>>"
        times1 = make_shared<Times>(make_shared<Lit>("2"), make_shared<Lit>(points[0].str(X)));
        plus1 = make_shared<Plus>(make_shared<Lit>(points[1].str(X)), make_shared<Lit>(points[2].str(X)));
        equal1 = make_shared<Equal>(times1, plus1);

        times2 = make_shared<Times>(make_shared<Lit>("2"), make_shared<Lit>(points[0].str(Y)));
        plus2 = make_shared<Plus>(make_shared<Lit>(points[1].str(Y)), make_shared<Lit>(points[2].str(Y)));
        equal2 = make_shared<Equal>(times2, plus2);

        and1 = make_shared<And>(equal1, equal2);
        f1 = ((And *)and1.get())->simple();
        f1->printConvertedFormula(cout);
        }
        break;
    case is_intersection:
        {
        // "<<(0_x - 1_x) * (1_y - 2_y) = (0_y - 1_y) * (1_x - 2_x) & (0_x - 3_x) * (3_y - 4_y) = (0_y - 3_y) * (3_x - 4_x)>>"
        minus1 = createMinus(points[0].str(X), points[1].str(X));
        minus2 = createMinus(points[1].str(Y), points[2].str(Y));
        minus3 = createMinus(points[0].str(Y), points[1].str(Y));
        minus4 = createMinus(points[1].str(X), points[2].str(X));
        times1 = make_shared<Times>(minus1, minus2);
        times2 = make_shared<Times>(minus3, minus4);
        equal1 = make_shared<Equal>(times1, times2);

        minus5 = createMinus(points[0].str(X), points[3].str(X));
        minus6 = createMinus(points[3].str(Y), points[4].str(Y));
        minus7 = createMinus(points[0].str(Y), points[3].str(Y));
        minus8 = createMinus(points[3].str(X), points[4].str(X));
        times3 = make_shared<Times>(minus5, minus6);
        times4 = make_shared<Times>(minus7, minus8);
        equal2 = make_shared<Equal>(times3, times4);

        and1 = make_shared<And>(equal1, equal2);
        f1 = ((And *)and1.get())->simple();
        f1->printConvertedFormula(cout);
        }
        break;
    case is_equal:
        {
        // "<<(0_x = 1_x) & (0_y = 1_y)>>"
        equal1 = make_shared<Equal>(make_shared<Lit>(points[0].str(X)), make_shared<Lit>(points[1].str(X)));
        equal2 = make_shared<Equal>(make_shared<Lit>(points[0].str(Y)), make_shared<Lit>(points[1].str(Y)));

        and1 = make_shared<And>(equal1, equal2);
        f1 = ((And *)and1.get())->simple();
        f1->printConvertedFormula(cout);
        }
        break;
    default:
        break;
    }
}