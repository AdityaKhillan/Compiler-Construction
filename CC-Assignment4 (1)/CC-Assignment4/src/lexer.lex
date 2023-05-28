%option noyywrap
%x STATE1
%x STATE2
%x STATE3
%x STATE4
%x IFDEF
%x ELIF
%x NDEF

%{
    #include "parser.hh"
    #include <string>
    #include <unordered_map>
    #include <cstring>
    #include <tuple>
    std::unordered_map<std::string, std::string> mp;
    extern int yyerror(std::string msg);
    std::string key;
    int def = 0;
    int count = 0;
%}



%%
"/*"                 BEGIN(STATE4);
<STATE4>[^*]*        /* eat anything that's not a '*' */
<STATE4>"*"+[^*/]*   /* eat up '*'s not followed by '/'s */
<STATE4>"*"+"/"      {BEGIN(INITIAL);}

"#def " { BEGIN STATE1;}                                                            
<STATE1>[a-zA-Z]+ {key = yytext; mp[key] = "";}
<STATE1>[ ]+ {BEGIN STATE2;}
<STATE1>"\n" {BEGIN INITIAL; mp[key]="1";}
<STATE2>[;0-9+-/*=() ]+ {mp[key].append(yytext);}
<STATE2>[A-Za-z]+ {mp[key].append(yytext);}
<STATE2>"\n" {
    BEGIN INITIAL;
    if(mp[key] == "") {
        mp[key] = "1";
    }
}

"#undef " {BEGIN STATE3;}
<STATE3>[a-zA-Z]+ {if(mp.find(yytext)!=mp.end()) mp.erase(yytext);}
<STATE3>"\n" {
    BEGIN INITIAL;
}



"#ifdef " {BEGIN IFDEF;}
<IFDEF>[a-zA-Z]+ {
    if(mp.find(yytext) != mp.end()) {
        BEGIN INITIAL;
        def=1;
    }
    else {
        BEGIN NDEF;

    }
}
<NDEF>"#endif" {
    def=0;
    BEGIN INITIAL;
}
<NDEF>"#elif " {
    if(def==0){
        BEGIN ELIF;
    }
}
<ELIF>[a-zA-Z]+ {
    std::cout<<yytext<<std::endl;
    if(mp.find(yytext) != mp.end()) {
        BEGIN INITIAL;
        def=1;
    }
    else {
        BEGIN NDEF;

    }
}
<NDEF>"#else" {

    if(def == 0){
        BEGIN INITIAL;
    }else if(count>=1){
        yyerror("#else more than 1 times\n");
    }else{
        BEGIN NDEF;
    }
    count = 1;

}
<NDEF><<EOF>> {
    yyerror("close the #ifdef with #endif");
}

"//".*    {/*skip*/}                      
"+"       { return TPLUS; }
"-"       { return TDASH; }
"*"       { return TSTAR; }
"/"       { return TSLASH; }
";"       { return TSCOL; }
"("       { return TLPAREN; }
")"       { return TRPAREN; }
"="       { return TEQUAL; }
":"       { return TCOLON; }
"?"       { return TQUESTMARK; }
"dbg"     { return TDBG; }
"let"     { return TLET; }
"int"     { yylval.lexeme = std::string(yytext); return DTYPE;}//INT;}
"short"     { yylval.lexeme = std::string(yytext); return DTYPE;}//INT;}
"long"     { yylval.lexeme = std::string(yytext); return DTYPE;}//INT;}
"if"        {return TIF;}
"else"      {return TELSE;}
"{"         {return TLBRACE;}
"}"         {return TRBRACE;}
"fun"       {return TFUN;}
","             {return TCOMMA;}
[0-9]+    { yylval.lexeme = std::string(yytext); return TINT_LIT; }
[a-zA-Z]+ {
    if(mp.find(yytext) != mp.end()) {
        std::string val = mp[yytext];
        for(int i=val.size()-1;i>=0;i--) {
            unput(val[i]);                              
        }
    }
    else {
        yylval.lexeme = std::string(yytext); return TIDENT;
    }
}
[ \t\n]   { /* skip */ }
.         { yyerror("unknown char"); }

"#elif " {
    if(def != 1){
        yyerror("No #ifdef present, can't start with #elif");
    }
    else {
        BEGIN NDEF;
    }
}
"#else" {

    if(def != 1){
        yyerror("No #ifdef present, can't start with #else");

    }
    else if(count >= 1) {
        yyerror("more than one #else are not allowed");
    }
    else {
        BEGIN NDEF;
    }
    count = 1;
}
"#endif" {
    def=0;
}
<<EOF>> {
    if(def==1){
        yyerror("No #endif present\n");
    }
    yyterminate();
}

%%


std::string token_to_string(int token, const char *lexeme) {
    std::string s;
    switch (token) {
        case TPLUS: s = "TPLUS"; break;
        case TDASH: s = "TDASH"; break;
        case TSTAR: s = "TSTAR"; break;
        case TSLASH: s = "TSLASH"; break;
        case TSCOL: s = "TSCOL"; break;
        case TLPAREN: s = "TLPAREN"; break;
        case TRPAREN: s = "TRPAREN"; break;
        case TEQUAL: s = "TEQUAL"; break;
       
        case TDBG: s = "TDBG"; break;
        case TLET: s = "TLET"; break;
       
        case TINT_LIT: s = "TINT_LIT"; s.append("  ").append(lexeme); break;
        case TIDENT: s = "TIDENT"; s.append("  ").append(lexeme); break;
    }

    return s;
}