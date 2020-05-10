/*file:    recEq.c*/
/*author:  Luke McNaughton & Medhat Kandil */
/*version: 2.0*/
/*Description: Solve equation in given input*/

#include <stdio.h>  /* getchar, printf */
#include <stdlib.h> /* NULL */
#include <string.h>
#include "scanner.h"
#include "recognizeEq.h"

/* The functions acceptNumber, acceptIdentifier and acceptCharacter have as
 * (first) argument a pointer to an token list; moreover acceptCharacter has as
 * second argument a character. They check whether the first token
 * in the list is a number, an identifier or the given character, respectively.
 * When that is the case, they yield the value 1 and the pointer points to the rest of
 * the token list. Otherwise they yield 0 and the pointer remains unchanged.
 */

int acceptNumber(List *lp) {  //DO NOT CHANGE
  if (*lp != NULL && (*lp)->tt == Number) {
    *lp = (*lp)->next;
    return 1;
  }
  return 0;
}

int acceptIdentifier(List *lp) {  //DO NOT CHANGE
  if (*lp != NULL && (*lp)->tt == Identifier) {
    *lp = (*lp)->next;
    return 1;
  }
  return 0;
}

int acceptCharacter(List *lp, char c) {  //DO NOT CHANGE
  if (*lp != NULL && (*lp)->tt == Symbol && ((*lp)->t).symbol == c) {
    *lp = (*lp)->next;
    return 1;
  }
  return 0;
}

/* This function is used to check wheter we are trying to use a symbol that
 * is not allowed in our grammar */
int acceptSymbol(List *lp) {
    if(*lp != NULL && (*lp)->tt == Symbol) {
        char c = (*lp)->t.symbol;
        if(c != '+' && c != '-' && c!= '=' && c != '^') return 0;
    }
    return 1;
}

int acceptEquation(List *lp) {

    int equal = 0;  //Wheter we have two equal symbols in the equation (permanent)
    int symb = 0;  //Wheter we have just analyzed a symbol (on-off)

    if(acceptCharacter(lp, '+') || acceptCharacter(lp, '^') || acceptCharacter(lp, '=')) return 0;
    
    while((*lp) != NULL){ 

        if(!acceptSymbol(lp)){
            return 0;
        }

        /* We can't have two consecutive numbers */
        /*if(acceptNumber(lp)) {
            if(symb) symb = 0;  //The symbol flag is reset after finding a number
            
            if(acceptNumber(lp)){
                return 0;
            }
        }*/

        /* It's possible to have a power only after an identifier, a number must follow */
         /* The symbol flag is reset after finding an identifier */
         /* Can't have a number or identifier right afterwards */
        if(acceptIdentifier(lp)) {
            
            if(symb){
                symb = 0;
            }
           
            if(acceptNumber(lp) || acceptIdentifier(lp)){
                return 0;
            }
            
            if(acceptCharacter(lp, '^')) {
                if(!acceptNumber(lp)) return 0;
                else {
                    /* We can't have another number or identifier, a symbol or nothing must follow */
                    if(acceptNumber(lp) || acceptIdentifier(lp)){
                        return 0;
                    }
                }
            }
        }

        /* We can't have two consecutive symbols */
        if(acceptCharacter(lp, '+') || acceptCharacter(lp, '-')) {
            if(symb) return 0;
            symb = 1;  //We set the symbol flag
        }

        if(acceptCharacter(lp, '=')) {
            /* Only the minus sign is acceptable after an equal */
            if(acceptCharacter(lp, '-')) symb = 0;
            /* If we have two consecutive symbols or if it's not the first equal */
            if(symb || equal) return 0;
            /* Flags are set */
            equal = 1;
            symb = 1;
        }

        /* Power is possible only after an identifier */
        if(acceptCharacter(lp, '^')){
            return 0;
        }
    }

    /* An equation can't finish with any symbol or without an equal */
    if(symb || !equal){
        return 0;
    }

    return 1;
}

