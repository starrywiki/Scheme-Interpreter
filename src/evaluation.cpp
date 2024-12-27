#include <cstring>
#include <map>
#include <vector>

#include "Debug.hpp"
#include "Def.hpp"
#include "RE.hpp"
#include "expr.hpp"
#include "syntax.hpp"
#include "value.hpp"

extern std ::map<std ::string, ExprType> primitives;
extern std ::map<std ::string, ExprType> reserved_words;

// by creating a new environment with bindings and evaluating the body
Value Let::eval(Assoc &env) {
    DEBUG_PRINT("Entering Let::eval");
    int len = bind.size();
    Assoc new_env = env;
    for (int i = 0; i < len; ++i) {
        new_env = extend(bind[i].first, bind[i].second->eval(env), new_env);
    }
    return body->eval(new_env);
}  // let expression

// Evaluates a `Letrec` expression by initializing bindings to `nullptr`, then
// updating them iteratively
Value Letrec::eval(Assoc &env) {
    int len = bind.size();
    DEBUG_PRINT("Entering Letrec::eval with " << len << " bindings.");
    Assoc e1 = env;  // Intermediate environment
    Assoc e2 = env;  // Final extended environment
    DEBUG_PRINT("Step 1: Initializing bindings with Value(nullptr).");
    for (int i = 0; i < len; ++i) {
        DEBUG_PRINT("Initializing " << bind[i].first
                                    << " with Value(nullptr).");
        e1 = extend(bind[i].first, Value(nullptr), e1);
    }
    DEBUG_PRINT("Step 2: Evaluating bindings in extended environment e1.");
    for (int i = 0; i < len; ++i) {
        DEBUG_PRINT("Evaluating binding " << bind[i].first << ".");
        Value var = bind[i].second->eval(e1);
        e2 = extend(bind[i].first, var, e2);
    }
    DEBUG_PRINT("Step 3: Modifying bindings in environment e2.");
    for (int i = 0; i < len; ++i) {
        DEBUG_PRINT("Modifying binding " << bind[i].first << ".");
        Value var = bind[i].second->eval(e2);
        modify(bind[i].first, var, e2);
    }
    DEBUG_PRINT("Evaluating body expression.");
    return body->eval(e2);
}  // letrec expression

// returning a closure with parameters, body, and environment
Value Lambda::eval(Assoc &env) {
    DEBUG_PRINT("Entering Lambda::eval");
    return ClosureV(x, e, env);
}  // lambda expression

// by invoking a function or lambda with given arguments
Value Apply::eval(Assoc &e) {
    DEBUG_PRINT("Entering Apply::eval");
    Value rval = rator->eval(e);
    Closure *clos = dynamic_cast<Closure *>(rval.get());
    if (!clos) {
        throw RuntimeError("WA");
    }
    int len1 = clos->parameters.size();
    int len2 = rand.size();
    if (len1 != len2) throw RuntimeError("WA");
    Assoc new_env = clos->env;
    std::vector<Value> varss;
    for (int i = 0; i < len2; ++i) {
        Value tmpval = rand[i]->eval(e);
        DEBUG_PRINT("Evaluated argument "
                    << i << ", value type: " << tmpval->v_type);
        varss.push_back(tmpval);
    }
    for (int i = 0; i < len2; ++i) {
        new_env = extend(clos->parameters[i], varss[i], new_env);
    }
    return (clos->e)->eval(new_env);
}  // for function calling

// Evaluates a `Var` expression by looking up the variable in the environment
Value Var::eval(Assoc &e) {
    DEBUG_PRINT("Entering Var::eval");
    if (std::isdigit(x[0]) || x[0] == '.' || x[0] == '@') {
        throw RuntimeError("WA");
    }
    Value isexist = find(x, e);
    if (isexist.get() == nullptr) {
        throw RuntimeError("WA");
    } else {
        return isexist;
    }
}  // evaluation of variable

Value Fixnum::eval(Assoc &e) {
    DEBUG_PRINT("Entering Fixnum::eval");
    return IntegerV(n);
}  // evaluation of a fixnum

// checking the condition and evaluating either the consequent or alternative
Value If::eval(Assoc &e) {
    DEBUG_PRINT("Entering IF::eval");
    Value firstv = cond->eval(e);
    Boolean* it = dynamic_cast<Boolean *>(firstv.get());
    if (it && it->b == false) {
        return alter->eval(e);
    } else {
        return conseq->eval(e);
    }
}  // if expression

Value True::eval(Assoc &e) { return BooleanV(true); }  // evaluation of #t

Value False::eval(Assoc &e) { return BooleanV(false); }  // evaluation of #f

Value Begin::eval(Assoc &e) {
    DEBUG_PRINT("Entering Begin::eval");
    int len = es.size();
    if (len == 0) {
        throw RuntimeError("WA");
    }
    for (int i = 0; i < len - 1; ++i) {
        Value it = es[i].get()->eval(e);
    }
    return es[len - 1].get()->eval(e);
}  // begin expression

