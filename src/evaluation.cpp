#include <cstring>
#include <map>
#include <vector>

#include "Def.hpp"
#include "RE.hpp"
#include "expr.hpp"
#include "syntax.hpp"
#include "value.hpp"

extern std ::map<std ::string, ExprType> primitives;
extern std ::map<std ::string, ExprType> reserved_words;

Value Let::eval(Assoc &env) {}  // let expression

Value Lambda::eval(Assoc &env) {
    return ClosureV(x,e,env);
}  // lambda expression

Value Apply::eval(Assoc &e) {
    Value rval = rator->eval(e);
    Closure* clos = dynamic_cast<Closure*>(rval.get());
    if(clos){
        int len1 = clos->parameters.size();
        int len2 = rand.size();
        if(len1!=len2) throw RuntimeError("戳啦");
        Assoc new_env = clos->env;
        for(int i=0;i<len2;++i){
            auto tmpval = rand[i]->eval(new_env);
            new_env = extend(clos->parameters[i],tmpval,new_env);
        }
        return clos->e.get()->eval(new_env);
    }else{
        throw RuntimeError("戳啦");
    }
}  // for function calling

Value Letrec::eval(Assoc &env) {}  // letrec expression

Value Var::eval(Assoc &e) {
    if(std::isdigit(x[0])||x[0]=='.'||x[0]=='@'){
        throw RuntimeError("戳啦");
    }
    Value isexist = find(x,e);
    if(isexist.get()==nullptr){
        throw RuntimeError("戳啦");
    } else{
        return isexist;
    }
}  // evaluation of variable

Value Fixnum::eval(Assoc &e) { return IntegerV(n); }  // evaluation of a fixnum

Value If::eval(Assoc &e) {  
    Value firstv = cond->eval(e);
    Value secondv = conseq->eval(e);
    Value thirdv = alter->eval(e);
    // auto is2sym = dynamic_cast<Var *>(secondv.get());
    // auto is3sym = dynamic_cast<Var *>(thirdv.get());
    auto it = dynamic_cast<Boolean *>(firstv.get());
    if (it && it->b == false) {
        return thirdv;
    } else {
        return secondv;
    }
}  // if expression

Value True::eval(Assoc &e) { return BooleanV(true); }  // evaluation of #t

Value False::eval(Assoc &e) { return BooleanV(false); }  // evaluation of #f

Value Begin::eval(Assoc &e) {
    int len = es.size();
    if (len == 0) {
        throw RuntimeError("戳啦");
    }
    for (int i = 0; i < len - 1; ++i) {
        auto it = es[i].get()->eval(e);
    }
    return es[len - 1].get()->eval(e);
}  // begin expression

Value Quote::eval(Assoc &e) {
    auto istrue = dynamic_cast<TrueSyntax *>(s.get());
    auto isfalse = dynamic_cast<FalseSyntax *>(s.get());
    auto isnum = dynamic_cast<Number *>(s.get());
    auto issymbol = dynamic_cast<Identifier *>(s.get());
    auto islists = dynamic_cast<List *>(s.get());
    if (istrue) {
        return BooleanV(true);
    } else if (isfalse) {
        return BooleanV(false);
    } else if (isnum) {
        return IntegerV(isnum->n);
    } else if (issymbol) {
        return SymbolV(issymbol->s);
    } else if (islists) {
        int len = islists->stxs.size();
        if (len == 0) {
            return NullV();
        } else if (len == 3) {
            auto dot = dynamic_cast<Identifier *>(islists->stxs[1].get());
            if (dot && dot->s == ".") {
                return PairV(
                    (Expr(new Quote(islists->stxs[0]))).get()->eval(e),
                    (Expr(new Quote(islists->stxs[2]))).get()->eval(e));
            }
        }
        bool isspecial = false;
        Value res = NullV();
        if (len >= 3) {
            auto dotp =
                dynamic_cast<Identifier *>(islists->stxs[len - 2].get());
            if (dotp && dotp->s == ".") {
                res = (Expr(new Quote(islists->stxs[len - 1]))).get()->eval(e);
                isspecial = true;
            }
        }
        if (isspecial) {
            for (int i = len - 3; i >= 0; --i) {
                res = PairV((Expr(new Quote(islists->stxs[i]))).get()->eval(e),
                            res);
            }
            return res;
        } else {
            for (int i = len - 1; i >= 0; --i) {
                res = PairV((Expr(new Quote(islists->stxs[i]))).get()->eval(e),
                            res);
            }
            return res;
        }
    } else {
        return NullV();
    }
}  // quote expression

