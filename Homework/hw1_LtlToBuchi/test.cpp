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
#include "fsm.h"
#include "ltl_to_buchi.h"

using namespace model::ltl;
using namespace model::fsm;


int main() {
    bool logs = true;
    ltl_to_buchi(   G(P("p") >> F(P("q"))), logs);
    ltl_to_buchi(   U(P("x"), U(P("y"), P("z"))), logs);
    ltl_to_buchi(   G(P("p") >> (!P("q") && (U(X(!P("p")), P("q"))))), logs);
    ltl_to_buchi(   U(P("true"), P("p")) || P("q"), logs);
    ltl_to_buchi(   !U(P("true"), P("p")) || P("q"), logs);
    ltl_to_buchi(   F(P("p") >> ( !P("q") && ( X(U(!P("p"), P("q")) )))), logs);
    ltl_to_buchi(   X((P("false") && P("false")) || P("true")), logs);
    ltl_to_buchi(   (P("p") && P("p")) || P("d"), logs);
    ltl_to_buchi(   G(F(X(P("p")))), logs);
    ltl_to_buchi(   F(P("p")), logs); 
    ltl_to_buchi(   P("x") || !U(P("y"), P("z")), logs);
    ltl_to_buchi(   F(P("p")) || P("q"), logs);
    ltl_to_buchi(   U((P("p") || P("q")),(P("p") && P("q"))), logs);
    ltl_to_buchi(   F(P("p") >> X(!(P("q")))), logs);
  return 0;
}

