/*
 *  The scanner definition for COOL.
 */
/*
 * For Flex 2.6.x %option noyywrap is required as well as linker flag e.g. -lfl must NOT be present.
 */
%option noyywrap

/*
 *  Stuff enclosed in %{ %} in the first section is copied verbatim to the
 *  output, so headers and global definitions are placed here to be visible
 * to the code in the file.  Don't remove anything that was here initially
 */
%{
#include <cool-parse.h>
#include <stringtab.h>
#include <utilities.h>

#include <fstream>

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

size_t buf_size();

#define RETURN_IF_STRING_TOO_LONG() \
  do { \
    if (buf_size() > MAX_STR_CONST - 1) { \
      yylval.error_msg = "String constant too long"; \
      BEGIN(INITIAL); \
      return (ERROR); \
    } \
  } while (0);

static int comment_level = 0;
%}

/*
 * Define names for regular expressions here.
 */

DARROW          =>
DIGIT           [0-9]
TYPEID          [A-Z][a-zA-Z0-9_]*
OBJECTID        [a-z][a-zA-Z0-9_]*

%x COMMENT
%x LINE_COMMENT
%x STRING

%%

 /* trigger special start conditions */

 /* multi-line comments */
<INITIAL,COMMENT,LINE_COMMENT>"(*" { ++comment_level; BEGIN(COMMENT); }
<COMMENT>{
[^\n(*]*  { } /* ignore comment section; \n is handled separately (see below) */
[()*]     { }
"*)"      { --comment_level; if (comment_level == 0) { BEGIN(INITIAL); } }
}
"*)"        { yylval.error_msg = "Unmatched *)"; return (ERROR); }

 /* single-line comment */
<INITIAL>"--" { BEGIN(LINE_COMMENT); }
<LINE_COMMENT>{
[^\n]*  { } /* ignore single-line comment section */
"\n"      { curr_lineno++; BEGIN(INITIAL); }
}

<INITIAL>\" { string_buf_ptr = string_buf; BEGIN(STRING); }
<STRING>{
\n {
  yylval.error_msg = "Unterminated string constant";
  ++curr_lineno;
  BEGIN(INITIAL);
  return (ERROR);
}
\0 {
  yylval.error_msg = "String contains null character";
  return (ERROR);
}
\" {
  *string_buf_ptr++ = '\0';
  cool_yylval.symbol = stringtable.add_string(string_buf, buf_size());
  BEGIN(INITIAL);
  return (STR_CONST);
}
 /*
  *  String constants (C syntax)
  *  Escape sequence \c is accepted for all characters c. Except for \n \t \b \f, the result is c.
  *
  */
\\n { RETURN_IF_STRING_TOO_LONG(); *string_buf_ptr++ = '\n'; }
\\t { RETURN_IF_STRING_TOO_LONG(); *string_buf_ptr++ = '\t'; }
\\b { RETURN_IF_STRING_TOO_LONG(); *string_buf_ptr++ = '\b'; }
\\f { RETURN_IF_STRING_TOO_LONG(); *string_buf_ptr++ = '\f'; }
\\(.|\n) {
  RETURN_IF_STRING_TOO_LONG();
  *string_buf_ptr++ = yytext[1];
}
[^\\\n\"]+ {  /* any proper string */
  for (int i = 0; i < yyleng; ++i) {
    RETURN_IF_STRING_TOO_LONG();
    *string_buf_ptr++ = yytext[i];
  }
}
}

<COMMENT,STRING><<EOF>> {
  yylval.error_msg = "EOF in comment";
  BEGIN(INITIAL);
  return (ERROR);
}

 /*
  * Keywords are case-insensitive except for the values true and false,
  * which must begin with a lower-case letter.
  * CLASS = 258,
  * ELSE = 259,
  * FI = 260,
  * IF = 261,
  * IN = 262,
  * INHERITS = 263,
  * LET = 264,
  * LOOP = 265,
  * POOL = 266,
  * THEN = 267,
  * WHILE = 268,
  * CASE = 269,
  * ESAC = 270,
  * OF = 271,
  * DARROW = 272,
  * NEW = 273,
  * ISVOID = 274,
  * STR_CONST = 275,
  * INT_CONST = 276,
  * BOOL_CONST = 277,
  * TYPEID = 278,
  * OBJECTID = 279,
  * ASSIGN = 280,
  * NOT = 281,
  * LE = 282,
  * ERROR = 283,
  */
(?i:class)      return (CLASS);
(?i:else)       return (ELSE);
(?i:fi)         return (FI);
(?i:if)         return (IF);
(?i:in)         return (IN);
(?i:inherits)   return (INHERITS);
(?i:let)        return (LET);
(?i:loop)       return (LOOP);
(?i:pool)       return (POOL);
(?i:then)       return (THEN);
(?i:while)      return (WHILE);
(?i:case)       return (CASE);
(?i:esac)       return (ESAC);
(?i:of)         return (OF);
(?i:new)        return (NEW);
"<-"            return (ASSIGN);
(?i:isvoid)     return (ISVOID);
(?i:not)        return (NOT);


{DIGIT}+ {
  cool_yylval.symbol = inttable.add_string(yytext);
  return (INT_CONST);
}

t(?i:rue) { cool_yylval.boolean = 1; return (BOOL_CONST); }
f(?i:alse) { cool_yylval.boolean = 0; return (BOOL_CONST); }

{TYPEID} {
  cool_yylval.symbol = idtable.add_string(yytext);
  return (TYPEID);
}

{OBJECTID} /* Handle brackets?: "[]"* */ {
  cool_yylval.symbol = idtable.add_string(yytext);
  return (OBJECTID);
}

 /*
  *  The operators && other stuff.
  */
{DARROW}    { return (DARROW); }
"<="        { return (LE); }
"+"         { return int('+'); }
"/"         { return int('/'); }
"-"         { return int('-'); }
"*"         { return int('*'); }
"="         { return int('='); }
"<"         { return int('<'); }
"."         { return int('.'); }
"~"         { return int('~'); }
","         { return int(','); }
";"         { return int(';'); }
":"         { return int(':'); }
"("         { return int('('); }
")"         { return int(')'); }
"@"         { return int('@'); }
"{"         { return int('{'); }
"}"         { return int('}'); }

[ \f\r\t\v] {  }  /* ignore whitespace characters */

<INITIAL,COMMENT>"\n" { curr_lineno++; }

 /* mark anything else as an error */
[^\n] {
  yylval.error_msg = yytext;
  return (ERROR);
}

%%

size_t buf_size() { return string_buf_ptr - (&string_buf[0]); }
