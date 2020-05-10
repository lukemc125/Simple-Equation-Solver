/*file:    recEq.c*/
/*author:  Luke McNaughton & Medhat Kandil */
/*version: 2.0*/
/*Description: Solve equation in given input*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scanner.h"
#include "scanner.c"
#include "recognizeEq.h"


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

int acceptCharacter(List *lp, char c) {  //DO NOT CHANGE
  if (*lp != NULL && (*lp)->tt == Symbol && ((*lp)->t).symbol == c) {
    *lp = (*lp)->next;
    return 1;
  }
  return 0;
}

int acceptEquation(List *lp) {
  int equalCount = 0; 
  int symb = 0;  //Flag to prevent consecutive symbols
  if(acceptCharacter(lp, '+') || acceptCharacter(lp, '^') ||
    acceptCharacter(lp, '=')){
      return 0;
    }
  while((*lp) != NULL){

    //Cant have two consecutive numbers
    if(acceptNumber(lp)) {
        symb=0;
    }

    if(acceptIdentifier(lp)) {
      if(symb==1){
        symb = 0;
      }
      if(acceptNumber(lp) || acceptIdentifier(lp)){
        return 0;
      }
      //Power must follow   
      if(acceptCharacter(lp, '^')) {
        if(acceptNumber(lp)==0) {
          return 0;
        }
        else {
            if(acceptNumber(lp) || acceptIdentifier(lp)){
              return 0;
            }
        }
      }
    }

      if(acceptCharacter(lp, '+') || acceptCharacter(lp, '-')) {
          /* We can't have two consecutive symbols */
          if(symb==1) {
            return 0;
          }
          symb = 1;
      }
      if(acceptCharacter(lp, '=')) {
          if(acceptCharacter(lp, '-')) symb = 0;
          //two consecutive equals
          if(symb ==1|| equalCount ==1){
            return 0;
          }
          equalCount = 1;
          symb = 1;
      }
      if(acceptCharacter(lp, '^')){
        return 0;
      }
  }

  /*Can't finish with any symbol or without an equal */
  if(symb==1 || equalCount==0){
    return 0;
  }

  return 1;
}

void solveEq(int degree, double nums, double idnums){

  double solution;

  printf(" in 1 variable of degree %d\n", degree);
  if(degree == 1) {
    solution = -(nums) / idnums;
    if(solution <= 0.0 && solution >= -0.0005){
      solution = 0;
    }
    if(idnums != 0.0){
      printf("solution: %.3f\n", solution);
    }
    else{
      printf("not solvable\n");
    }
  }
}

int verifyEq(List tl1, List tl2, int degree, int numVars, int eqExists, int op, double nums, double idnums, double temp, char *id){

  //For symbols
  while(tl2 != NULL) {
    if(tl2->tt == Symbol) {
        if(tl2->t.symbol == '+') op = 1;
        else if(tl2->t.symbol == '-') op = 0;
        else if(tl2->t.symbol == '=') {
            op = 1;
            eqExists = 1;
        }
        tl2 = tl2->next;
    }

  if(tl2->tt == Number) {
      if(tl2->next != NULL && tl2->next->tt == Identifier){
        temp = tl2->t.number;
      }
      else {
        nums += ((!op && eqExists) || (op && !eqExists)) ? tl2->t.number : -(tl2->t.number);
      }
      tl2 = tl2->next; 
  }

  //Looks at current identifier, and also determines if there is another identifier, and if it has a higher power than the original
  if(tl2 != NULL && tl2->tt == Identifier) {
    char *currID = tl2->t.identifier;
    if(id == NULL) {
        id = malloc((strlen(currID)+1) * sizeof(char));
        strncpy(id, currID, strlen(currID)+1);
        numVars++;
    }
    if(strcmp(id, currID) != 0) {
        numVars++;
        if(numVars != 1) break;
    }
    tl2 = tl2-> next; 
    if(tl2 != NULL && tl2->tt == Symbol && tl2->t.symbol == '^') {
        tl2 = tl2->next;
        if(degree < tl2->t.number){
          degree = tl2->t.number;
        }
        if(tl2->t.number == 0) {
            nums += ((!op && eqExists) || (op && !eqExists)) ? temp : -(temp);
        }
        else if(tl2->t.number == 1) {
            idnums += ((!op && eqExists) || (op && !eqExists)) ? temp : -(temp);
        }
        tl2 = tl2->next;
      } else {
          idnums += ((!op && eqExists) || (op && !eqExists)) ? temp : -(temp);
          if(degree < 1) degree = 1;
      }
      temp = 1; 
    }
    }

    if(numVars == 1) {
      solveEq(degree, nums, idnums);
    }
    else {
      printf(", but not in 1 variable\n");
    }

  return 0;
}


void recognizeEquations(char *ar) {
  int degree = -1, numVars = 0, eqExists = 0, op =1;
  List tl, tl1, tl2;
  char *id = NULL;
  double nums = 0, idnums = 0, temp = 1;

  tl = tokenList(ar);
  printList(tl);
  tl1 = tl2 = tl;

  if(tl1 == NULL || !acceptEquation(&tl1)){
    printf("this is not an equation\n");
  } else { 
      printf("this is an equation");
      verifyEq(tl1, tl2, degree, numVars, eqExists, op, nums, idnums, temp, id);
      free(id);
  }
  freeTokenList(tl);
}

int main(int argc, char **argv){
    
  char *ar;

  printf("give an equation: ");
  ar = readInput();
  while (ar[0] != '!') {
    recognizeEquations(ar);

    free(ar);
    printf("\ngive an equation: ");
    ar = readInput();
  }
  printf("good bye");
  printf("\n");
    return 0;
}