Value Quote::eval(Assoc &e) {
    DEBUG_PRINT("Entering Quote::eval");
    // Check the quoted syntax's type and evaluate accordingly
    TrueSyntax* istrue = dynamic_cast<TrueSyntax *>(s.get());
    FalseSyntax* isfalse = dynamic_cast<FalseSyntax *>(s.get());
    Number* isnum = dynamic_cast<Number *>(s.get());
    Identifier* issymbol = dynamic_cast<Identifier *>(s.get());
    List* islists = dynamic_cast<List *>(s.get());
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
        } 
        // Handle dotted pairs (e.g., (a . b))
        else if (len == 3) {
            Identifier* dot = dynamic_cast<Identifier *>(islists->stxs[1].get());
            if (dot && dot->s == ".") {
                return PairV(
                    (Expr(new Quote(islists->stxs[0]))).get()->eval(e),
                    (Expr(new Quote(islists->stxs[2]))).get()->eval(e));
            }
        }
        // Handle special case lists(e.g.,(cdr(cdr (quote (1 . (2 . 3))))))
        bool isspecial = false;
        Value res = NullV();
        if (len >= 3) { 
            Identifier* dotp =
                dynamic_cast<Identifier *>(islists->stxs[len - 2].get());
            if (dotp && dotp->s == ".") {
                res = (Expr(new Quote(islists->stxs[len - 1]))).get()->eval(e);
                isspecial = true;
            }
        }
        // Evaluate special case lists
        if (isspecial) {
            for (int i = len - 3; i >= 0; --i) {
                res = PairV((Expr(new Quote(islists->stxs[i]))).get()->eval(e),
                            res);
            }
            return res;
        } 
        // Evaluate regular lists
        else {
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
        throw RuntimeError("WA");
    return IntegerV((dynamic_cast<Integer *>(rand1.get())->n) *
                    dynamic_cast<Integer *>(rand2.get())->n);
}  // *

Value Plus::evalRator(const Value &rand1, const Value &rand2) {
    if (rand1->v_type != V_INT || rand2->v_type != V_INT)
        throw RuntimeError("WA");
    return IntegerV((dynamic_cast<Integer *>(rand1.get())->n) +
                    dynamic_cast<Integer *>(rand2.get())->n);
}  // +

Value Minus::evalRator(const Value &rand1, const Value &rand2) {
    if (rand1->v_type != V_INT || rand2->v_type != V_INT)
        throw RuntimeError("WA");
    return IntegerV((dynamic_cast<Integer *>(rand1.get())->n) -
                    dynamic_cast<Integer *>(rand2.get())->n);
}  // -

Value Less::evalRator(const Value &rand1, const Value &rand2) {
    if (rand1->v_type != V_INT || rand2->v_type != V_INT)
        throw RuntimeError("WA");
    int num1 = dynamic_cast<Integer *>(rand1.get())->n;
    int num2 = dynamic_cast<Integer *>(rand2.get())->n;
    if (num1 < num2) return BooleanV(true);
    return BooleanV(false);
}  // <

Value LessEq::evalRator(const Value &rand1, const Value &rand2) {
    if (rand1->v_type != V_INT || rand2->v_type != V_INT)
        throw RuntimeError("WA");
    int num1 = dynamic_cast<Integer *>(rand1.get())->n;
    int num2 = dynamic_cast<Integer *>(rand2.get())->n;
    if (num1 <= num2) return BooleanV(true);
    return BooleanV(false);
}  // <=

Value Equal::evalRator(const Value &rand1, const Value &rand2) {
    if (rand1->v_type != V_INT || rand2->v_type != V_INT)
        throw RuntimeError("WA");
    int num1 = dynamic_cast<Integer *>(rand1.get())->n;
    int num2 = dynamic_cast<Integer *>(rand2.get())->n;
    if (num1 == num2) return BooleanV(true);
    return BooleanV(false);
}  // =

Value GreaterEq::evalRator(const Value &rand1, const Value &rand2) {
    if (rand1->v_type != V_INT || rand2->v_type != V_INT)
        throw RuntimeError("WA");
    int num1 = dynamic_cast<Integer *>(rand1.get())->n;
    int num2 = dynamic_cast<Integer *>(rand2.get())->n;
    if (num1 >= num2) return BooleanV(true);
    return BooleanV(false);
}  // >=

Value Greater::evalRator(const Value &rand1, const Value &rand2) {
    if (rand1->v_type != V_INT || rand2->v_type != V_INT)
        throw RuntimeError("WA");
    int num1 = dynamic_cast<Integer *>(rand1.get())->n;
    int num2 = dynamic_cast<Integer *>(rand2.get())->n;
    if (num1 > num2) return BooleanV(true);
    return BooleanV(false);
}  // >

// Evaluates equality between two values, supporting different types
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
    if (rand->v_type == V_PROC) {
        return BooleanV(true);
    } else {
        return BooleanV(false);
    }
}  // procedure?

Value Not::evalRator(const Value &rand) {
    Boolean* isv = dynamic_cast<Boolean *>(rand.get());
    if (isv && isv->b == false) {
        return BooleanV(true);
    } else {
        return BooleanV(false);
    }
}  // not

Value Car::evalRator(const Value &rand) {
    Pair* pair = dynamic_cast<Pair *>(rand.get());
    if (!pair) {
        throw RuntimeError("WA");
    }
    return Value(pair->car);
}  // car

Value Cdr::evalRator(const Value &rand) {
    Pair* pair = dynamic_cast<Pair *>(rand.get());
    if (!pair) {
        throw RuntimeError("WA");
    }
    return Value(pair->cdr);
}  // cdr
