/*file:    recEq.c*/
/*author:  Luke McNaughton & Medhat Kandil */
/*version: 2.0*/
/*Description: Determine if the input is an equation, and gives the variable of the highest degree*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scanner.c"
#include "scanner.h"


int global_identifier_flag = 0;     // Global flag used for printing, identifies if the number of variables
                                    // in input is valid or invalid (0 -> invalid, 1 -> valid)
                                    // When 0 -> number of variables != 1, else flag == 1
int variable_exist = 0;             // Used when calculating the maximum degree, depending on whether the
                                    // variable exist we will set the starting degree to 1 or 0
                                    // exist == 1, doesn't exist == 0
                                    
int acceptNumber(List *lp) {
    if (*lp != NULL && (*lp)->tt == Number) {
        *lp = (*lp)->next;
        return 1;
    }
    return 0;
}

int acceptIdentifier(List *lp) {
    if (*lp != NULL && (*lp)->tt == Identifier) {
        *lp = (*lp)->next;
        return 1;
    }
    return 0;
}

int acceptCharacter(List *lp, char c) {
    if (*lp != NULL && (*lp)->tt == Symbol && ((*lp)->t).symbol == c) {
        *lp = (*lp)->next;
        return 1;
    }
    return 0;
}

int acceptExpressions(List *lp){

  if(*lp==NULL)return 0;            // Check for empty list

  if(acceptCharacter(lp, '-')){     // We extend the original acceptExpressions to be able to accept expressions
    if (!acceptTerm(lp)) {          // Starting with a negative sign '-'
      return 0;
    }
  }else{
    if (!acceptTerm(lp)) {
      return 0;
    }
  }
  while (acceptCharacter(lp, '+') || acceptCharacter(lp, '-')) {
    if (!acceptTerm(lp)) {
      return 0;
    }
  }
  return 1;
}

int acceptEquation(List *lp  ){
  if (acceptExpressions(lp) && acceptCharacter(lp,'=') && acceptExpressions(lp)){ // A composite function provided in BNF grammar
    return 1;                                                                     // <equation> ::= <expression> = <expression>
  }
  return 0;
}

void globalReset(){
  global_identifier_flag = 0; // Function to reseting global counters and flags
  variable_exist = 0;         // To prepare for the new input line
}

void countVariables(List tl){
  int length_of_identifier = 0;  // variable to store length of the first found identifier
  char *first_variable;          // array to store the identifier itself
  if(tl == NULL){                // Check if empty
    return ;
  }

  List tl_copy;            // Create a copy to iterate through the nodes without losing the list
  tl_copy = tl;            // after tl = tl->next

    while(tl_copy != NULL && global_identifier_flag == 0){

      // Enter if found first identifier
      if(tl_copy->tt==Identifier){
        global_identifier_flag  = 1;                                //set to 1 since current number of variables is 1
        variable_exist = 1;                                         // set to 1 to know that there is at least one variable of degree 1
                                                                    // Used for special case when both x^0 and x without indicated power are in input
        length_of_identifier = strlen(tl_copy->t.identifier)+1;
        first_variable = malloc(sizeof(char)*length_of_identifier);
        int i = 0;
        for(i = 0; tl_copy->t.identifier[i]!='\0'; i++){            // Copy identifier into array to compare to the following identifiers
          first_variable[i] = tl_copy->t.identifier[i];
        }
        first_variable[i]='\0';
      }
      tl_copy=tl_copy->next;
    }

    if(global_identifier_flag == 0){    // Enter and return if global_identifier_flag was unchanged
      return ;                          // Case when not a since identifier is in the input
    }

  while(tl_copy != NULL){
    if(tl_copy->tt==Identifier){

      if(strlen(first_variable)!=strlen(tl_copy->t.identifier)){  // if the string length of the new identifier is different -> more than 1 variable
        global_identifier_flag = 0;                               // set to 0 since more that 1 variables
        free(first_variable);
        return ;
      } else {

        for(int i = 0; first_variable[i]!='\0'; i++){             // Check element by element if the arrays are the same
          if(first_variable[i] != tl_copy->t.identifier[i]){      // if at some point individual element is different -> more than one variable
            global_identifier_flag = 0;
            free(first_variable);
            return ;
          }
        }
      }
    }
    tl_copy=tl_copy->next;
  }
  free(first_variable);
  return ;
}

int findDegree(List tl){
  int start_degree = 0;
  if (variable_exist == 1){  // after execution of countVariables we know if there is at least 1 variable
    start_degree = 1;        // If variable exist we set 1 as a minimum degree
  }
  /*
      in the folowing loop when we find a symbol "^" move to the next node and test if
      it has TokenType of a Number and is greater than the current highest Degree (start_degree)
      then we overwrite the start_degree variable.
  */
  while (tl != NULL){
    if (tl->tt == Symbol && tl->t.symbol == '^'){
      tl=tl->next;
      if (tl->tt == Number && tl->t.number > start_degree){
        start_degree = tl->t.number;
      }
    }
    tl = tl->next;
  }
  return start_degree;
}

void recognizeEquations(){
  int highest_degree = 0;
  char *ar;
  List tl, tl1;
  printf("give an equation: ");
  ar = readInput();
  while (ar[0] != '!') {

      tl = tokenList(ar);
      printList(tl);
      tl1 = tl;
      countVariables(tl);
      tl=tl1;
      highest_degree = findDegree(tl);
      // Enter if the input evaluated as an equation
      if (acceptEquation(&tl1) && tl1 == NULL) {
        // Flag indicates only 1 identifier present
        if (global_identifier_flag == 1){
          printf("this is an equation in 1 variable of degree %d\n", highest_degree);
        // Otherwise input still evaluated as expression but not with one variable.
        } else {
          printf("this is an equation, but not in 1 variable\n");
        }
      } else {
        printf("this is not an equation\n");
      }
      free(ar);
      freeTokenList(tl);
      globalReset();                              // Reset of global variables
      printf("\ngive an equation: ");
      ar = readInput();
  }
  free(ar);
  printf("good bye\n");
}

int main(int argc, char **argv){
  recognizeEquations();
  return 0;
}