void recognizeEquations() {
  char *ar;
  List tl, tl1, tl2;
  printf("give an equation: ");
  ar = readInput();
  while (ar[0] != '!') {
    tl = tokenList(ar);
    printList(tl);
    //variables for iterating on the equation
    tl1 = tl;
    tl2 = tl;

    /* We check if it's an equation at all */
    if(tl1 == NULL || !acceptEquation(&tl1)) printf("this is not an equation\n");
    else {  //If it is:
        printf("this is an equation");
        //Flags
        int pow = -1;  //Degree of the equation
        int vars = 0;  //Amount of variables, if != 1 print specific message
        int eq = 0;  //If we found the equal sign
        int op = 1;  //If we are adding or subtracting
        char *id = NULL;  //Name of the first identifier for comparing
        double nums = 0, idnums = 0, temp = 1;
        while(tl2 != NULL) {
            /* Handling symbols */
            if(tl2->tt == Symbol) {
                if(tl2->t.symbol == '+') op = 1;
                else if(tl2->t.symbol == '-') op = 0;
                else if(tl2->t.symbol == '=') {
                    op = 1;
                    eq = 1;
                }
                tl2 = tl2->next;  //There must be something after a symbol
            }

            if(tl2->tt == Number) {
                if(tl2->next != NULL && tl2->next->tt == Identifier) temp = tl2->t.number;
                else nums += ((!op && eq) || (op && !eq)) ? tl2->t.number
                                                        : -(tl2->t.number);
                tl2 = tl2->next;  //Next element
                //if(tl2 == NULL) break;  //WHY??
            }

            if(tl2 != NULL && tl2->tt == Identifier) {
                /* We analyze the current identifier name */
                char *currID = tl2->t.identifier;
                /* If we never found any identifier so far */
                if(id == NULL) {
                    id = malloc((strlen(currID)+1) * sizeof(char));
                    strncpy(id, currID, strlen(currID)+1);
                    vars++;
                }
                /* If the current identifier is different from the first one */
                if(strcmp(id, currID) != 0) {
                    vars++;
                    if(vars != 1) break;
                }
                tl2 = tl2-> next;  //We skip to the next element manually
                /* If after the identifier there is a power symbol */
                if(tl2 != NULL && tl2->tt == Symbol && tl2->t.symbol == '^') {
                    tl2 = tl2->next;  //We skip to the subsequent number
                    /* If the saved power is smaller than the current one */
                    if(pow < tl2->t.number) pow = tl2->t.number;
                    /* If the power is 0 the identifier is 1 */
                    if(tl2->t.number == 0) {
                        nums += ((!op && eq) || (op && !eq)) ? temp : -(temp);
                    }
                    /* If the power is not 1, no point is saving the id value */
                    else if(tl2->t.number == 1) {
                        idnums += ((!op && eq) || (op && !eq)) ? temp : -(temp);
                    }
                    //if(tl2->next != NULL) tl2 = tl2->next;  //Why the NULL check??
                    tl2 = tl2->next;  //Manually skip to next element
                } else {
                    idnums += ((!op && eq) || (op && !eq)) ? temp : -(temp);
                    /* If there is no power after the identifier and the power
                     * is smaller than 1, we set pow to 1 */
                    if(pow < 1) pow = 1;
                }
                temp = 1;  //Reset temp after an identifier
            }
        }

        /* If there is no identifiers or several of them */
        if(vars != 1) printf(", but not in 1 variable\n");
        /* If there is 1 identifier with his relative degree */
        else {
            printf(" in 1 variable of degree %d\n", pow);
            if(pow == 1) {
                //printf("\n*nums=%f idnums=%f\n", -(nums), idnums);
                double res = -(nums) / idnums;
                if(res <= 0.0 && res >= -0.0005) res = 0;
                if(idnums != 0.0) printf("solution: %.3f\n", res);
                else printf("not solvable\n");
            }
        }
        free(id);  //The saved id is freed
    }

    /* We free everything we used for checking the equation */
    free(ar);
    freeTokenList(tl);
    printf("\ngive an equation: ");
    ar = readInput();
  }
  free(ar);
  printf("good bye\n");
}
