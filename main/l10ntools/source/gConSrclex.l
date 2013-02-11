/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/


/*****************************************************************************
 **********************   L E X   D E F I N I T I O N   **********************
 *****************************************************************************
 * lex grammar for parsing ressource source files (*.src)
 * file is converted to gConSrc_yy.cxx with "flex"
 *****************************************************************************/



/***************   O V E R W R I T I N G   F U N C T I O N S   ***************/
%{
/* enlarge token buffer to tokenize whole std::strings */
#undef  YYLMAX
#define YYLMAX 64000

/* change reader function (input) to our own version */
#define YY_INPUT(buf,result,max_size) { IMPLptr->lexRead(buf, &result, max_size); }
%}



/*****************************   O P T I O N S   *****************************/
/* 8bit               --> allow 8bit characters in the input stream   */
/* noyywrap           --> yywrap is not called (single file scan)     */
/* never-interactive  --> no check for console output                 */
/* prefix=            --> yyFlexLexer change name                     */
/* --- The following options are for future use (maybe)               */
/* yyclass=           --> subClass yyFlexLexer to allow own functions */
/* c++                --> generate C++ classes                        */
%option 8bit noyywrap never-interactive prefix="genSrc_"
%p 24000
%e 1200
%n 500



/*******************   R U L E S   D E F I N I T I O N S   *******************/
%%

