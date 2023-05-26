//
//  ltl_to_buchi.hpp
//  buchi-supremace
//
//  Created by Андрей Бирюков on 01.05.2023.
//

#include <stdio.h>
#include "fsm.h"
#include "ltl.h"

using namespace model::ltl;
using namespace model::fsm;

bool LOGS = true;

void printFormulas(std::vector<Formula>& f) {
    for (int i = 0; i < (int) f.size(); ++i) {
        std::cout << i << ") " << f[i] << std::endl;
    }
    std::cout << std::endl;
}

void printVector(std::vector<Formula>& f) {
    for (int i = 0; i < (int) f.size(); ++i) {
        std::cout << f[i] << ", ";
    }
    std::cout << std::endl;
}


const Formula& convert(const Formula& f) {
    switch (f.kind()) {
        case Formula::ATOM:
            return P(f.prop());
            
        case Formula::NOT:
            return !convert(f.arg());
            
        case Formula::AND:
            return convert(f.lhs()) && convert(f.rhs());
            
        case Formula::OR:
            return convert(f.lhs()) || convert(f.rhs());
            
        case Formula::IMPL: // (A1 -> A2) = !A1 | A2
            return !convert(f.lhs()) || convert(f.rhs());
            
        case Formula::X:
            return X(convert(f.arg()));
            
        case Formula::G: // G{A} = !F{!A}
            return !convert(F(!f.arg()));
            
        case Formula::F: // F{A} = {true} U {A}
            return U(P("true"), convert(f.lhs()));
            
        case Formula::U:
            return U(convert(f.lhs()), convert(f.rhs()));
            
        case Formula::R: // (A1 R A2) = !(!A1 U !A2)
            return !(U(!convert(f.lhs()), !convert(f.rhs())));
    }
    return f;
}

const Formula& processX(const Formula& f) {
    switch (f.kind()){
        case Formula::NOT: // X(!A) = !X(A)
            return !(processX(f.arg()));
            
        case Formula::AND:
            return processX(f.lhs()) && processX(f.rhs());
            
        case Formula::OR:
            return processX(f.lhs()) || processX(f.rhs());
            
        case Formula::X:
            switch (f.arg().kind()) {
                case Formula::ATOM:
                    if (f.arg().prop() == "true") {
                        return P("true");
                    }
                    if (f.arg().prop() == "false") {
                        return P("false");
                    }
                    return X(processX(f.arg()));
                    
                case Formula::NOT:
                    return !processX(X(f.arg().arg()));
                    
                case Formula::AND:
                    return (processX(X(f.arg().lhs())) && processX(X(f.arg().rhs())));
                    
                case Formula::OR:
                    return (processX(X(f.arg().lhs())) || processX(X(f.arg().rhs())));
                    
                case Formula::U:
                    return U(processX(X(f.arg().lhs())), processX(X(f.arg().rhs())));
                    
                default:
                    return X(processX(f.arg()));
            }
            
        case Formula::U:
            return U(processX(f.lhs()), processX(f.rhs()));
            
        default:
            return f;
    }
}

const Formula& processNeg(const Formula& f) {
    switch (f.kind()){
        case Formula::NOT:
            switch (f.arg().kind()) {
                case Formula::ATOM:
                    if (f.arg().prop() == "true") {
                        return P("false");
                    }
                    if (f.arg().prop() == "false") {
                        return P("true");
                    }
                    return f;
                case Formula::NOT:
                    return processNeg(f.arg().arg());
                case Formula::OR:
                    return processNeg(!f.arg().lhs() && !f.arg().rhs());
                case Formula::AND:
                    return processNeg(!f.arg().lhs() || !f.arg().rhs());
                default:
                    return !(processNeg(f.arg()));
            }
            
        case Formula::AND:
            return processNeg(f.lhs()) && processNeg(f.rhs());
            
        case Formula::OR:
            return processNeg(f.lhs()) || processNeg(f.rhs());
            
        case Formula::X:
            return X(processNeg(f.arg()));
            
        case Formula::U:
            return U(processNeg(f.lhs()), processNeg(f.rhs()));
            
        default:
            return f;
    }
}

