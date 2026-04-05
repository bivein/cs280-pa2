

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

	if (AssignStmt(in, line)) {
		return true;
	}
	else if (ReadLnStmt(in, line)) {
		return true;
	}
	else if (WriteLnStmt(in, line)) {
		return true;
	}
	else if (WriteStmt(in, line)) {
		return true;
	}
	else {
		return false;
	}
}

bool WriteLnStmt(istream& in, int& line) { // WriteLnStmt ::= WRITELN ( ExprList ) 
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok.GetToken() != WRITELN) {
		ParseError(line, "Missing writeln in writeln statement");
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() != LPAREN) {
		ParseError(line, "Missing open parentheses in writeln statement");
		return false;
	}

	if (!ExprList(in, line)) {
		ParseError(line, "Missing expression list in WRITELN");
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() != RPAREN) {
		ParseError(line, "Mising closing parentheses in writeln statement");
		return false;
	}
	return true;
}

bool WriteStmt(istream& in, int& line) { // WriteStmt ::= WRITE ( ExprList )
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok.GetToken() != WRITE) {
		ParseError(line, "Missing WRITE in write statement");
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() != LPAREN) {
		ParseError(line, "Missing open parentheses in write statement");
		return false;
	}

	if (!ExprList(in, line)) {
		ParseError(line, "Missing expresssion list in write statement");
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() != RPAREN) {
		ParseError(line, "Missing closing parentheses in write statement");
		return false;
	}

	return true;
}

bool ReadLnStmt(istream& in, int& line) { // ReadLnStmt ::= READLN ( VarList )
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok.GetToken() != READLN) {
		ParseError(line, "Missing READLN in readln statement");
		return false;
	}

	tok = Parser::GetNextToken(in, line);

	if (tok.GetToken() != LPAREN) {
		ParseError(line, "Missing opening parentheses in readln statement");
		return false;
	}

	if (!VarList(in, line)) {
		ParseError(line, "Missing variable list in readln statement");
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() != RPAREN) {
		ParseError(line, "Missing closing parentheses in readln statement");
		return false;
	}

	return true;
}


bool IfStmt(istream& in, int& line) { // IfStmt ::= IF Expr THEN Stmt [ ELSE Stmt ]
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok.GetToken() != IF) {
		ParseError(line, "Missing if keyword in if statement");
		return false;
	}

	if (!Expr(in, line)) {
		ParseError(line, "Missing expression in if statement");
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() != THEN) {
		ParseError(line, "Missing then keyword in if statement");
		return false;
	}

	if (!Stmt(in, line)) {
		ParseError(line, "Missing statement in if statement");
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() == ELSE) {
		if (!Stmt(in, line)) {
			ParseError(line, "Missing statement after else in if statement");
			return false;
		}
	}
	else {
		Parser::PushBackToken(tok);
	}

	return true;
}

bool AssignStmt(istream& in, int& line) { // AssignStmt ::= Variable := Expr
	LexItem tok = Parser::GetNextToken(in, line);
	Parser::PushBackToken(tok);

	if (!Variable(in, line)) {
		ParseError(line, "Missing variable in assignment statement");
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() != ASSOP) {
		ParseError(line, "Missing assignment operator in assignment statement");
		return false;
	}

	if (!Expr(in, line)) {
		ParseError(line, "Missing expression in assignment statement");
		return false;
	}

	return true;
}

bool Variable(istream& in, int& line) { // Variable ::= IDENT
	LexItem tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() != IDENT) {
		ParseError(line, "Missing identifier for variable");
		return false;
	}
	return true;
}

bool ExprList(istream& in, int& line) { // ExprList ::= Expr { , Expr }
	LexItem tok = Parser::GetNextToken(in, line);
	Parser::PushBackToken(tok);

	if (!Expr(in, line)) {
		ParseError(line, "Missing expression in expression list");
		return false;
	}

	while (true) {

		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() != COMMA) {
			ParseError(line, "Missing comma in expression list");
			break;
		}

		if (!Expr(in, line)) {
			ParseError(line, "Missing expression in expression list");
			return false;
		}
	}
	return true;
}