[bB][iI][tT][mM][aA][pP]\ +[a-zA-Z]+[^\{]*                                  |
[bB][uU][tT][tT][oO][nN][iI][mM][aA][gG][eE]\ +[a-zA-Z]+[^\{]*              |
[cC][aA][nN][cC][eE][lL][bB][uU][tT][tT][oO][nN]\ +[a-zA-Z]+[^\{]*          |
[cC][hH][eE][cC][kK][bB][oO][xX]\ +[a-zA-Z]+[^\{]*                          |
[cC][oO][nN][tT][rR][oO][lL]\ +[a-zA-Z]+[^\{]*                              |
[cC][oO][mM][bB][oO][bB][oO][xX]\ +[a-zA-Z]+[^\{]*                          |
[eE][dD][iI][tT]\ +[a-zA-Z]+[^\{]*                                          |
[eE][rR][rR][oO][rR][bB][oO][xX]\ +[a-zA-Z]+[^\{]*                          |
[fF][iI][xX][eE][dD][tT][eE][xX][tT]\ +[a-zA-Z]+[^\{]*                      |
[fF][iI][xX][eE][dD][lL][iI][nN][eE]\ +[a-zA-Z]+[^\{]*                      |
[fF][lL][oO][aA][tT][iI][nN][gG][wW][iI][nN][dD][oO][wW]\ +[a-zA-Z]+[^\{]*  |
[hH][eE][lL][pP][bB][uU][tT][tT][oO][nN]\ +[a-zA-Z]+[^\{]*                  |
[iI][dD][lL][iI][sS][tT]\ +[a-zA-Z]+[^\{]*                                  |
[iI][mM][aA][gG][eE]\ +[a-zA-Z]+[^\{]*                                      |
[iI][mM][aA][gG][eE][lL][iI][sS][tT]\ +[a-zA-Z]+[^\{]*                      |
[iI][mM][aA][gG][eE][bB][uU][tT][tT][oO][nN]\ +[a-zA-Z]+[^\{]*              |
[iI][tT][eE][mM][lL][iI][sS][tT]\ +[a-zA-Z]+[^\{]*                          |
[lL][iI][sS][tT][bB][oO][xX]\ +[a-zA-Z]+[^\{]*                              |
[mM][eE][nN][uU]\ +[a-zA-Z]+[^\{]*                                          |
[mM][eE][nN][uU][iI][tT][eE][mM]\ +[a-zA-Z]+[^\{]*                          |
[mM][eE][tT][rR][iI][cC][fF][iI][eE][lL][dD]\ +[a-zA-Z]+[^\{]*              |
[mM][oO][dD][aA][lL][dD][iI][aA][lL][oO][gG]\ +[a-zA-Z]+[^\{]*              |
[mM][uU][lL][tT][iI][lL][iI][nN][eE][eE][dD][iI][tT]\ +[a-zA-Z]+[^\{]*      |
[nN][uU][mM][eE][rR][iI][cC][fF][iI][eE][lL][dD]\ +[a-zA-Z]+[^\{]*          |
[oO][kK][bB][uU][tT][tT][oO][nN]\ +[a-zA-Z]+[^\{]*                          |
[pP][aA][gG][eE][iI][tT][eE][mM]\ +[a-zA-Z]+[^\{]*                          |
[pP][aA][gG][eE][lL][iI][sS][tT]\ +[a-zA-Z]+[^\{]*                          |
[pP][uU][sS][hH][bB][uU][tT][tT][oO][nN]\ +[a-zA-Z]+[^\{]*                  |
[qQ][uU][eE][rR][yY][bB][oO][xX]\ +[a-zA-Z]+[^\{]*                          |
[rR][aA][dD][iI][oO][bB][uU][tT][tT][oO][nN]\ +[a-zA-Z]+[^\{]*              |
[rR][eE][sS][oO][uU][rR][cC][eE]\ +[a-zA-Z]+[^\{]*                          |
[sS][tT][rR][iI][nN][gG]\ +[a-zA-Z]+[^\{]*                                  |
[sS][tT][rR][iI][nN][gG][aA][rR][rR][aA][yY]\ +[a-zA-Z]+[^\{]*              |
[tT][aA][bB][cC][oO][nN][tT][rR][oO][lL]\ +[a-zA-Z]+[^\{]*                  |
[tT][aA][bB][dD][iI][aA][lL][oO][gG]\ +[a-zA-Z]+[^\{]*                      |
[tT][aA][bB][pP][aA][gG][eE]\ +[a-zA-Z]+[^\{]*                              |
[tT][oO][oO][lL][bB][oO][xX]\ +[a-zA-Z]+[^\{]*                              |
[tT][oO][oO][lL][bB][oO][xX][iI][tT][eE][mM]\ +[a-zA-Z]+[^\{]*              {
  LOCptr->pushKey(yytext);
}



[bB][iI][tT][mM][aA][pP][ \t\n]+"{"                                  |
[bB][uU][tT][tT][oO][nN][iI][mM][aA][gG][eE][ \t\n]+"{"              |
[cC][aA][nN][cC][eE][lL][bB][uU][tT][tT][oO][nN][ \t\n]"{"           |
[cC][hH][eE][cC][kK][bB][oO][xX][ \t\n]+"{"                          |
[cC][oO][nN][tT][rR][oO][lL][ \t\n]+"{"                              |
[cC][oO][mM][bB][oO][bB][oO][xX][ \t\n]+"{"                          |
[eE][dD][iI][tT][ \t\n]+"{"                                          |
[eE][rR][rR][oO][rR][bB][oO][xX][ \t\n]+"{"                          |
[fF][iI][xX][eE][dD][tT][eE][xX][tT][ \t\n]+"{"                      |
[fF][iI][xX][eE][dD][lL][iI][nN][eE][ \t\n]+"{"                      |
[fF][lL][oO][aA][tT][iI][nN][gG][wW][iI][nN][dD][oO][wW][ \t\n]+"{"  |
[hH][eE][lL][pP][bB][uU][tT][tT][oO][nN][ \t\n]+"{"                  |
[iI][dD][lL][iI][sS][tT][ \t\n=]+"{"                                 |
[iI][mM][aA][gG][eE][ \t\n]+"{"                                      |
[iI][mM][aA][gG][eE][bB][uU][tT][tT][oO][nN][ \t\n]+"{"              |
[iI][mM][aA][gG][eE][lL][iI][sS][tT][ \t\n]+"{"                      |
[iI][tT][eE][mM][lL][iI][sS][tT][ \t\n=]+"{"                         |
[lL][iI][sS][tT][bB][oO][xX][ \t\n]+"{"                              |
[mM][eE][nN][uU][ \t\n]+"{"                                          |
[mM][eE][nN][uU][iI][tT][eE][mM][ \t\n]+"{"                          |
[mM][eE][tT][rR][iI][cC][fF][iI][eE][lL][dD][ \t\n]+"{"              |
[mM][oO][dD][aA][lL][dD][iI][aA][lL][oO][gG][ \t\n]+"{"              |
[mM][uU][lL][tT][iI][lL][iI][nN][eE][eE][dD][iI][tT][ \t\n]+"{"      |
[nN][uU][mM][eE][rR][iI][cC][fF][iI][eE][lL][dD][ \t\n]+"{"          |
[oO][kK][bB][uU][tT][tT][oO][nN][ \t\n]+"{"                          |
[pP][aA][gG][eE][iI][tT][eE][mM][ \t\n]+"{"                          |
[pP][aA][gG][eE][lL][iI][sS][tT][ \t\n]+"{"                          |
[pP][uU][sS][hH][bB][uU][tT][tT][oO][nN][ \t\n]+"{"                  |
[qQ][uU][eE][rR][yY][bB][oO][xX][ \t\n]+"{"                          |
[rR][aA][dD][iI][oO][bB][uU][tT][tT][oO][nN][ \t\n]+"{"              |
[rR][eE][sS][oO][uU][rR][cC][eE][ \t\n]+"{"                          |
[sS][tT][rR][iI][nN][gG][ \t\n]+"{"                                  |
[sS][tT][rR][iI][nN][gG][aA][rR][rR][aA][yY][ \t\n]+"{"              |
[tT][aA][bB][cC][oO][nN][tT][rR][oO][lL][ \t\n]+"{"                  |
[tT][aA][bB][dD][iI][aA][lL][oO][gG][ \t\n]+"{"                      |
[tT][aA][bB][pP][aA][gG][eE][ \t\n]+"{"                              |
[tT][oO][oO][lL][bB][oO][xX][ \t\n]+"{"                              |
[tT][oO][oO][lL][bB][oO][xX][iI][tT][eE][mM][ \t\n]+"{"              {
  LOCptr->pushNoKey(yytext);
}

[hH][eE][lL][pP][iI][dD]\ *"="\ *[a-zA-Z\"]+[^;]*                 |
[iI][dD][eE][nN][tT][iI][fF][iI][eE][rR]\ *"="\ *[a-zA-Z\"]+[^;]* {
  LOCptr->registerKey(yytext);
}

[mM][eE][sS][sS][aA][gG][eE]\ *"["\ *"en-US"\ *"]"\ *"="[^\n]*                         |
[qQ][uU][iI][cC][kK][hH][eE][lL][pP][tT][eE][xX][tT]\ *"["\ *"en-US"\ *"]"\ *"="[^\n]* |
[tT][eE][xX][tT]\ *"["\ *"en-US"\ *"]"\ *"="[^\n]*                                     |
[tT][iI][tT][lL][eE]\ *"["\ *"en-US"\ *"]"\ *"="[^\n]*                                 {
  LOCptr->saveData(yytext);
}

[iI][tT][eE][mM][lL][iI][sS][tT]\ *"["\ *"en-US"\ *"]"\ *"="[ \t\n]+"{"[^\}]*"};"          |
[sS][tT][rR][iI][nN][gG][lL][iI][sS][tT]\ *"["\ *"en-US"\ *"]"\ *"="[ \t\n]+"{"[^\}]*"};"  {
  LOCptr->saveItemList(yytext);
}

"}"[ ]*";" {
  LOCptr->popKey(yytext);
}

.|\n {
  IMPLptr->collectData(yytext);
}

%%