// 1) раскрыть все формулы
// 2) внести внутрь все X
// 3) убрать двойные отрицания
const Formula& simplify(const Formula& f) {
    const Formula& f1 = convert(f);
    if (LOGS)
        std::cout << "Convered: " << f1 << std::endl;
    const Formula& f2 = processX(f1);
    if (LOGS)
        std::cout << "X proceeded: " << f2 << std::endl;
    const Formula& f3 = processNeg(f2);
    return f3;
}

bool contains(const std::vector<Formula>& vFormulas, const Formula& f) {
    for (Formula fi : vFormulas) {
        if (fi == f) {
            return true;
        }
    }
    return false;
}

void push_back_if_not_contains(std::vector<Formula>& vFormulas, const Formula& f) {
    if (!contains(vFormulas, f)) {
        vFormulas.push_back(f);
    }
}

void closure_and_atoms(const Formula& f, std::vector<Formula>& vClosure, std::vector<Formula>& vAtoms) {
    switch (f.kind()) {
        case Formula::ATOM:
            push_back_if_not_contains(vClosure, f);
            if (f.prop() != "true" && f.prop() != "false") {
                push_back_if_not_contains(vAtoms, f);
            }
            break;
        case Formula::NOT:
            closure_and_atoms(f.arg(), vClosure, vAtoms);
            break;
        case Formula::AND:
        {
            closure_and_atoms(f.lhs(), vClosure, vAtoms);
            closure_and_atoms(f.rhs(), vClosure, vAtoms);
            push_back_if_not_contains(vClosure, f);
            break;
        }
        case Formula::OR:
        {
            closure_and_atoms(f.lhs(), vClosure, vAtoms);
            closure_and_atoms(f.rhs(), vClosure, vAtoms);
            push_back_if_not_contains(vClosure, f);
            break;
        }
        case Formula::U:
        {
            closure_and_atoms(f.lhs(), vClosure, vAtoms);
            closure_and_atoms(f.rhs(), vClosure, vAtoms);
            push_back_if_not_contains(vClosure, f);
            break;
        }
        case Formula::X:
        {
            closure_and_atoms(f.lhs(), vClosure, vAtoms);
            push_back_if_not_contains(vClosure, f);
            push_back_if_not_contains(vAtoms, f);
            break;
        }
        default:
            break;
            
    }
}

int check_true(const Formula& f, std::vector<Formula> tableRow) {
    switch (f.kind()) {
        case Formula::ATOM:
            if (f.prop() == "true")
                return 1;
            if (f.prop() == "false") {
                return 0;
            }
            if (contains(tableRow, f)) {
                return 1;
            }
            return 0;
        case Formula::X:
            if (contains(tableRow, f)) {
                return 1;
            }
            return 0;
        case Formula::NOT: {
            int tmp = check_true(f.arg(), tableRow);
            if (tmp == 1) {
                return 0;
            } else if (tmp == 0) {
                return 1;
            }
            return tmp;
        }
        case Formula::AND:
        {
            int left = check_true(f.lhs(), tableRow);
            int right = check_true(f.rhs(), tableRow);
            if (left == -1 || right == -1) {
                return -1;
            }
            if (left == 1 && right == 1) {
                return 1;
            }
            return 0;
        }
        case Formula::OR:
        {
            int left = check_true(f.lhs(), tableRow);
            int right = check_true(f.rhs(), tableRow);
            if (left == 1 || right == 1) {
                return 1;
            }
            if (left == -1 || right == -1) {
                return -1;
            }
            return 0;
        }
        default:
            return -1;
    }
}