Value MakeVoid::eval(Assoc &e) { return VoidV(); }  // (void)

Value Exit::eval(Assoc &e) { return TerminateV(); }  // (exit)

Value Binary::eval(Assoc &e) {
    return evalRator(rand1.get()->eval(e), rand2.get()->eval(e));
}  // evaluation of two-operators primitive

Value Unary::eval(Assoc &e) {
    return evalRator(rand.get()->eval(e));
}  // evaluation of single-operator primitive

Value Mult::evalRator(const Value &rand1, const Value &rand2) {
    if (rand1->v_type != V_INT || rand2->v_type != V_INT)
        throw RuntimeError("戳啦");
    return IntegerV((dynamic_cast<Integer *>(rand1.get())->n) *
                    dynamic_cast<Integer *>(rand2.get())->n);
}  // *

Value Plus::evalRator(const Value &rand1, const Value &rand2) {
    if (rand1->v_type != V_INT || rand2->v_type != V_INT)
        throw RuntimeError("戳啦");
    return IntegerV((dynamic_cast<Integer *>(rand1.get())->n) +
                    dynamic_cast<Integer *>(rand2.get())->n);
}  // +

Value Minus::evalRator(const Value &rand1, const Value &rand2) {
    if (rand1->v_type != V_INT || rand2->v_type != V_INT)
        throw RuntimeError("戳啦");
    return IntegerV((dynamic_cast<Integer *>(rand1.get())->n) -
                    dynamic_cast<Integer *>(rand2.get())->n);
}  // -

Value Less::evalRator(const Value &rand1, const Value &rand2) {
    if (rand1->v_type != V_INT || rand2->v_type != V_INT)
        throw RuntimeError("戳啦");
    int num1 = dynamic_cast<Integer *>(rand1.get())->n;
    int num2 = dynamic_cast<Integer *>(rand2.get())->n;
    if (num1 < num2) return BooleanV(true);
    return BooleanV(false);
}  // <

Value LessEq::evalRator(const Value &rand1, const Value &rand2) {
    if (rand1->v_type != V_INT || rand2->v_type != V_INT)
        throw RuntimeError("戳啦");
    int num1 = dynamic_cast<Integer *>(rand1.get())->n;
    int num2 = dynamic_cast<Integer *>(rand2.get())->n;
    if (num1 <= num2) return BooleanV(true);
    return BooleanV(false);
}  // <=

Value Equal::evalRator(const Value &rand1, const Value &rand2) {
    if (rand1->v_type != V_INT || rand2->v_type != V_INT)
        throw RuntimeError("戳啦");
    int num1 = dynamic_cast<Integer *>(rand1.get())->n;
    int num2 = dynamic_cast<Integer *>(rand2.get())->n;
    if (num1 == num2) return BooleanV(true);
    return BooleanV(false);
}  // =

Value GreaterEq::evalRator(const Value &rand1, const Value &rand2) {
    if (rand1->v_type != V_INT || rand2->v_type != V_INT)
        throw RuntimeError("戳啦");
    int num1 = dynamic_cast<Integer *>(rand1.get())->n;
    int num2 = dynamic_cast<Integer *>(rand2.get())->n;
    if (num1 >= num2) return BooleanV(true);
    return BooleanV(false);
}  // >=

