%{ /*** C/C++ Declarations ***/

#include "common.hh"
#include "model.hh"
#include "scanner.hh"
#include <iostream>

/* import the parser's token type into a local typedef */
typedef yy::parser::token token;
typedef yy::parser::token_type token_type;

/* By default yylex returns int, we use token_type. Unfortunately yyterminate
 * by default returns 0, which is not of token_type. */
#define yyterminate() return token::END

/* This disables inclusion of unistd.h, which is not available under Visual C++
 * on Win32. The C++ scanner uses STL streams instead. */
#define YY_NO_UNISTD_H

%}

/*** Flex Declarations and Options ***/

/* enable c++ scanner class generation */
%option c++

/* change the name of the scanner class. results in "XigenFlexLexer" */
%option prefix="Xigen"

/* the manual says "somewhat more optimized" */
%option batch

/* enable scanner to generate debug output. disable this for release
 * versions. */
%option debug

/* no support for include files is planned */
%option yywrap nounput 

/* enables the use of start condition stacks */
%option stack

/* The following paragraph suffices to track locations accurately. Each time
 * yylex is invoked, the begin position is moved onto the end position. */
%{
#define YY_USER_ACTION yylloc->columns(yyleng);
%}

%{
/* debug facility */
#define XI_DUMPLOC { std::cerr << "yyleng: " << yyleng << " loc: " << *yylloc << std::endl; }
%}

%{
#define XI_STR { yylval->stringVal = new std::string(yytext, yyleng); };
%}

%{
	using namespace std;
	std::string g_comment;
%}


%x comment

%% /*** Regular Expressions Part ***/

 /* code to place at the beginning of yylex() */
%{
	// reset location
	yylloc->step();
%}

	/*** BEGIN XIGEN - Change the xigen lexer rules below ***/

answer			{ return token::ANSWER; }
calb			{ return token::CALB; }
command			{ return token::COMMAND; }
crc			{ return token::CRC; }
defaults		{ return token::DEFAULTS; }
dualsync		{ return token::DUALSYNC; }
fields			{ return token::FIELDS; }
flag			{ return token::FLAG; }
flagset			{ return token::FLAGSET; }
inline			{ return token::INLINE; }
is			{ return token::IS; }
lock			{ return token::LOCK; }
metalen			{ return token::METALEN; }
normal			{ return token::NORMAL; }
of			{ return token::OF; }
protocol		{ return token::PROTOCOL; }
public			{ return token::PUBLIC; }
publicstruct		{ return token::PUBLICSTRUCT; }
reader			{ return token::READER; }
reserved		{ return token::RESERVED; }
service			{ return token::SERVICE; }
serviceanswer		{ return token::SERVICEANSWER; }
serviceresult		{ return token::SERVICERESULT; }
universal		{ return token::UNIVERSAL; }
with			{ return token::WITH; }
without			{ return token::WITHOUT; }
writer			{ return token::WRITER; }

int64s			{ XI_STR; return token::TYPE_INT64S; }
int64u			{ XI_STR; return token::TYPE_INT64U; }
int32s			{ XI_STR; return token::TYPE_INT32S; }
int32u			{ XI_STR; return token::TYPE_INT32U; }
int16u			{ XI_STR; return token::TYPE_INT16U; }
int16s			{ XI_STR; return token::TYPE_INT16S; }
int8u			{ XI_STR; return token::TYPE_INT8U; }
int8s			{ XI_STR; return token::TYPE_INT8S; }
float			{ XI_STR; return token::TYPE_FLOAT; }
double			{ XI_STR; return token::TYPE_DOUBLE; }
char			{ XI_STR; return token::TYPE_CHAR; }
byte			{ XI_STR; return token::TYPE_BYTE; }
cfloat			{ XI_STR; return token::TYPE_CFLOAT; }
cdfloat			{ XI_STR; return token::TYPE_CDFLOAT; }



[A-Za-z][A-Za-z0-9_-]* {
	XI_STR;
	return token::IDENTIFIER;
}

"\""+[A-Za-z][A-Za-z0-9_,.-]*+"\"" {
	yylval->stringVal = new std::string(yytext+1, yyleng-2);
	return token::STRING; 
}

[0-9]+ {
	yylval->integerVal = strtoul(yytext, NULL, 10);
	return token::INTEGER;
}

0x[0-9a-zA-Z]+ {
	yylval->integerVal = strtoul(yytext, NULL, 16);
	return token::HEXINTEGER;
}

"//@"[{}] {
	XI_STR;
	return token::COMMENT_SECTION;
}

"/**" {
	BEGIN(comment);
	g_comment.assign( yytext, yyleng );
	yylloc->step();
}

<comment>[^*\n]*		g_comment.append( yytext, yyleng ); /* eat anything that's not a '*' */
<comment>"*"+[^*/\n]* 		g_comment.append( yytext, yyleng ); /* eat up '*'s not followed by '/'s */
<comment>\n 			g_comment.append( yytext, yyleng ); yylloc->lines(); yylloc->step(); 

<comment>"*/"[ \t]*\n {		
	g_comment.append( yytext, yyleng-1 ); /* eat last cr */
	yylloc->lines(); yylloc->step(); 
	BEGIN(INITIAL); 
	yylval->stringVal = new std::string( g_comment );
	/*std::cerr << g_comment << std::endl;*/
	return token::COMMENT_DOXYGEN;
}


 /* gobble up white-spaces */
[ \t\r]+ {
	yylloc->step();
}

 /* gobble up end-of-lines */
\n {
	yylloc->lines(yyleng); yylloc->step();
	return token::EOL;
}

 /* pass all other characters up to bison */
. {
	return static_cast<token_type>(*yytext);
}

 /*** END XIGEN - Change the xigen lexer rules above ***/

%% /*** Additional Code ***/


namespace yy {


Scanner::Scanner(std::istream* in,
		 std::ostream* out)
	: XigenFlexLexer(in, out)
{
}

Scanner::~Scanner()
{
}

void Scanner::set_debug(bool b)
{
	yy_flex_debug = b;
}

}

/* This implementation of XigenFlexLexer::yylex() is required to fill the
 * vtable of the class XigenFlexLexer. We define the scanner's main yylex
 * function via YY_DECL to reside in the Scanner class instead. */

#ifdef yylex
#undef yylex
#endif

int XigenFlexLexer::yylex()
{
	std::cerr << "in XigenFlexLexer::yylex() !" << std::endl;
	return 0;
}

/* When the scanner receives an end-of-file indication from YY_INPUT, it then
 * checks the yywrap() function. If yywrap() returns false (zero), then it is
 * assumed that the function has gone ahead and set up `yyin' to point to
 * another input file, and scanning continues. If it returns true (non-zero),
 * then the scanner terminates, returning 0 to its caller. */

int XigenFlexLexer::yywrap()
{
	return 1;
}

/* vim: set ts=8 sw=8: */
