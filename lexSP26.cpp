/*
 * lexSP26.cpp
 * Lexical Analyzer for Pascal-Like Language
 * CS280 - Spring 2026
 */

#include <cctype>
#include <map>

using std::map;
using namespace std;

#include "lexSP26.h"

LexItem id_or_kw(const string& lexeme , int linenum)
{
	map<string,Token> kwmap = {
		{ "writeln", WRITELN},
		{ "write", WRITE},
		{ "if", IF },
		{ "else", ELSE },
		{ "then", THEN },
		{ "div", IDIV },
		{ "mod", MOD },
		{ "and", AND },
		{ "or", OR },
		{ "not", NOT },
		{ "true", TRUE },
		{ "false", FALSE },
		{ "integer", INTEGER },
		{ "real", REAL },
		{ "string", STRING },
		{ "boolean", BOOLEAN },
		{ "begin", BEGIN },
		{ "end", END },
		{ "var", VAR },
		{ "program", PROGRAM },
		{ "const", CONST },
		{ "readln", READLN },
		{ "char", CHAR }
	};
	string strlexeme = lexeme;
	
	Token tt ;
	for(int i = 0; i < strlexeme.length(); i++)
	{
		strlexeme[i] = tolower(strlexeme[i]);
	}
	
	tt = IDENT;
	auto kIt = kwmap.find(strlexeme);
	if( kIt != kwmap.end() )
	{
		tt = kIt->second;
	}
	
	if(tt == TRUE || tt == FALSE)	
		tt = BCONST;
	return LexItem(tt, lexeme, linenum);
}

map<Token,string> tokenPrint = {
		{PROGRAM, "PROGRAM" },
		{WRITELN, "WRITELN"},
		{WRITE, "WRITE"},
		{READLN, "READLN"},
		{ IF, "IF" },
		{ ELSE, "ELSE" },	
		{ IDENT, "IDENT" },
		{ END, "END" },
		{ BEGIN, "BEGIN" },
		{ INTEGER, "INTEGER" },
		{ REAL, "REAL" },
		{ CHAR, "CHAR" },
		{ BOOLEAN, "BOOLEAN" },
		{ ICONST, "ICONST" },
		{ RCONST, "RCONST" },
		{ SCONST, "SCONST" },
		{ BCONST, "BCONST"},
		{ CONST, "CONST" },
		{ TRUE, "TRUE" },
		{ FALSE, "FALSE" },
		{ THEN, "THEN" },
		{ VAR, "VAR" },
		{ CONST, "CONST" },
			
		{ PLUS, "PLUS" },
		{ MINUS, "MINUS" },
		{ MULT, "MULT" },
		{ DIV, "DIV" },
		{ ASSOP, "ASSOP" },
		{ EQ, "EQ" },
		{ GTHAN, "GTHAN" },
		{ LTHAN, "LTHAN" },
		{ AND, "AND" },
		{ OR, "OR"},
		{ NOT, "NOT" },
		{ IDIV, "IDIV" },
		{ MOD, "MOD" },
		            
		{ COMMA, "COMMA" },
		{ SEMICOL, "SEMICOL" },
		{ LPAREN, "LPAREN" },
		{ RPAREN, "RPAREN" },
		{ DOT, "DOT" },
		{ COLON, "COLON" },
		{ LBRACE, "LBRACE" },
		{ RBRACE, "RBRACE" },
		{ ERR, "ERR" },
		{ DONE, "DONE" },
};

ostream& operator<<(ostream& out, const LexItem& tok) {
	
	Token tt = tok.GetToken();
	out << tokenPrint[ tt ];
	if( tt == ICONST || tt == RCONST || tt == BCONST) {
		out << ": (" << tok.GetLexeme() << ")";
	}
	else if(tt == IDENT ){
		out << ": <" << tok.GetLexeme() << ">";
	}
	else if(tt == SCONST){
		out << ": \'" << tok.GetLexeme() << "\'";
	}
	else if(tt == CCONST){
		out << ": \'" << tok.GetLexeme() << "\'";
	}
	else if( tok == ERR ) {
		cout << ": Error in line (" << tok.GetLinenum()+1 << ") " << tok.GetLexeme() ;
	}
	else
	{
		out << ": \"" << tok.GetLexeme() << "\"";
	}
	
	return out;
}

