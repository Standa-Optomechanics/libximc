%{ /*** C/C++ Declarations ***/


#include "common.hh"
#include "model.hh"
#include "parsercontext.hh"

using namespace xigen;

%}

/*** yacc/bison Declarations ***/

/* Require bison 2.3 or later */
%require "2.3"

/* add debug output code to generated parser. disable this for release
 * versions. */
%debug

/* start symbol is named "start" */
%start start

/* write out a header file containing the token defines */
%defines

/* use newer C++ skeleton file */
%skeleton "lalr1.cc"

/* use default namespace yy to keep compatibility with bison 2.3 */

/* keep track of the current position within the input */
%locations
%initial-action
{
	// initialize the initial location object
	@$.begin.filename = @$.end.filename = &driver.streamname;
};

/* The driver is passed by reference to the parser and to the scanner. This
 * provides a simple but effective pure interface, not relying on global
 * variables. */
%parse-param { class Driver& driver }

/* verbose error messages, drop for compatibility with 2.7 */
/*%define parse.error verbose*/

 /*** BEGIN XIGEN - Change the xigen grammar's tokens below ***/

%union {
	unsigned int 	integerVal;
	bool 		boolVal;
	std::string* 	stringVal;
	xigen::Command* 	nodeCommand;
	xigen::Field* 		nodeField;
	xigen::ArrayField*	nodeArrayField;
	xigen::Communicator* 	nodeCommunicator;
	xigen::Comments* 	nodeComments;
	xigen::Comment* 	nodeComment;
	xigen::VariableEnum::Type variableType;
	xigen::CalibrationEnum::Type calibrationType;
}

/* token declarations */
%token			END	 0	"end of file"
%token			EOL		"end of line"
%token <integerVal> 	INTEGER		"integer"
%token <integerVal> 	HEXINTEGER	"hexinteger"
%token <stringVal> 	STRING		"string"
%token <stringVal> 	IDENTIFIER	"identifier"

%token <stringVal>	ANSWER CALB COMMAND COMMENT_SECTION COMMENT_DOXYGEN CRC DEFAULTS DUALSYNC FIELDS FLAG FLAGSET INLINE IS LOCK METALEN NORMAL OF
%token <stringVal>	PROTOCOL PUBLIC PUBLICSTRUCT READER RESERVED SERVICE SERVICEANSWER SERVICERESULT UNIVERSAL WITH WITHOUT WRITER
%token <stringVal>	TYPE_INT64S TYPE_INT64U TYPE_INT32S TYPE_INT32U TYPE_INT16U TYPE_INT16S TYPE_INT8U TYPE_INT8S TYPE_BYTE TYPE_FLOAT TYPE_DOUBLE TYPE_CHAR TYPE_CFLOAT TYPE_CDFLOAT

%destructor	{ delete $$; } STRING 
%printer	{ debug_stream () << *$$; } IDENTIFIER 

/* type declarations */

%type <nodeCommand> 		command	command_header
%type <nodeField> 		field declaration declaration_helper flag_declaration reserved_declaration variable_declaration array_declaration constant_declaration
%type <nodeArrayField> 		array_declaration_pure 
%type <nodeCommunicator>	command_code	

%type <integerVal>		number 
%type <stringVal>		feature_single
%type <nodeComments>		comments
%type <nodeComment>		comment comment_doxygen comment_section

%type <variableType>		variable_type

%type <calibrationType>		calibration_type


/*%destructor { delete $$; } command command_header field*/
%printer 	{ debug_stream () << $$; } command command_header field declaration flag_declaration reserved_declaration variable_declaration
%printer 	{ debug_stream () << toString($$); } variable_type

 /*** END XIGEN - Change the xigen grammar's tokens above ***/

%{

#include "driver.hh"
#include "scanner.hh"

/* this "connects" the bison parser in the driver to the flex scanner class
 * object. it defines the yylex() function call to pull the next token from the
 * current lexer object of the driver context. */
#undef yylex
#define yylex driver.getLexer()->lex

%}

%% /*** Grammar Rules ***/

 /*** BEGIN XIGEN - Change the xigen grammar rules below ***/

number			: INTEGER | HEXINTEGER