bool check_true_formula_existance(const std::vector<Formula>& curState, const Formula& f) {
    // 1 - формула верна - надо добавить / должна уже быть в curState
    // 0 - формула неверна - не надо добавлять
    switch (f.kind()) {
        case Formula::ATOM:
            return contains(curState, f);
        case Formula::NOT:
            return !check_true_formula_existance(curState, f.arg());
        case Formula::AND:
        {
            bool left = check_true_formula_existance(curState, f.lhs());
            bool right = check_true_formula_existance(curState, f.rhs());
            if (left && right) {
                return true;
            }
            return false;
        }
        case Formula::OR:
        {
            bool left = check_true_formula_existance(curState, f.lhs());
            bool right = check_true_formula_existance(curState, f.rhs());
            if (left || right) {
                return true;
            }
            return false;
        }
        case Formula::U:
            return contains(curState, f);
        case Formula::X:
            return contains(curState, f);
        default:
            std::cout << "?!?!?!?!?!?!" << std::endl;
            return false;;
    }
}

void printBits(int num, int size)
{
    for (int i = 0; i < size; ++i) {
        int b = num >> (size - i - 1);
        if (b & 1)
            std::cout << "1";
        else
            std::cout << "0";
    }
    std::cout << std::endl;
}

std::map<std::string, std::vector<Formula>> compute_states(const std::vector<Formula>& vClosure, const std::vector<Formula>& vAtoms) {
    
    std::map<std::string, std::vector<Formula>> states;
    int states_num = 1;
    
    // табличка истинности по атомам
    for (int i = 0; i < (1 << vAtoms.size()); ++i) {
        std::vector<Formula> trueTableRowItems;
        for (int j = 0; j < (int) vAtoms.size(); ++j) {
            bool exists_in_table = (i >> j) & 1;
            if (exists_in_table) {
                trueTableRowItems.push_back(vAtoms[vAtoms.size() - j - 1]);
            }
        }
        
        std::vector<Formula> vIntense;
        for (Formula clousureItem : vClosure) {
            if (check_true(clousureItem, trueTableRowItems) == 1) {
                vIntense.push_back(clousureItem);
            }
        }
        if (LOGS) {
            std::cout << "Table row: ";
            printBits(i, (int) vAtoms.size());
            std::cout << "First step:" << std::endl;
            printVector(vIntense);
            std::cout << std::endl;
        }
        std::vector<std::vector<Formula>> states_for_row;
        states_for_row.push_back(vIntense);
        
        for (Formula clousureItem : vClosure) {
            size_t size = states_for_row.size();
            if (clousureItem.kind() == Formula::U) {
                for (int vectorIndex = 0; vectorIndex < (int) size; ++vectorIndex) {
                    if (clousureItem.rhs().kind() == Formula::NOT &&
                                !contains(states_for_row[vectorIndex], clousureItem.rhs().arg())) {
                        states_for_row[vectorIndex].push_back(clousureItem);
                    } else if (contains(states_for_row[vectorIndex], clousureItem.rhs())) {
                        states_for_row[vectorIndex].push_back(clousureItem);
                    } else if (clousureItem.lhs().kind() == Formula::NOT &&
                               !contains(states_for_row[vectorIndex], clousureItem.lhs().arg())) {
                        std::vector<Formula> tmp(states_for_row[vectorIndex]);
                        tmp.push_back(clousureItem);
                        states_for_row.push_back(tmp);
                    } else if (contains(states_for_row[vectorIndex], clousureItem.lhs())) {
                        std::vector<Formula> tmp(states_for_row[vectorIndex]);
                        tmp.push_back(clousureItem);
                        states_for_row.push_back(tmp);
                    }
                    
                }
                for (int vectorIndex = 0; vectorIndex < (int) states_for_row.size(); ++vectorIndex) {
                    for (Formula clousureItem : vClosure) {
                        if (check_true_formula_existance(states_for_row[vectorIndex], clousureItem)) {
                            if (!contains(states_for_row[vectorIndex], clousureItem)) {
                                states_for_row[vectorIndex].push_back(clousureItem);
                            }
                        }
                    }
                }
                
            }
        }
        
        if (LOGS) {
            std::cout << "States for row: " << std::endl;
            for (int vectorIndex = 0; vectorIndex < (int) states_for_row.size(); ++vectorIndex) {
                std::cout << vectorIndex + 1 << ") ";
                printVector(states_for_row[vectorIndex]);
            }
            std::cout << std::endl;
            std::cout << "=======================================================\n";
            std::cout << std::endl;
        }
        
        for (std::vector<Formula> state : states_for_row) {
            states.insert({"s" + std::to_string(states_num), state});
            ++states_num;
        }
    }
    return states;
}

