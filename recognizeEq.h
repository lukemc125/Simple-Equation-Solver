/* recognizeExp.h, Gerard Renardel, 29 January 2014 */

#ifndef RECOGNIZEEQ_H
#define RECOGNIZEEQ_H

int acceptNumber(List *lp);
int acceptIdentifier(List *lp);
int acceptCharacter(List *lp, char c);
int acceptExpression(List *lp);
int acceptEquation(List *lp);
void recognizeEquations();

#endif
