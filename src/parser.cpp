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
#define mp make_pair
using std ::pair;
using std ::string;
using std ::vector;

extern std ::map<std ::string, ExprType> primitives;
extern std ::map<std ::string, ExprType> reserved_words;

Expr Syntax ::parse(Assoc &env) {}

Expr Number ::parse(Assoc &env) { return Expr(new Fixnum(n)); }

Expr Identifier ::parse(Assoc &env) { return Expr(new Var(s)); }

Expr TrueSyntax ::parse(Assoc &env) { return Expr(new True()); }

Expr FalseSyntax ::parse(Assoc &env) { return Expr(new False()); }

Expr List ::parse(Assoc &env) {
    if (stxs.empty()) {
        throw RuntimeError("错啦");
    }
    auto op = dynamic_cast<Identifier *>(stxs[0].get())->s;
    if (primitives.find(op) != primitives.end()) {
        switch (primitives[op]) {
            case E_MINUS:
                if (stxs.size() != 3) {
                    throw RuntimeError("错啦");
                } else {
                    return Expr(
                        new Minus(stxs[1]->parse(env), stxs[2]->parse(env)));
                }
                break;
            case E_PLUS:
                if (stxs.size() != 3) {
                    throw RuntimeError("错啦");
                } else {
                    return Expr(
                        new Plus(stxs[1]->parse(env), stxs[2]->parse(env)));
                }
                break;
            case E_MUL:
                if (stxs.size() != 3) {
                    throw RuntimeError("错啦");
                } else {
                    return Expr(
                        new Mult(stxs[1]->parse(env), stxs[2]->parse(env)));
                }
                break;
            case E_GT:
                if (stxs.size() != 3) {
                    throw RuntimeError("错啦");
                } else {
                    return Expr(
                        new Greater(stxs[1]->parse(env), stxs[2]->parse(env)));
                }
                break;
            case E_GE:
                if (stxs.size() != 3) {
                    throw RuntimeError("错啦");
                } else {
                    return Expr(new GreaterEq(stxs[1]->parse(env),
                                              stxs[2]->parse(env)));
                }
                break;
            case E_LT:
                if (stxs.size() != 3) {
                    throw RuntimeError("错啦");
                } else {
                    return Expr(
                        new Less(stxs[1]->parse(env), stxs[2]->parse(env)));
                }
                break;
            case E_LE:
                if (stxs.size() != 3) {
                    throw RuntimeError("错啦");
                } else {
                    return Expr(
                        new LessEq(stxs[1]->parse(env), stxs[2]->parse(env)));
                }
                break;
            case E_EQ:
                if (stxs.size() != 3) {
                    throw RuntimeError("错啦");
                } else {
                    return Expr(
                        new Equal(stxs[1]->parse(env), stxs[2]->parse(env)));
                }
                break;
            case E_VOID:
                if (stxs.size() != 1) {
                    throw RuntimeError("错啦");
                } else {
                    return Expr(new MakeVoid());
                }
                break;
            case E_EXIT:
                if (stxs.size() != 1) {
                    throw RuntimeError("错啦");
                } else {
                    return Expr(new Exit());
                }
                break;
            case E_CONS:
                if (stxs.size() != 3) {
                    throw RuntimeError("错啦");
                } else {
                    return Expr(
                        new Cons(stxs[1]->parse(env), stxs[2]->parse(env)));
                }
                break;
            case E_CAR:
                if (stxs.size() != 2) {
                    throw RuntimeError("错啦");
                } else {
                    return Expr(new Car(stxs[1]->parse(env)));
                }
                break;
            case E_CDR:
                if (stxs.size() != 2) {
                    throw RuntimeError("错啦");
                } else {
                    return Expr(new Cdr(stxs[1]->parse(env)));
                }
                break;
            case E_NOT:
                if(stxs.size()!=2){
                    throw RuntimeError("错啦");
                }else{
                    return Expr(new Not(stxs[1]->parse(env)));

                }
            default:
                break;
        }
    } else if (reserved_words.find(op) != reserved_words.end()) {
        switch (reserved_words[op]) {
            case E_QUOTE:
                if (stxs.size() != 2) {
                    throw RuntimeError("错啦");
                } else {
                    return Expr(new Quote(stxs[1]));
                }
                break;
            case E_BEGIN:
                if (stxs.size() < 2) {
                    throw RuntimeError("戳啦");
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
                if (stxs.size() != 4) {
                    throw RuntimeError("戳啦");
                } else{
                    return (new If(stxs[1]->parse(env),stxs[2]->parse(env),stxs[3]->parse(env)));
                }
            default:
                break;
        }
    }
}
#endif