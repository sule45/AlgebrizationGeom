#include "fol.hpp"

#include <string>
#include <string.h>

extern int yyparse();

/* Ovaj pokazivac ce nakon parsiranja dobiti vrednost 
   adrese parsirane formule. */
extern Formula parsed_formula;
extern bool exitIndicator;
extern bool printIndicator;

int main(int argc, char* argv[])
{
    bool optIndicator = false;

    for(int i = 1; i < argc ;i++){
        
	if(strcmp(argv[i], "-o") == 0){
            optIndicator = true;
            
        }
        
        if(strcmp(argv[i], "-h") == 0){
            cout <<  "USE: \n"
                 <<  "collinear(A1, A2, A3) \n" 
                 <<  "parallel(A1, A2, A3, A4) \n"
                 <<  "perpendicular(A1, A2, A3, A4)  \n"
                 <<  "lengths_eq(A1, A2, A3, A4)  \n"
                 <<  "is_midA(A1, A2, A3)  \n"
                 <<  "is_intersection(A1, A2, A3, A4, A5)  \n"
                 <<  "is_equal(A2, A2) \n\n" << endl;
        }
        
    }

    while(true){

        yyparse();

        if(exitIndicator)
            break;

        if(printIndicator)
            parsed_formula.get()->doTheMagic(optIndicator, cout);
        else
            cout << endl;

        printIndicator = false;
    }

    cout << endl;
    return 0;
}
