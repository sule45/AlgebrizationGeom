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

    if(argc > 1 && strcmp(argv[1], "-o") == 0){
        optIndicator = true;
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
