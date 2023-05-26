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

#include "ltl.h"

namespace model::ltl {

std::vector<std::unique_ptr<const Formula>> Formula::_formulae;

std::ostream& operator <<(std::ostream &out, const Formula &formula) {
  switch (formula.kind()) {
  case Formula::ATOM:
    return out << formula.prop();
  case Formula::NOT:
    return out << "!(" << formula.arg() << ")";
  case Formula::AND:
    return out << "(" << formula.lhs() << ") && (" << formula.rhs() << ")";
  case Formula::OR:
    return out << "(" << formula.lhs() << ") || (" << formula.rhs() << ")";
  case Formula::IMPL:
    return out << "(" << formula.lhs() << ") -> (" << formula.rhs() << ")";
  case Formula::X:
    return out << "X(" << formula.arg() << ")";
  case Formula::G:
    return out << "G(" << formula.arg() << ")";
  case Formula::F:
    return out << "F(" << formula.arg() << ")";
  case Formula::U:
    return out << "(" << formula.lhs() << ") U (" << formula.rhs() << ")";
  case Formula::R:
    return out << "(" << formula.lhs() << ") R (" << formula.rhs() << ")";
  }

  return out;
}

bool Formula::operator ==(const Formula& f) const {
    if ( this->kind() != f.kind()) {
        return false;
    }
    if (this->prop() != f.prop()) {
        return false;
    }
    switch (this->kind()) {
        case ATOM:
            return true;
        case NOT:
            return this->arg() == f.arg();
        case X:
            return this->arg() == f.arg();
        case G:
            return this->arg() == f.arg();
        case F:
            return this->arg() == f.arg();
        case AND:
            return ((this->lhs() == f.lhs()) && (this->rhs() == f.rhs())) || ((this->rhs() == f.lhs()) && (this->lhs() == f.rhs()));
        case OR:
            return ((this->lhs() == f.lhs()) && (this->rhs() == f.rhs())) || ((this->rhs() == f.lhs()) && (this->lhs() == f.rhs()));
        case IMPL:
            return (this->lhs() == f.lhs()) && (this->rhs() == f.rhs());
        case U:
            return (this->lhs() == f.lhs()) && (this->rhs() == f.rhs());
        case R:
            return (this->lhs() == f.lhs()) && (this->rhs() == f.rhs());
    }
    return false;
}

} // namespace model::ltl