Value Greater::evalRator(const Value &rand1, const Value &rand2) {
    if (rand1->v_type != V_INT || rand2->v_type != V_INT)
        throw RuntimeError("戳啦");
    int num1 = dynamic_cast<Integer *>(rand1.get())->n;
    int num2 = dynamic_cast<Integer *>(rand2.get())->n;
    if (num1 > num2) return BooleanV(true);
    return BooleanV(false);
}  // >

Value IsEq::evalRator(const Value &rand1, const Value &rand2) {
    if (rand1.get() == rand2.get()) {
        return BooleanV(true);
    } else if (rand1->v_type == V_INT && rand2->v_type == V_INT) {
        int num1 = dynamic_cast<Integer *>(rand1.get())->n;
        int num2 = dynamic_cast<Integer *>(rand2.get())->n;
        if (num1 == num2) {
            return BooleanV(true);
        } else {
            return BooleanV(false);
        }
    } else if (rand1->v_type == V_BOOL && rand2->v_type == V_BOOL) {
        bool bool1 = dynamic_cast<Boolean *>(rand1.get())->b;
        bool bool2 = dynamic_cast<Boolean *>(rand2.get())->b;
        if (bool1 == bool2) {
            return BooleanV(true);
        } else {
            return BooleanV(false);
        }
    } else if (rand1->v_type == V_SYM && rand2->v_type == V_SYM) {
        std::string sym1 = dynamic_cast<Symbol *>(rand1.get())->s;
        std::string sym2 = dynamic_cast<Symbol *>(rand2.get())->s;
        if (sym1 == sym2) {
            return BooleanV(true);
        } else {
            return BooleanV(false);
        }
    } else if (rand1->v_type == V_VOID && rand2->v_type == V_VOID) {
        return BooleanV(true);
    } else if (rand1->v_type == V_NULL && rand2->v_type == V_NULL) {
        return BooleanV(true);
    } else {
        return BooleanV(false);
    }
}  // eq?

Value Cons::evalRator(const Value &rand1, const Value &rand2) {
    return PairV(rand1, rand2);
}  // cons

Value IsBoolean::evalRator(const Value &rand) {
    if (rand->v_type == V_BOOL) {
        return BooleanV(true);
    } else {
        return BooleanV(false);
    }
}  // boolean?

Value IsFixnum::evalRator(const Value &rand) {
    if (rand->v_type == V_INT) {
        return BooleanV(true);
    } else {
        return BooleanV(false);
    }
}  // fixnum?

Value IsSymbol::evalRator(const Value &rand) {
    if (rand->v_type == V_SYM) {
        return BooleanV(true);
    } else {
        return BooleanV(false);
    }
}  // symbol?

Value IsNull::evalRator(const Value &rand) {
    if (rand->v_type == V_NULL) {
        return BooleanV(true);
    } else {
        return BooleanV(false);
    }
}  // null?

Value IsPair::evalRator(const Value &rand) {
    if (rand->v_type == V_PAIR) {
        return BooleanV(true);
    } else {
        return BooleanV(false);
    }
}  // pair?

Value IsProcedure::evalRator(const Value &rand) {
    if(rand->v_type==V_PROC){
        return BooleanV(true);
    } else{
        return BooleanV(false);
    }
}  // procedure?

Value Not::evalRator(const Value &rand) {
    auto isv = dynamic_cast<Boolean *>(rand.get());
    if (isv && isv->b == false) {
        return BooleanV(true);
    } else{
        return BooleanV(false);
    }
}  // not

Value Car::evalRator(const Value &rand) {
    auto pair = dynamic_cast<Pair *>(rand.get());
    if (!pair) {
        throw RuntimeError("戳啦");
    }
    return Value(pair->car);
}  // car

Value Cdr::evalRator(const Value &rand) {
    auto pair = dynamic_cast<Pair *>(rand.get());
    if (!pair) {
        throw RuntimeError("戳啦");
    }
    return Value(pair->cdr);
}  // cdr
