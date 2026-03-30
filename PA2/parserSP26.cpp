

#include "parserSP26.h"

map<string, bool> defVar;
map<string, bool> defConst;

namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken(istream& in, int& line) {
		if( pushed_back ) {
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem & t) {
		if( pushed_back ) {
			abort();
		}
		pushed_back = true;
		pushed_token = t;	
	}

}

static int error_count = 0;

int ErrCount()
{
    return error_count;
}

void ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}


bool Prog(istream& in, int& line); // Prog ::= PROGRAM IDENT ; Block .
bool Block(istream& in, int& line); // Block ::= [ DeclPart ] CompStmt
bool ConstPart(istream& in, int& line); // ConstPart ::= CONST ConstDef { ; ConstDef } ;
bool DeclPart(istream& in, int& line); // DeclPart ::= [ ConstPart ] [ VarPart ]
bool VarPart(istream& in, int& line); // VarPart ::= VAR DeclStmt { ; DeclStmt } ;

bool ConstDef(istream& in, int& line) { // ConstDef ::= IDENT = Expr
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok.GetToken() != IDENT) { // IDENT Check
		ParseError(line, "Missing identifier in const definition");
		return false;
	}

	std::string var = tok.GetLexeme(); // Store name
	if (defConst.find(var) != defConst.end()) {
		ParseError(line, "Constant redefinition: " + var);
		return false;
	}
	else {
		defConst.insert(std::make_pair(var, true));
	}
	

	tok = Parser::GetNextToken(in, line); // Assignment check
	if (tok.GetToken() != EQ) {
		ParseError(line, "Missing assignment operator in const definition");
		return false;
	}

	if (!Expr(in, line)) { 
		ParseError(line, "Missing expr in const def");
		return false;
	}

	return true;

}

bool DeclStmt(istream& in, int& line) { // DeclStmt ::= IDENT {, IDENT } : Type [:= Expr]
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok.GetToken() != IDENT) {
		ParseError(line, "Missing identifier in declaration statement");
		return false;
	}

	while (true) { // Zero or more IDENT check
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() != COMMA) {
			Parser::PushBackToken(tok);
			break;
		}

		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() != IDENT) {
			ParseError(line, "Missing identifier in declaration statement");
			return false;
		}
	}

	tok = Parser::GetNextToken(in, line); // Colon Check
	if (tok.GetToken() != COLON) {
		ParseError(line, "Missing colon in declaration statement");
		return false;
	}

	tok = Parser::GetNextToken(in, line); // Type Check
	if (tok.GetToken() != INTEGER &&
		tok.GetToken() != REAL &&
		tok.GetToken() != BOOLEAN && 
		tok.GetToken() != STRING && 
		tok.GetToken() != CHAR) {
		ParseError(line, "Invalid type in declaration");
		return false;
	}

	// Optional Expr

	tok = Parser::GetNextToken(in, line); // Expr Check
	if (tok.GetToken() == ASSOP) {
		if (!Expr(in, line)) {
			ParseError(line, "Missing expression in declaration statement");
			return false;
		}
	}
	else {
		Parser::PushBackToken(tok);
	}
	return true;
}

bool Stmt(istream& in, int& line) { // Stmt ::= SimpleStmt | StructuredStmt
	LexItem tok = Parser::GetNextToken(in, line);
	Parser::PushBackToken(tok);

	if (SimpleStmt(in, line)) {
		return true;
	}
	else if (StructuredStmt(in, line)) {
		return true;
	}
	else {
		ParseError(line, "Missing simple or structured statement");
		return false;
	}
}
bool StructuredStmt(istream& in, int& line) { // StructuredStmt ::= IfStmt | CompStmt 
	LexItem tok = Parser::GetNextToken(in, line);
	Parser::PushBackToken(tok);

	if (IfStmt(in, line)) {
		return true;
	}
	else if (CompStmt(in, line)) {
		return true;
	}
	else {
		return false;
	}
}
bool CompStmt(istream& in, int& line) { // CompStmt ::= BEGIN Stmt {; Stmt } END
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok.GetToken() != BEGIN) {
		ParseError(line, "Begin missing from compound statement");
		return false;
	}
	if (!Stmt(in, line)) {
		ParseError(line, "Missing statement from compound statement");
		return false;
	}

	while (true) {
		tok = Parser::GetNextToken(in, line);

		if (tok.GetToken() != SEMICOL) {
			Parser::PushBackToken(tok);
			break;
		}

		if (!Stmt(in, line)) {
			ParseError(line, "Missing statement from compound statement");
			return false;
		}
	}

	tok = Parser::GetNextToken(in, line);

	if (tok.GetToken() != END) {
		ParseError(line, "End missing from compound statement");
		return false;
	}

	return true;
}

bool SimpleStmt(istream& in, int& line) { // SimpleStmt ::= AssignStmt | ReadLnStmt | WriteLnStmt | WriteStmt
	LexItem tok = Parser::GetNextToken(in, line);
	Parser::PushBackToken(tok);


}
bool WriteLnStmt(istream& in, int& line); // WriteLnStmt ::= WRITELN ( ExprList )
bool WriteStmt(istream& in, int& line); // WriteStmt ::= WRITE ( ExprList )
bool ReadLnStmt(istream& in, int& line); // ReadLnStmt ::= ReadLnStmt ( VarList )
bool IfStmt(istream& in, int& line); // IfStmt ::= IF Expr THEN Stmt [ ELSE Stmt ]
bool AssignStmt(istream& in, int& line); // AssignStmt ::= Variable := Expr
bool Variable(istream& in, int& line); // Variable ::= IDENT
bool ExprList(istream& in, int& line); // ExprList ::= Expr { , Expr }
bool Expr(istream& in, int& line); // Expr ::= RelExpr ::= SimpleExpr [ ( = | < | > ) SimpleExpr ]
bool IdentList(istream& in, int& line); // IdentList ::= IDENT {, IDENT}
bool VarList(istream& in, int& line); // VarList ::= Variable {, Variable }
bool SimpleExpr(istream& in, int& line); // SimpleExpr :: Term { ( + | - | OR ) Term }
bool Term(istream& in, int& line); // Term ::= SFactor { ( * | / | DIV | MOD | AND ) SFactor }
bool SFactor(istream& in, int& line); // SFactor ::= [( - | + | NOT )] Factor
bool Factor(istream& in, int& line, int sign); // Factor ::= IDENT | ICONST | RCONST | SCONST | BCONST | CCONST | (Expr)
int ErrCount();
