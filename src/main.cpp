#include "Def.hpp"
#include "syntax.hpp"
#include "expr.hpp"
#include "value.hpp"
#include "RE.hpp"
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <string>

extern std :: map<std :: string, ExprType> primitives;
extern std :: map<std :: string, ExprType> reserved_words;

void REPL()
{
    // read - evaluation - print loop
    Assoc global_env = empty();
    std::vector<std::string> var2;
    Expr pr = nullptr;
    var2.push_back("X");  
    var2.push_back("Y");  
    global_env = extend("*",ClosureV(var2,Expr(new Mult(new Var("X"),new Var("Y"))),global_env),global_env);
    global_env = extend("-",ClosureV(var2,Expr(new Minus(new Var("X"),new Var("Y"))),global_env),global_env);
    global_env = extend("+",ClosureV(var2,Expr(new Plus(new Var("X"),new Var("Y"))),global_env),global_env);
    global_env = extend("<",ClosureV(var2,Expr(new Less(new Var("X"),new Var("Y"))),global_env),global_env);
    global_env = extend("<=",ClosureV(var2,Expr(new LessEq(new Var("X"),new Var("Y"))),global_env),global_env);
    global_env = extend("=",ClosureV(var2,Expr(new Equal(new Var("X"),new Var("Y"))),global_env),global_env);
    global_env = extend(">",ClosureV(var2,Expr(new Greater(new Var("X"),new Var("Y"))),global_env),global_env);
    global_env = extend(">=",ClosureV(var2,Expr(new GreaterEq(new Var("X"),new Var("Y"))),global_env),global_env);
    global_env = extend("eq?",ClosureV(var2,Expr(new IsEq(new Var("X"),new Var("Y"))),global_env),global_env);
    global_env = extend("cons",ClosureV(var2,Expr(new Cons(new Var("X"),new Var("Y"))),global_env),global_env);
    std::vector<std::string> var1;
    var1.push_back("X");
    global_env = extend("boolean?",ClosureV(var1,Expr(new IsBoolean(new Var("X"))),global_env),global_env);
    global_env = extend("fixnum?",ClosureV(var1,Expr(new IsFixnum(new Var("X"))),global_env),global_env);
    global_env = extend("symbol?",ClosureV(var1,Expr(new IsSymbol(new Var("X"))),global_env),global_env);
    global_env = extend("null?",ClosureV(var1,Expr(new IsNull(new Var("X"))),global_env),global_env);
    global_env = extend("pair?",ClosureV(var1,Expr(new IsPair(new Var("X"))),global_env),global_env);
    global_env = extend("procedure?",ClosureV(var1,Expr(new IsProcedure(new Var("X"))),global_env),global_env);
    global_env = extend("not",ClosureV(var1,Expr(new Not(new Var("X"))),global_env),global_env);
    global_env = extend("car",ClosureV(var1,Expr(new Car(new Var("X"))),global_env),global_env);
    global_env = extend("cdr",ClosureV(var1,Expr(new Cdr(new Var("X"))),global_env),global_env);
    std::vector<std::string> var0;
    global_env = extend("exit",ClosureV(var0,Expr(new Exit()),global_env),global_env);
    global_env = extend("void",ClosureV(var0,Expr(new MakeVoid()),global_env),global_env);

    
    while (1)
    {
        #ifndef ONLINE_JUDGE
            std::cout << "scm> ";
        #endif
        Syntax stx = readSyntax(std :: cin); // read
        try
        {
            Expr expr = stx -> parse(global_env); // parse
            // stx -> show(std :: cout); // syntax print
            Value val = expr -> eval(global_env);
            if (val -> v_type == V_TERMINATE)
                break;
            val -> show(std :: cout); // value print
        }
        catch (const RuntimeError &RE)
        {
            // std :: cout << RE.message();
            std :: cout << "RuntimeError";
        }
        puts("");
    }
}

int main(int argc, char *argv[]) {
    initPrimitives();
    initReservedWords();
    REPL();
    return 0;
}