variable_type		: TYPE_INT64U	{ $$ = VariableEnum::Int64u; }
			| TYPE_INT64S	{ $$ = VariableEnum::Int64s; }
			| TYPE_INT32U	{ $$ = VariableEnum::Int32u; }
			| TYPE_INT32S	{ $$ = VariableEnum::Int32s; }
			| TYPE_INT16U	{ $$ = VariableEnum::Int16u; }
			| TYPE_INT16S	{ $$ = VariableEnum::Int16s; }
			| TYPE_INT8U	{ $$ = VariableEnum::Int8u; }
			| TYPE_INT8S	{ $$ = VariableEnum::Int8s; }
			| TYPE_FLOAT	{ $$ = VariableEnum::Float; }
			| TYPE_DOUBLE	{ $$ = VariableEnum::Double; }
			| TYPE_CHAR	{ $$ = VariableEnum::Char; }
			| TYPE_BYTE	{ $$ = VariableEnum::Byte; }
			| TYPE_CFLOAT	{ $$ = VariableEnum::CFloat; }
			| TYPE_CDFLOAT	{ $$ = VariableEnum::CDFloat; }

calibration_type	: CALB		{ $$ = CalibrationEnum::calb; }
		 	| NORMAL 	{ $$ = CalibrationEnum::normal; }
		 	

flag_declaration	: variable_type FLAG IDENTIFIER OF IDENTIFIER
			{
				$$ = new FlagField($1, *$3, *$5); 
			}
			| variable_type FLAG IDENTIFIER 
			{
				$$ = new FlagField($1, *$3); 
			}

variable_declaration	: variable_type SERVICEANSWER IDENTIFIER
			{
				$$ = new DataField($1, *$3, DataField::Purpose::answer, CalibrationEnum::none); 
			}
			| variable_type SERVICERESULT IDENTIFIER
			{
				$$ = new DataField($1, *$3, DataField::Purpose::result, CalibrationEnum::none); 
			}
			| calibration_type variable_type IDENTIFIER
			{
				$$ = new DataField($2, *$3, DataField::Purpose::normal, $1); 
			}
			| variable_type IDENTIFIER
			{
				$$ = new DataField($1, *$2, DataField::Purpose::normal, CalibrationEnum::none); 
			}

constant_declaration	: variable_type IDENTIFIER '=' number
			{
				$$ = new ConstantField($1, *$2, $4); 
			}

array_declaration	: calibration_type array_declaration_pure
		  	{
				$2->setCalibrationType($1);
				$$ = $2;
			}
		  	| array_declaration_pure { $$ = $1; }

array_declaration_pure	: variable_type IDENTIFIER '[' number ']' METALEN
			{
				$$ = new ArrayField($1, *$2, $4, true); 
			}
			| variable_type IDENTIFIER '[' number ']'
			{
				$$ = new ArrayField($1, *$2, $4, false); 
			}
			| variable_type IDENTIFIER '[' '@' IDENTIFIER ']'
			{
				$$ = new ArrayField($1, *$2, *$5); 
			}

reserved_declaration	: RESERVED number
			{
				$$ = new ReservedField($2);
			}

command_code		: STRING '(' number ')'
	 		{ 
				$$ = new Communicator( *$1, $3 );
			}
	 		| HEXINTEGER '(' number ')'
	 		{ 
				$$ = new Communicator( $1, $3 );
			}

verb_reader		: READER command_code 
	 		{ 
				driver.getContext().currentCommand()->communicatorReader = $2;
			}

verb_writer		: WRITER command_code 
	 		{ 
				driver.getContext().currentCommand()->communicatorWriter = $2;
			}

verb_universal		: UNIVERSAL command_code 
	 		{ 
				driver.getContext().currentCommand()->communicatorUniversal = $2;
			}

verb_ro			: 
			| verb_ro verb_reader
			| verb_ro verb_writer

verb_roc		: verb_ro
			| verb_universal

command_header		: SERVICE COMMAND STRING 
			{
				/* syntax deprecated */
				if (driver.trace_ast)
					driver.report(@2, "Found deprecated syntax for command " + *$3);
				$$ = new Command( *$3 );
				$$->setDefaultFeatures( driver.getContext().protocol.features );
				driver.getContext().addCommand( $$ );
			}
			| COMMAND STRING 
			{
				$$ = new Command( *$2 );
				$$->setDefaultFeatures( driver.getContext().protocol.features );
				driver.getContext().addCommand( $$ );
			}

feature_single		: CRC
			{
				$$ = new std::string("crc");
			}
			| ANSWER
			{
				$$ = new std::string("answer");
			}
			| INLINE
			{
				$$ = new std::string("inline");
			}
			| PUBLIC
			{
				$$ = new std::string("public");
			}
			| PUBLICSTRUCT
			{
				$$ = new std::string("publicstruct");
			}
			| LOCK
			{
				$$ = new std::string("lock");
			}
			| DUALSYNC
			{
				$$ = new std::string("dualsync");
			}

