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

#include "bdd.h"

namespace model::bdd {

const Node Bdd::zero(-1, nullptr, nullptr);
const Node Bdd::one (-1, nullptr, nullptr);


// https://stackoverflow.com/questions/36802354/print-binary-tree-in-a-pretty-way-using-c

void printBT(const std::string& prefix, const Node& node, bool isLeft) {
    std::cout << prefix;

    std::cout << (isLeft ? "└──── " : "╞ ═ ═ " );

    // print the value of the node
    if (&node == &Bdd::zero) {
      std::cout << "0" << std::endl;
      return;
    }

    if (&node == &Bdd::one) {
      std::cout << "1" << std::endl;
      return;
    }

    std::cout << "x" << node.var << std::endl;

    // enter the next tree level - left and right branch
    printBT( prefix + "      ", *node.low, false);
    printBT( prefix + "      ", *node.high, true);
}

void printBT(const Node& node)
{
    std::cout << std::endl;
    printBT("", node, false);
    std::cout << std::endl;
}

const Node& Bdd::compose(int var, const Node * low, const Node * high) {
    Node *elem = new Node(var, low, high);
    if (auto search = pool.find(*elem); search != pool.end()) {
        return search->second;
    }
    pool.insert({*elem, Node(var, low, high)});
    return pool[*elem];
}

const Node& Bdd::create(const Formula& formula) {
    if (formula.kind() == Formula::FALSE) {
        return Bdd::zero;
    }
    if (formula.kind() == Formula::TRUE) {
        return Bdd::one;
    }
    
    int var = getFormulaVar(formula);
    
    const Formula& applyLow = apply(formula, var, false);
    const Formula& applyHigh = apply(formula, var, true);
    
    const Node& low = create(applyLow);
    const Node& high = create(applyHigh);
    
    if (&low == &high) {
        return high;
    }
    
    return compose(var, &low, &high);
}

} // namespace model::bdd
