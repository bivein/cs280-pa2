/* 
 * parser.h
 * Programming Assignment 2
 * Spring 2026
*/

#ifndef PARSER_H_
#define PARSER_H_

#include <iostream>

using namespace std;

#include "lexSP26.h"

extern bool Prog(istream& in, int& line);
extern bool Block(istream& in, int& line);
extern bool ConstPart(istream& in, int& line);
extern bool DeclPart(istream& in, int& line);
extern bool VarPart(istream& in, int& line);
extern bool ConstDef(istream& in, int& line);
extern bool DeclStmt(istream& in, int& line);
extern bool Stmt(istream& in, int& line);
extern bool StructuredStmt(istream& in, int& line);
extern bool CompStmt(istream& in, int& line);
extern bool SimpleStmt(istream& in, int& line);
extern bool WriteLnStmt(istream& in, int& line);
extern bool WriteStmt(istream& in, int& line);
extern bool ReadLnStmt(istream& in, int& line);
extern bool IfStmt(istream& in, int& line);
extern bool AssignStmt(istream& in, int& line);
extern bool Variable(istream& in, int& line);
extern bool ExprList(istream& in, int& line);
extern bool Expr(istream& in, int& line);
extern bool IdentList(istream& in, int& line);
extern bool VarList(istream& in, int& line);
extern bool SimpleExpr(istream& in, int& line);
extern bool Term(istream& in, int& line);
extern bool SFactor(istream& in, int& line);
extern bool Factor(istream& in, int& line, int sign);
extern int ErrCount();

#endif /* PARSE_H_ */