features_with		: WITH feature_single
		 	{
				driver.getContext().currentFeatures[*$2] = true;
			}
		 	| features_with ',' feature_single
		 	{
				driver.getContext().currentFeatures[*$3] = true;
			}

features_wout		: WITHOUT feature_single
		 	{
				driver.getContext().currentFeatures[*$2] = false;
			}
		 	| features_wout ',' feature_single
		 	{
				driver.getContext().currentFeatures[*$3] = false;
			}

features_distinct_impl	: features_with features_wout 
		 	| features_with
			| features_wout

features_distinct	: {
				driver.getContext().currentFeatures.clear();
			} features_distinct_impl

command_fields		: FIELDS { driver.getContext().inAnswer = false; } ':' EOL fields

command_result		: ANSWER { driver.getContext().inAnswer = true;  } ':' EOL fields

			/* get default values */
declaration		: flag_declaration
			| constant_declaration
			| variable_declaration
			| array_declaration
			| reserved_declaration

declaration_helper	: declaration
	 		{
				$$ = $1;
				if (driver.getContext().inAnswer)
					driver.getContext().currentCommand()->addFieldAnswer( $$ );
				else
					driver.getContext().currentCommand()->addFieldRequest( $$ );

			}
		   	
			/* get default values */
field			: declaration_helper EOL 
			| declaration_helper comment_doxygen
	 		{
				$$ = $1;
				$$->setComment( $2 );
			}

fields			: 
			| fields field 


flag			: IDENTIFIER '=' HEXINTEGER EOL 
	 		{
				ConstantField* flag = new ConstantField( VariableEnum::Int32u, *$1, $3 );
				driver.getContext().currentFlagset()->addFlag( flag );
			}
			| IDENTIFIER '=' HEXINTEGER comment_doxygen
	 		{
				ConstantField* flag = new ConstantField( VariableEnum::Int32u, *$1, $3 );
				flag->setComment( $4 );
				driver.getContext().currentFlagset()->addFlag( flag );
			}

flags			:
			| flags flag


flagset_header		: comment_doxygen FLAGSET IDENTIFIER 
	  		{
				driver.getContext().addFlagset( new Flagset( *$3, $1 ) );
			}

flagset			: flagset_header ':' EOL flags skips

flagsets		: 
	  		| flagsets flagset 


skips			: EOL
			| skips EOL

command_body 		: 
			| command_body features_distinct EOL
			{
				driver.getContext().currentCommand()->addFeatures( driver.getContext().currentFeatures );
			}
	 		| command_body command_fields
	 		| command_body command_result

comment_doxygen		: COMMENT_DOXYGEN
			{
				$$ = new Comment( true, *$1 );
			}

comment_section		: COMMENT_SECTION
		 	{
				$$ = new Comment( false, *$1 );
			}

comment			: comment_doxygen
	  		| comment_section

comments		: comment
			{
				$$ = new Comments();
				$$->push_back( *$1 );
			}
			| comments EOL
			| comments comment
			{
				*$$ = *$1;
				$$->push_back( *$2 );
			}

command 		: comments command_header verb_roc EOL command_body skips
			{
				$$ = $2;
				$$->setComments(*$1);
				$$->finish();
			}
			| command_header verb_roc EOL command_body skips
			{
				$$->finish();
			}

commands		: command
			{ 
				if (driver.trace_ast)
					driver.report(@1, "Command:\n" + $1->toString());
			}
			| commands command
			{ 
				if (driver.trace_ast)
					driver.report(@2, "Command:\n" + $2->toString());
			}

protocol_header		: PROTOCOL STRING 
		 	{
				driver.getContext().protocol.version = *$2;
			}
			| protocol_header EOL DEFAULTS features_distinct
			{
				driver.getContext().protocol.registerDefaults( driver.getContext().currentFeatures );
			}

/* TODO trailing comments are lost */
start 			: protocol_header skips flagsets commands comments
			{
				if (driver.trace_ast)
					driver.report(@1, "Protocol:\n" + driver.getContext().protocol.toString());
			}
			| protocol_header skips flagsets commands 
			{
				if (driver.trace_ast)
					driver.report(@1, "Protocol:\n" + driver.getContext().protocol.toString());
			}
	
 /*** END XIGEN - Change the xigen grammar rules above ***/

%% /*** Additional Code ***/

void yy::parser::error(const parser::location_type& l, const std::string& m)
{
	driver.report(l, "Error: " + m);
}

/* vim: set ts=8 sw=8: */
