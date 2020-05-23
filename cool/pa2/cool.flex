/*
 *  The scanner definition for COOL.
 */

/*
 *  Stuff enclosed in %{ %} in the first section is copied verbatim to the
 *  output, so headers and global definitions are placed here to be visible
 * to the code in the file.  Don't remove anything that was here initially
 */
%{
#include <cool-parse.h>
#include <stringtab.h>
#include <utilities.h>

/* The compiler assumes these identifiers. */
#define yylval cool_yylval
#define yylex  cool_yylex

/* Max size of string constants */
#define MAX_STR_CONST 1025
#define YY_NO_UNPUT   /* keep g++ happy */

extern FILE *fin; /* we read from this file */

/* define YY_INPUT so we read from the FILE fin:
 * This change makes it possible to use this scanner in
 * the Cool compiler.
 */
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
	if ( (result = fread( (char*)buf, sizeof(char), max_size, fin)) < 0) \
		YY_FATAL_ERROR( "read() in flex scanner failed");

char string_buf[MAX_STR_CONST]; /* to assemble string constants */
char *string_buf_ptr;

extern int curr_lineno;
extern int verbose_flag;

extern YYSTYPE cool_yylval;

/*
 *  Add Your own definitions here
 */
%}

/*
 * Define names for regular expressions here.
 */

DARROW          =>
BEGIN_COMMENT   "(*"
END_COMMENT     "*)"
DIGIT           [0-9]
ID              [a-zA-z][a-zA-z0-9]*
CHAR            [a-zA-z]

%%

 /*
  *  Nested comments
  */


 /*
  *  The multiple-character operators.
  */
{DARROW}		{ return (DARROW); }

 /*
  * Keywords are case-insensitive except for the values true and false,
  * which must begin with a lower-case letter.
CLASS = 258,
ELSE = 259,
FI = 260,
IF = 261,
IN = 262,
INHERITS = 263,
LET = 264,
LOOP = 265,
POOL = 266,
THEN = 267,
WHILE = 268,
CASE = 269,
ESAC = 270,
OF = 271,
DARROW = 272,
NEW = 273,
ISVOID = 274,
STR_CONST = 275,
INT_CONST = 276,
BOOL_CONST = 277,
TYPEID = 278,
OBJECTID = 279,
ASSIGN = 280,
NOT = 281,
LE = 282,
ERROR = 283,
LET_STMT = 285
  */
(?i:class)  return (CLASS);
else        return (ELSE);
fi          return (FI);
if          return (IF);
 /* in          return (IN); */
inherits    return (INHERITS);
let         return (LET);
loop        return (LOOP);
pool        return (POOL);
then        return (THEN);
while       return (WHILE);
case        return (CASE);
esac        return (ESAC);
of          return (OF);
new         return (NEW);
"<-"        return (ASSIGN);


 /*
  *  String constants (C syntax)
  *  Escape sequence \c is accepted for all characters c. Except for
  *  \n \t \b \f, the result is c.
  *
  */

 /* {CHAR}+     { return (STR_CONST); } */
 /* {DIGIT}+    { cool_yylval.symbol = inttable.add_string(yytext); return (INT_CONST); } */
 /* true|false  { return (BOOL_CONST); } */

{DIGIT}+ {
  cool_yylval.symbol = inttable.add_string(yytext);
  return (INT_CONST);
}

true|false {
  cool_yylval.boolean = (strcmp(yytext, "true") == 0 ? 1 : 0);
  return (BOOL_CONST);
}

 /* \".*\" {
   cool_yylval.symbol = stringtable.add_string(yytext);
   return (STR_CONST);
 } */

%%
