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

Value Lambda::eval(Assoc &env) {}  // lambda expression

Value Apply::eval(Assoc &e) {}  // for function calling

Value Letrec::eval(Assoc &env) {}  // letrec expression

Value Var::eval(Assoc &e) {}  // evaluation of variable

Value Fixnum::eval(Assoc &e) { return IntegerV(n); }  // evaluation of a fixnum

Value If::eval(Assoc &e) {}  // if expression

Value True::eval(Assoc &e) { return BooleanV(true); }  // evaluation of #t

Value False::eval(Assoc &e) { return BooleanV(false); }  // evaluation of #f

Value Begin::eval(Assoc &e) {}  // begin expression

Value Quote::eval(Assoc &e) {
    auto istrue = dynamic_cast<TrueSyntax *>(s.get());
    auto isfalse = dynamic_cast<FalseSyntax *>(s.get());
    auto isnum = dynamic_cast<Number *>(s.get());
    auto issymbol = dynamic_cast<Identifier *>(s.get());
    auto islists = dynamic_cast<List *>(s.get());
    if (istrue) {
        return BooleanV(true);
    } else if (isfalse) {
        return BooleanV(true);
    } else if (isnum) {
        return IntegerV(isnum->n);
    } else if (issymbol && issymbol->s != ".") {
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
        Value res = NullV();
        for (int i = len - 1; i >= 0; --i) {
            if (i == len - 2 &&
                dynamic_cast<Identifier *>(islists->stxs[len - 2].get())->s ==
                    ".") {
                continue;
            }
            res =
                PairV((Expr(new Quote(islists->stxs[i]))).get()->eval(e), res);
        }
        return res;
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

Value IsEq::evalRator(const Value &rand1, const Value &rand2) {}  // eq?

Value Cons::evalRator(const Value &rand1, const Value &rand2) {
    return PairV(rand1, rand2);
}  // cons

Value IsBoolean::evalRator(const Value &rand) {}  // boolean?

Value IsFixnum::evalRator(const Value &rand) {}  // fixnum?

Value IsSymbol::evalRator(const Value &rand) {}  // symbol?

Value IsNull::evalRator(const Value &rand) {}  // null?

Value IsPair::evalRator(const Value &rand) {}  // pair?

Value IsProcedure::evalRator(const Value &rand) {}  // procedure?

Value Not::evalRator(const Value &rand) {}  // not

Value Car::evalRator(const Value &rand) {
    auto pair = dynamic_cast<Pair *>(rand.get());
    if (!pair) {
        throw std::runtime_error("car: Argument is not a pair");
    }
    return Value(pair->car);
}  // car

Value Cdr::evalRator(const Value &rand) {
    auto pair = dynamic_cast<Pair *>(rand.get());
    if (!pair) {
        throw std::runtime_error("cdr: Argument is not a pair");
    }
    return Value(pair->cdr);
}  // cdr