bool Expr(istream& in, int& line) { // Expr ::= RelExpr ::= SimpleExpr [ ( = | < | > ) SimpleExpr ]
	LexItem tok = Parser::GetNextToken(in, line);
	Parser::PushBackToken(tok);

	if (!SimpleExpr(in, line)) {
		ParseError(line, "Missing simple expression in expression");
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	
	if (tok.GetToken() == EQ ||
		tok.GetToken() == LTHAN ||
		tok.GetToken() == GTHAN) {
		if (!SimpleExpr(in, line)) {
			ParseError(line, "Missing simpleexpr in expr");
			return false;
		}

	}
	else {
		Parser::PushBackToken(tok);
	}

	return true;
}

bool IdentList(istream& in, int& line) { // IdentList ::= IDENT {, IDENT}
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok.GetToken() != IDENT) {
		ParseError(line, "Missing identifier in identlist");
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	while (true) {
		if (tok.GetToken() != COMMA) {
			Parser::PushBackToken(tok);
			break;
		}
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() != IDENT) {
			ParseError(line, "Missing identifier after comma");
			return false;
		}
	}
	return true;
}

bool VarList(istream& in, int& line) { // VarList ::= Variable {, Variable }

	if (!Variable(in, line)) {
		ParseError(line, "Missing variable in varlist");
		return false;
	}

	while (true) {
		LexItem tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() != COMMA) {
			Parser::PushBackToken(tok);
			break;
		}
		if (!Variable(in, line)) {
			ParseError(line, "Missing variable after comma");
			return false;
		}
	}
	return true;
}

bool SimpleExpr(istream& in, int& line) { // SimpleExpr :: Term { ( + | - | OR ) Term }
	if (!Term(in, line)) {
		ParseError(line, "Missing term in simple expression");
		return false;
	}

	while (true) {
		LexItem tok = Parser::GetNextToken(in, line);

		if (tok.GetToken() != PLUS &&
			tok.GetToken() != MINUS &&
			tok.GetToken() != OR) {
			Parser::PushBackToken(tok);
			break;
		}

		if (!Term(in, line)) {
			ParseError(line, "Missing term after operator");
			return false;
		}
	}

	return true;
}

bool Term(istream& in, int& line) { // Term ::= SFactor { ( * | / | DIV | MOD | AND ) SFactor }
	if (!SFactor(in, line)) {
		ParseError(line, "Missing SFactor in term");
		return false;
	}

	while (true) {
		LexItem tok = GetNextToken(in, line);

		if (tok.GetToken() != MULT &&
			tok.GetToken() != DIV &&
			tok.GetToken() != IDIV &&
			tok.GetToken() != MOD &&
			tok.GetToken() != AND&&
			) {
			Parser::PushBackToken(tok);
			break;
		}
		
		if (!SFactor(in, line)) {
			ParseError(line, "Missing sfactor following operator");
			return false;
		}
	}
	return true;
}

bool SFactor(istream& in, int& line) { // SFactor ::= [( - | + | NOT )] Factor 
	LexItem tok = Parser::GetNextToken(in, line);

	int sign = 1;
	bool notSign = false;

	if (tok.GetToken() == MINUS)
		sign = -1;
	else if (tok.GetToken() == PLUS)
		sign = 1;
	else if (tok.GetToken() == NOT) {
		notSign = true;
		sign = 0;
	}
	else {
		Parser::PushBackToken(tok);
	}

	if (!Factor(in, line, sign) {
		ParseError(line, "Missing factor after unary operator");
		return false;
	}

	return true;
}
bool Factor(istream& in, int& line, int sign) { // Factor ::= IDENT | ICONST | RCONST | SCONST | BCONST | CCONST | (Expr)

}
int ErrCount();
