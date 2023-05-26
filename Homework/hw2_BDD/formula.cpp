/*
 * Copyright 2021 ISP RAS (http://www.ispras.ru)
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
 * in compliance with the License. You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License
 * is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
 * or implied. See the License for the specific language governing permissions and limitations under
 * the License.
 */

#include "formula.h"

namespace model::logic {

const Formula Formula::F(Formula::FALSE);
const Formula Formula::T(Formula::TRUE);

const Formula F = Formula::F;
const Formula T = Formula::T;

std::vector<std::unique_ptr<const Formula>> Formula::_formulae;

std::ostream& operator <<(std::ostream &out, const Formula &formula) {
  switch (formula.kind()) {
  case Formula::FALSE:
    return out << "false";
  case Formula::TRUE:
    return out << "true";
  case Formula::VAR:
    return out << "x" << formula.var();
  case Formula::NOT:
    return out << "!(" << formula.arg() << ")";
  case Formula::AND:
    return out << "(" << formula.lhs() << ") && (" << formula.rhs() << ")";
  case Formula::OR:
    return out << "(" << formula.lhs() << ") || (" << formula.rhs() << ")";
  case Formula::XOR:
    return out << "(" << formula.lhs() << ") != (" << formula.rhs() << ")";
  case Formula::IMPL:
    return out << "(" << formula.lhs() << ") -> (" << formula.rhs() << ")";
  case Formula::EQ:
    return out << "(" << formula.lhs() << ") == (" << formula.rhs() << ")";
  }

  return out;
}

int getFormulaVar(const Formula& formula) {
    switch (formula.kind()) {
        case Formula::FALSE:
            return formula.var();

        case Formula::TRUE:
            return formula.var();

        case Formula::VAR:
            return formula.var();

        case Formula::NOT:
            return getFormulaVar(formula.arg());

        case Formula::AND:
        {
            int left = getFormulaVar(formula.lhs());
            int right = getFormulaVar(formula.rhs());
            if (left == -1) {
                return right;
            }
            if (right == -1) {
                return left;
            }
            if (left < right) {
                return left;
            }
            return right;
        }
        case Formula::OR:
        {
            int left = getFormulaVar(formula.lhs());
            int right = getFormulaVar(formula.rhs());
            if (left == -1) {
                return right;
            }
            if (right == -1) {
                return left;
            }
            if (left < right) {
                return left;
            }
            return right;
        }
        case Formula::XOR:
        {
            int left = getFormulaVar(formula.lhs());
            int right = getFormulaVar(formula.rhs());
            if (left == -1) {
                return right;
            }
            if (right == -1) {
                return left;
            }
            if (left < right) {
                return left;
            }
            return right;
        }
        case Formula::IMPL:
        {
            int left = getFormulaVar(formula.lhs());
            int right = getFormulaVar(formula.rhs());
            if (left == -1) {
                return right;
            }
            if (right == -1) {
                return left;
            }
            if (left < right) {
                return left;
            }
            return right;
        }
        case Formula::EQ:
        {
            int left = getFormulaVar(formula.lhs());
            int right = getFormulaVar(formula.rhs());
            if (left == -1) {
                return right;
            }
            if (right == -1) {
                return left;
            }
            if (left < right) {
                return left;
            }
            return right;
        }
    }
}

const Formula& apply(const Formula& formula, const int& var, const bool& state) {
    switch (formula.kind()) {
        case Formula::FALSE:
            return F;
            
        case Formula::TRUE:
            return T;
        
        case Formula::VAR:
            if (var == formula.var()) {
                if (state) {
                    return T;
                }
                return F;
            }
            return formula;
        
        case Formula::NOT:
        {
            const Formula& argnot = apply(formula.arg(), var, state);
            if (argnot.kind() == Formula::TRUE) {
              return F;
            } else if (argnot.kind() == Formula::FALSE) {
              return T;
            }
            return !argnot;
        }
           
        case Formula::AND:
        {
            const Formula& left = apply(formula.lhs(), var, state);
            const Formula& right = apply(formula.rhs(), var, state);
            // F && x -> F, x && F -> F
            if (left.kind() == Formula::FALSE || right.kind() == Formula::FALSE) {
                return F;
            }
            // T && x -> x
            if (left.kind() == Formula::TRUE) {
                return right;
            }
            // x && T -> x
            if (right.kind() == Formula::TRUE) {
                return left;
            }
            // x && y -> x && y
            return left && right;
        }

        case Formula::OR:
        {
            const Formula& left = apply(formula.lhs(), var, state);
            const Formula& right = apply(formula.rhs(), var, state);
            // T || x -> T, x || T -> T
            if (left.kind() == Formula::TRUE || right.kind() == Formula::TRUE) {
                return T;
            }
            // F || x -> x
            if (left.kind() == Formula::FALSE) {
                return right;
            }
            // x || F -> x
            if (right.kind() == Formula::FALSE) {
                return left;
            }
            // x || y -> x || y
            return left || right;
        }

        case Formula::XOR:
        {
            const Formula& left = apply(formula.lhs(), var, state);
            const Formula& right = apply(formula.rhs(), var, state);
            // T + T -> F
            if (left.kind() == Formula::TRUE && right.kind() == Formula::TRUE) {
                return F;
            }
            // F + F -> F
            if (left.kind() == Formula::FALSE && right.kind() == Formula::FALSE) {
                return right;
            }
            // T + F -> T
            if (left.kind() == Formula::TRUE && right.kind() == Formula::FALSE) {
                return T;
            }
            // F + T -> T
            if (left.kind() == Formula::FALSE && right.kind() == Formula::TRUE) {
                return T;
            }
            // x + y -> x + y
            return left != right;
        }
            
        case Formula::IMPL:
        {
            const Formula& left = apply(formula.lhs(), var, state);
            const Formula& right = apply(formula.rhs(), var, state);
            // F -> неважно что = T
            if (left.kind() == Formula::FALSE) {
                return T;
            }
            // T -> F = F
            if (left.kind() == Formula::TRUE && right.kind() == Formula::FALSE) {
                return F;
            }
            // T -> T = T
            if (left.kind() == Formula::TRUE && right.kind() == Formula::TRUE) {
                return T;
            }
            // x -> y = x -> y
            return left >> right;
        }
            
        case Formula::EQ:
        {
            const Formula& left = apply(formula.lhs(), var, state);
            const Formula& right = apply(formula.rhs(), var, state);
            // T == T = T
            if (left.kind() == Formula::TRUE && right.kind() == Formula::TRUE) {
                return T;
            }
            // F == F = T
            if (left.kind() == Formula::FALSE && right.kind() == Formula::FALSE) {
                return T;
            }
            // F == T = F
            if (left.kind() == Formula::FALSE && right.kind() == Formula::TRUE) {
                return F;
            }
            // T == F = F
            if (left.kind() == Formula::TRUE && right.kind() == Formula::FALSE) {
                return F;
            }
            // x == y = x == y
            return left == right;
        }
    }
}

} // namespace model::logic