std::vector<Formula> getUFormulas(const std::vector<Formula>& vClosure) {
    std::vector<Formula> uFormulas;
    for (Formula f : vClosure) {
        if (f.kind() == Formula::U) {
            uFormulas.push_back(f);
        }
    }
    return uFormulas;
}

std::vector<Formula> getXFormulas(const std::vector<Formula>& vClosure) {
    std::vector<Formula> xFormulas;
    for (Formula f : vClosure) {
        if (f.kind() == Formula::X) {
            xFormulas.push_back(f);
        }
    }
    return xFormulas;
}

bool check_left_condition(const Formula& uF, const std::vector<Formula>& state) {
    return contains(state, uF);
}

bool check_right_unity_first(const Formula& psi, const std::vector<Formula>& state) {
    if (psi.kind() == Formula::NOT && !contains(state, psi.arg())) {
        return true;
    }
    return contains(state, psi);
}

bool check_right_intersection_first(const Formula& xi,  const std::vector<Formula>& state) {
    if (xi.kind() == Formula::NOT && !contains(state, xi.arg())) {
        return true;
    }
    return contains(state, xi);
}

int check_right_uf_presence(const bool& left_condition, const bool& right_unity_first,
                            const bool& right_intersection_first) {
    // 1 - U присутствует в s'
    // 0 - U не присуствуте в s'
    // -1 - без разницы
    if (left_condition == true && right_unity_first == true) {
        return -1;
    } else if (left_condition == true && right_unity_first == false) {
        if (right_intersection_first == true) {
            return 1;
        }
    } else if (left_condition == false && right_unity_first == false) {
        if (right_intersection_first == true) {
            return 0;
        } else {
            return -1;
        }
    }
    std::cout << "????????????????" << std::endl;
    return -1;
}

bool containsCorrectAtoms(const std::vector<Formula>& vFormulas, std::map<std::string, bool> xFormalasPresence) {
    for (const auto& [f, presence] : xFormalasPresence) {
        if (contains(vFormulas, P(f)) != presence) {
            return false;
        }
    }
    return true;
}

std::set<std::string> makeSetOfAtoms (const std::vector<Formula>& vFromState) {
    std::set<std::string> atoms;
    for (Formula f : vFromState) {
        if (f.kind() == Formula::ATOM) {
            if (f.prop() != "true" && f.prop() != "false") {
                atoms.insert(f.prop());
            }
        }
    }
    return atoms;
}

void set_final_states(const Formula& uFormula, int& index,
                      const std::map<std::string, std::vector<Formula>>& states,
                      Automaton& automation) {
    for (const auto& [name, vState] : states) {
        if (!contains(vState, uFormula)) {
            automation.set_final(name, index);
            continue;
        }
        if (uFormula.rhs().kind() == Formula::NOT) {
            if (!contains(vState, uFormula.rhs().arg())) {
                automation.set_final(name, index);
                continue;
            }
        } else {
            if (contains(vState, uFormula.rhs())) {
                automation.set_final(name, index);
                continue;
            }
        }
        
    }
}

