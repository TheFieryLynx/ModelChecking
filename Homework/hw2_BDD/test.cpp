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

#include <cassert>
#include <iostream>

#include "bdd.h"
#include "formula.h"

using namespace model::bdd;
using namespace model::logic;

void run(const Formula& formula) {
    static int test_num = 1;
    std::cout << "------------------< Test #" << test_num << " >------------------" << std::endl;
    std::cout << "Formula: " << std::endl;
    std::cout << formula << std::endl;
    Bdd bdd;

    printBT(bdd.create(formula));
    
    test_num++;
    std::cout << std::endl;
}

int main() {
    run(x(0));
    run(x(0) >>  x(1));
    run((x(1) && x(2)) || (x(3) && x(4)) || (x(5) && x(6)));
    run((x(1) != x(2)));
    run((x(1) == x(2)));
    run((x(0) && x(1)) || (x(0) && x(1)));
    run(!x(1) >> !x(0));
    run(T);
    run(!(x(4) != x(5) != ((x(2) && x(3)) || (x(0) && x(1) && x(2)) || (x(0) && x(1) && x(3)))));
    run(((x(2) != x(5)) != ((x(1) && x(4)) || (x(0) && x(3) && x(1)) || (x(0) && x(3) && x(4)))));
    return 0;
}
