#include "fol.hpp"


extern int yyparse();

/* Ovaj pokazivac ce nakon parsiranja dobiti vrednost 
   adrese parsirane formule. */
extern Formula parsed_formula;

int main()
{
    yyparse();
    if(parsed_formula.get() != NULL)
        cout << parsed_formula;

    cout << endl;

    return 0;
}
