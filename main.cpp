#include "fol.hpp"


extern int yyparse();

/* Ovaj pokazivac ce nakon parsiranja dobiti vrednost 
   adrese parsirane formule. */
extern Formula parsed_formula;
extern bool exitIndicator;

int main()
{
    while(true){
        yyparse();
        if(exitIndicator)
            break;
        cout << parsed_formula << endl;
    }

    cout << endl;
    return 0;
}