LexItem getNextToken(istream& in, int& linenum)
{
	enum TokState { START, INID, INSTRING, ININT, INREAL, INEXP, INCOMMENT1, INCOMMENT2 } lexstate = START;
	string lexeme;
	char ch, nextch, nextchar, prevch;
	Token tt;
		
    while(in.get(ch)) {
    	
		switch( lexstate ) {
		case START:
			if( ch == '\n' )
			{
				linenum++;
			}	
                
			if( isspace(ch) )
				continue;

			lexeme = ch;

			if( isalpha(ch) ) {
				lexeme = ch;
				lexstate = INID;
			}
			else if( ch == '\'' ) {
				lexstate = INSTRING;
			}
			
			else if( isdigit(ch) ) {
				lexstate = ININT;
			}
			else if( ch == '{' ) {
				lexstate = INCOMMENT1;
			}
			else if(ch == '(' && in.peek() == '*')
			{
				lexstate = INCOMMENT2;
				in.get(ch); 
				lexeme += ch;
			}				
			else {
				tt = ERR;
				switch( ch ) {
				case '+':
					tt = PLUS;
                    break;  
					
				case '-':
					tt = MINUS;
                    break; 
					
				case '*':
								
					tt = MULT;
					break;

				case '/':
					tt = DIV;
					break;
									
				case ':':
					tt = COLON;
					nextchar = in.peek();
					if(nextchar == '='){
						in.get(ch);
						lexeme += ch;
						tt = ASSOP;
						break;
					}
					break;
				
				case '=':
					tt = EQ;
					break;
				case '(':
					tt = LPAREN;
					break;			
				case ')':
					tt = RPAREN;
					break;
				
				case ';':
					tt = SEMICOL;
					break;
					
				case ',':
					tt = COMMA;
					break;
					
				case '>':
					tt = GTHAN;
					break;
				
				case '<':
					tt = LTHAN;
					break;
					
				case '.':
					tt = DOT;
					break;
				
				}//end of switch (ch)
				
				if(tt == ERR)
				{
					return LexItem(tt, "Invalid character for starting a token \"" + lexeme + "\"", linenum);
				}
				return LexItem(tt, lexeme, linenum);
			}//end of else
			
			break;	

		case INID:
			if( isalpha(ch) || isdigit(ch) || ch == '_' || ch == '$') {
							
				lexeme += ch;
			}
			else {
				in.putback(ch);
				return id_or_kw(lexeme, linenum);
			}
			break;
					
		case INSTRING:
                          
			if( ch == '\n' ) {
				return LexItem(ERR, "New line is not allowed within string literal \"" + lexeme + "\"" , linenum);
			}
			lexeme += ch;
			if( ch == '\'' ) {
				lexeme = lexeme.substr(1, lexeme.length()-2);
				return LexItem(SCONST, lexeme, linenum);
			}
			break;

		case ININT:
			if( isdigit(ch) ) {
				lexeme += ch;
			}
			else if(ch == '.' && isdigit(in.peek())) {
				lexstate = INREAL;
				lexeme += ch;
			}
			else {
				in.putback(ch);
				return LexItem(ICONST, lexeme, linenum);
			}
			break;
		
		case INREAL:
				
			if (isdigit(ch) ){
				lexeme += ch;
			}
			else if(ch == '.' && !isdigit(in.peek()) ){
				in.putback(ch);
				
				return LexItem(RCONST, lexeme, linenum);
			}
			else if(ch == 'E' || ch == 'e')
			{
				nextch = in.peek();
				if(isdigit(nextch) || nextch == '+' || nextch == '-' )
				{
					lexstate =  INEXP;
					prevch = ch;
					lexeme += ch;
					
				}
				else if(nextch == 'E' || nextch == 'e')
				{
					lexeme += ch;
					in.get(ch);
					lexeme += ch;
					return LexItem(ERR, ("Invalid exponent for a floating-point constant \"" + lexeme+ "\""  ), linenum);
				}
			}			
			else if(ch == '.' && isdigit(in.peek())){
				lexeme += ch;
				in.get(ch);
				lexeme += ch;
				return LexItem(ERR, "Invalid floating-point constant \"" + lexeme + "\"" , linenum);
			}
			else {
				in.putback(ch);
				return LexItem(RCONST, lexeme, linenum);
			}
			
			break;
		
		case INEXP:
			
			if((ch == '+' || ch == '-') && (prevch =='E' || prevch =='e'))
			{
				lexeme += ch;
				prevch = ch;
			}
			else if((ch == '+' || ch == '-') && (prevch =='+' || prevch =='-'))
			{
				lexeme += ch;
				return LexItem(ERR, "Invalid exponent for floating-point constant \"" + lexeme + "\"" , linenum);
			}
			else if (isdigit(ch) && (prevch == 'E' || prevch == 'e' || prevch == '+' || prevch == '-')) {
				lexeme += ch;
				prevch = ch;
				
			}
			else if((ch == 'E' || ch == 'e' || ch == '.' || ch == '+' || ch == '-') && (isdigit(prevch) ))
			{
				lexeme += ch;
				return LexItem(ERR, "Invalid exponent for floating-point constant \"" + lexeme + "\"" , linenum);
			}
			else 
			{
				in.putback(ch);
				return LexItem(RCONST, lexeme, linenum);
			}
			break;
					
		case INCOMMENT1: //{ . . . } comments
			if(ch == '\n') 
				linenum++;
				
			else if( ch == '}' ) {
               	lexeme += ch;
               	lexstate = START;
			}
			
			else if( ch == '{')
			{
				lexeme += ch;
				return LexItem(ERR, "Invalid nested comments \"" + lexeme + "\"", linenum);
			}
					
			break;
			
		case INCOMMENT2: //(* . . .  *) comments
			if(ch == '\n') 
				linenum++;
				
			else if( ch == '*' && in.peek() == ')') {
               	lexeme += ch;
				in.get(ch);
               	lexeme += ch;
               	lexstate = START;
			}
			
			else if( ch == '(' && in.peek() == '*')
			{
				lexeme += ch;
				in.get(ch);
				lexeme += ch;
				return LexItem(ERR, "Invalid nesting of comments \"" + lexeme + "\"", linenum);
			}	
			break;
		}
	}//end of while loop
	if(lexstate == INCOMMENT1 || lexstate == INCOMMENT2)
	{
		return LexItem(ERR, "Missing closing symbol(s) for a comment \"" + lexeme+ "\"" , linenum);
	}
	if( in.eof() )
		return LexItem(DONE, "", linenum);
		
	return LexItem(ERR, "some strange I/O error", linenum);
}