Automaton states_to_automation(const std::vector<Formula>& vClosure,
                               const std::map<std::string, std::vector<Formula>>& states) {
    Automaton automation;
    if (LOGS) {
        std::cout << "States:" << std::endl;
        for (const auto& [name, vState] : states) {
            std::cout << name << ": ";
            for (auto f : vState) {
                std::cout << f << ", ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
    
    
    std::vector<Formula> uFormulas = getUFormulas(vClosure);
    std::vector<Formula> xFormulas = getXFormulas(vClosure);
    for (const auto& [nameFrom, vFromState] : states) {
        automation.add_state(nameFrom);
    }
    
    if (LOGS) {
        std::cout << "ALL X:" << std::endl;
        printFormulas(xFormulas);
    }
    
    for (const auto& [nameFrom, vFromState] : states) {
        std::map<std::string, bool> xFormalasPresence;
        std::set<std::string> fromStateAtoms = makeSetOfAtoms(vFromState);
        
        for (Formula xF : xFormulas) {
            xFormalasPresence.insert({xF.arg().prop(), contains(vFromState, xF)});
        }

        std::vector<std::pair<Formula, int>> uStates;
        for (const Formula& uF : uFormulas) {
            bool left_condition = check_left_condition(uF, vFromState);
            bool right_unity_first = check_right_unity_first(uF.rhs(), vFromState);
            bool right_intersection_first = check_right_intersection_first(uF.lhs(), vFromState);
            int right_uf_presence = check_right_uf_presence(left_condition, right_unity_first, right_intersection_first);
            uStates.push_back(std::pair(uF, right_uf_presence));
        }
        
        for (const auto& [nameTo, vToState] : states) {
            bool accept = true;
            for (const auto& uState : uStates) {
                if (uState.second == 0) {
                    if (contains(vToState, uState.first)) {
                        accept = false;
                        
                    }
                }
                if (uState.second == 1) {
                    if (!contains(vToState, uState.first)) {
                        accept = false;
                    }
                }
            }
            if (!containsCorrectAtoms(vToState, xFormalasPresence)) {
                accept = false;
            }
            if (accept) {
                automation.add_trans(nameFrom, fromStateAtoms, nameTo);
            }
        }
        
    }
    for (int i = 1; i <= (int) uFormulas.size(); ++i) {
        set_final_states(uFormulas[i - 1], i, states, automation);
    }
    return automation;
}

void set_init_states(const Formula& initFormula,
                     const std::map<std::string, std::vector<Formula>>& states,
                     Automaton& automation) {
    if (initFormula.kind() == Formula::NOT) {
        for (const auto& [name, vState] : states) {
            if (!contains(vState, initFormula.arg())) {
                automation.set_initial(name);
            }
        }
    } else {
        for (const auto& [name, vState] : states) {
            if (contains(vState, initFormula)) {
                automation.set_initial(name);
            }
        }
    }
}

void ltl_to_buchi(const Formula& f, bool logsEnable) {
    static int test_num = 1;
    std::cout << "------------------< Test #" << test_num << " >------------------" << std::endl;
    LOGS = logsEnable;
    std::cout << "Initial formula: " << f << std::endl;
    const Formula& simpl_f = simplify(f);
    std::cout << "Simplified formula: " << simpl_f << std::endl;
    std::cout << std::endl;
    
    std::vector<Formula> vClosure;
    std::vector<Formula> vAtoms;
    closure_and_atoms(simpl_f, vClosure, vAtoms);
    if (LOGS) {
        std::cout << "Closure: " << std::endl;
        printFormulas(vClosure);
        std::cout << "Atoms: " << std::endl;
        printFormulas(vAtoms);
    }
    auto states = compute_states(vClosure, vAtoms);
    Automaton automation = states_to_automation(vClosure, states);
    set_init_states(simpl_f, states, automation);
    std::cout << automation << std::endl;
    test_num++;
    std::cout << std::endl;
}
