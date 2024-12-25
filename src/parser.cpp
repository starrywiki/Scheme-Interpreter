#ifndef PARSER
#define PARSER

// parser of myscheme

#include <cstring>
#include <iostream>
#include <map>

#include "Def.hpp"
#include "RE.hpp"
#include "expr.hpp"
#include "syntax.hpp"
#include "value.hpp"
//#include "Debug.hpp"

#define mp make_pair
using std ::pair;
using std ::string;
using std ::vector;

extern std ::map<std ::string, ExprType> primitives;
extern std ::map<std ::string, ExprType> reserved_words;

Expr Syntax ::parse(Assoc &env) {
    //DEBUG_PRINT("Entering Syntax::parse");
    if (get() == nullptr) throw RuntimeError("unexpected EOF");
    return get()->parse(env);
}

Expr Number ::parse(Assoc &env) { return Expr(new Fixnum(n)); }

Expr Identifier ::parse(Assoc &env) { 
    //DEBUG_PRINT("Parsing Identifier: " << s);
    return Expr(new Var(s)); }

Expr TrueSyntax ::parse(Assoc &env) { return Expr(new True()); }

Expr FalseSyntax ::parse(Assoc &env) { return Expr(new False()); }

Expr List ::parse(Assoc &env) {
    //DEBUG_PRINT("Parsing List with " << stxs.size() << " elements");
    if (stxs.empty()) {
        throw RuntimeError("WA");
    }
    auto id = dynamic_cast<Identifier *>(stxs[0].get());
    if (id == nullptr) {
        //DEBUG_PRINT("List first element is not an Identifier");
        int len = stxs.size();
        Expr ex = stxs[0]->parse(env);
        std::vector<Expr> exs;
        for (int i = 1; i < len; ++i) {
            exs.push_back(stxs[i]->parse(env));
        }
        //DEBUG_PRINT("Creating Apply expression");
        return Expr(new Apply(ex, exs));
    }
    string op = id->s;
    //DEBUG_PRINT("List operator: " << op);
    if (find(op, env).get() != nullptr) {
        //DEBUG_PRINT("Operator found in environment: " << op);
        std::vector<Expr> exprs;
        for (int i = 1; i < stxs.size(); ++i) {
            exprs.push_back(stxs[i]->parse(env));
        }
        return Expr(new Apply(stxs[0]->parse(env), exprs));
    }
    if (primitives.find(op) != primitives.end()) {
        //DEBUG_PRINT("Operator is a primitive: " << op);
        switch (primitives[op]) {
            case E_MINUS:
                if (stxs.size() != 3) {
                    throw RuntimeError("WA");
                } else {
                    return Expr(
                        new Minus(stxs[1]->parse(env), stxs[2]->parse(env)));
                }
                break;
            case E_PLUS:
                if (stxs.size() != 3) {
                    throw RuntimeError("WA");
                } else {
                    return Expr(
                        new Plus(stxs[1]->parse(env), stxs[2]->parse(env)));
                }
                break;
            case E_MUL:
                if (stxs.size() != 3) {
                    throw RuntimeError("WA");
                } else {
                    return Expr(
                        new Mult(stxs[1]->parse(env), stxs[2]->parse(env)));
                }
                break;
            case E_GT:
                if (stxs.size() != 3) {
                    throw RuntimeError("WA");
                } else {
                    return Expr(
                        new Greater(stxs[1]->parse(env), stxs[2]->parse(env)));
                }
                break;
            case E_GE:
                if (stxs.size() != 3) {
                    throw RuntimeError("WA");
                } else {
                    return Expr(new GreaterEq(stxs[1]->parse(env),
                                              stxs[2]->parse(env)));
                }
                break;
            case E_LT:
                if (stxs.size() != 3) {
                    throw RuntimeError("WA");
                } else {
                    return Expr(
                        new Less(stxs[1]->parse(env), stxs[2]->parse(env)));
                }
                break;
            case E_LE:
                if (stxs.size() != 3) {
                    throw RuntimeError("WA");
                } else {
                    return Expr(
                        new LessEq(stxs[1]->parse(env), stxs[2]->parse(env)));
                }
                break;
            case E_EQ:
                if (stxs.size() != 3) {
                    throw RuntimeError("WA");
                } else {
                    return Expr(
                        new Equal(stxs[1]->parse(env), stxs[2]->parse(env)));
                }
                break;
            case E_VOID:
                if (stxs.size() != 1) {
                    throw RuntimeError("WA");
                } else {
                    return Expr(new MakeVoid());
                }
                break;
            case E_EXIT:
                if (stxs.size() != 1) {
                    throw RuntimeError("WA");
                } else {
                    return Expr(new Exit());
                }
                break;
            case E_CONS:
                if (stxs.size() != 3) {
                    throw RuntimeError("WA");
                } else {
                    return Expr(
                        new Cons(stxs[1]->parse(env), stxs[2]->parse(env)));
                }
                break;
            case E_CAR:
                if (stxs.size() != 2) {
                    throw RuntimeError("WA");
                } else {
                    return Expr(new Car(stxs[1]->parse(env)));
                }
                break;
            case E_CDR:
                if (stxs.size() != 2) {
                    throw RuntimeError("WA");
                } else {
                    return Expr(new Cdr(stxs[1]->parse(env)));
                }
                break;
            case E_NOT:
                if (stxs.size() != 2) {
                    throw RuntimeError("WA");
                } else {
                    return Expr(new Not(stxs[1]->parse(env)));
                }
                break;
            case E_INTQ:
                if (stxs.size() != 2) {
                    throw RuntimeError("WA");
                } else {
                    return Expr(new IsFixnum(stxs[1]->parse(env)));
                }
                break;
            case E_BOOLQ:
                if (stxs.size() != 2) {
                    throw RuntimeError("WA");
                } else {
                    return Expr(new IsBoolean(stxs[1]->parse(env)));
                }
                break;
            case E_NULLQ:
                if (stxs.size() != 2) {
                    throw RuntimeError("WA");
                } else {
                    return Expr(new IsNull(stxs[1]->parse(env)));
                }
                break;
            case E_PAIRQ:
                if (stxs.size() != 2) {
                    throw RuntimeError("WA");
                } else {
                    return Expr(new IsPair(stxs[1]->parse(env)));
                }
                break;
            case E_SYMBOLQ:
                if (stxs.size() != 2) {
                    throw RuntimeError("WA");
                } else {
                    return Expr(new IsSymbol(stxs[1]->parse(env)));
                }
                break;
            case E_EQQ:
                if (stxs.size() != 3) {
                    throw RuntimeError("WA");
                } else {
                    return Expr(
                        new IsEq(stxs[1]->parse(env), stxs[2]->parse(env)));
                }
                break;
            case E_PROCQ:  //?
                if (stxs.size() != 2) {
                    throw RuntimeError("WA");
                } else {
                    return Expr(new IsProcedure(stxs[1]->parse(env)));
                }
                break;
            default:
                break;
        }
    }
    if (reserved_words.find(op) != reserved_words.end()) {
        //DEBUG_PRINT("Operator is a reserved word: " << op);
        switch (reserved_words[op]) {
            case E_QUOTE:
                if (stxs.size() != 2) {
                    throw RuntimeError("WA");
                } else {
                    return Expr(new Quote(stxs[1]));
                }
                break;
            case E_BEGIN:
                if (stxs.size() < 2) {
                    throw RuntimeError("WA");
                } else {
                    int len = stxs.size();
                    vector<Expr> exprs;
                    for (int i = 1; i < len; ++i) {
                        exprs.push_back(stxs[i]->parse(env));
                    }
                    return Expr(new Begin(exprs));
                }
                break;
            case E_IF:
                //DEBUG_PRINT("Handling E_IF");
                if (stxs.size() != 4) {
                    throw RuntimeError("WA");
                } else {
                    return Expr(new If(stxs[1]->parse(env), stxs[2]->parse(env),
                                       stxs[3]->parse(env)));
                }
                break;
            case E_LET:

                break;
            case E_LETREC:
                break;
            case E_LAMBDA:  // not finished？
                //DEBUG_PRINT("Handling E_LAMBDA");
                if (stxs.size() != 3) {
                    throw RuntimeError("WA");
                } else {
                    std::vector<std::string> vars;
                    auto varstxs = dynamic_cast<List *>(stxs[1].get())->stxs;
                    int len = varstxs.size();
                    Assoc new_env = env;
                    for (int i = 0; i < len; ++i) {
                        string s =
                            dynamic_cast<Identifier *>(varstxs[i].get())->s;
                        vars.push_back(s);
                        new_env = extend(s, VoidV(), new_env);
                    }
                    return Expr(new Lambda(vars, stxs[2]->parse(new_env)));
                }
                break;

            default:
                throw RuntimeError("WA");
                break;
        }
    }
    /*int len = stxs.size();
    Expr ex = stxs[0]->parse(env);
    std::vector<Expr> exs;
    for (int i = 1; i < len; ++i) {
        exs.push_back(stxs[i]->parse(env));
    }
    return Expr(new Apply(ex, exs));*/
}
#endif