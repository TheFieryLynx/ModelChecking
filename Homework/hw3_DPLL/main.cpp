//
//  main.cpp
//  DPLL
//
//  Created by Андрей Бирюков on 16.05.2023.
//

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <set>
#include <map>
#include <vector>
#include <stack>
#include <ctime>

enum State {
    TRUE,         // 0   говорим что формула БЕЗ отрицания true
    FALSE,        // 1   говорим что формула С отрицанием true
    UNDEFINED,       // 2
    ZERO,          // просто отдельный state для 0 элемента вектора
    
};

// нам надо запоминать:
// переменную
// то что изменилось при прокидывании одиночных clause
// значение которое мы дали, чтоб его заменить на другое при откате
struct stackElem {
    int elem;
    std::vector<int> changedElems;
    State val;
};

typedef std::vector<std::vector<int>> vVector;

// так как рекурсия запрещена - используем stack :(
std::stack<stackElem> dpllStack;

void printStackTop() {
    std::cout << dpllStack.top().elem << ", { ";
    for (auto i : dpllStack.top().changedElems) {
        std::cout << i << " ";
    }
    std::cout << "}, ";
    if (dpllStack.top().val == State::TRUE) {
        std::cout << "true" << std::endl;
    } else {
        std::cout << "false" << std::endl;
    }
}

void printDoubleVector(const vVector& v) {
    std::cout << std::endl;
    int i = 0;
    for (auto s : v) {
        std::cout << i << ": ";
        for (auto j : s) {
            std::cout << j << " ";
        }
        i++;
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void printValStates(std::vector<State>& val_states) {
    std::cout << std::endl;
    for (int i = 1; i < (int)val_states.size(); ++i) {
        std::cout << i << ": " << val_states[i] << std::endl;
    }
    std::cout << std::endl;
}

void printSet(const std::set<int>& s) {
    for (auto e : s) {
        std::cout << e << " ";
        
    }
    std::cout << std::endl;
}

State getState(const int& value, const std::vector<State>& val_states) {
    int val = abs(value);
    if (val_states[val] == State::UNDEFINED) {
        return State::UNDEFINED;
    }
    
    if (value < 0) {
        if (val_states[val] == State::TRUE) {
            return State::FALSE;
        }
        return State::TRUE;
    }
    return val_states[val];
}

vVector process_cnf_file(std::string filepath, vVector& clauses, std::vector<State>& val_states) {
    std::ifstream in(filepath);
    std::string line;
    
    int vars_num;
    int lines_num;
    std::string tmp;
    
    if (in.is_open()) {
        while (std::getline(in, line) && line[0] != 'p');
        std::stringstream lineStream(line);
        lineStream >> tmp >> tmp >> vars_num >> lines_num;
        std::vector<State> val_states_tmp(vars_num + 1, State::UNDEFINED);
        val_states = val_states_tmp;
        val_states[0] = State::ZERO;
        while (lines_num != 0) {
            std::getline(in, line);
            std::stringstream lineStream(line);
            std::vector<int> clause;
            int var;
            while(lineStream >> var) {
                if (var == 0) {
                    break;
                }
                clause.push_back(var);
            }
            clauses.push_back(clause);
            lines_num--;
        }
    } else {
        std::cout << "File is not open ????" << std::endl;
    }
    
    in.close();
    return clauses;
}

// возвращает элемент, если в клаузе есть один нужный элемент
// 0 - иначе
int getElemOfSingleClause(const std::vector<int>& clause, const std::vector<State>& val_states) {
    int length = 0;
    int ret = 0;
    for (int i : clause) {
        State i_state = getState(i, val_states);
        if (i_state == State::TRUE) {
            return 0;
        } else if (i_state == State::UNDEFINED) {
            ++length;
            ret = i;
        }
        if (length > 1) {
            return 0;
        }
    }
    
    return ret;
}

void updateValues(int& elem, std::vector<State>& val_states) {
    bool stackEmpty = dpllStack.empty();
    if (elem > 0) {
        val_states[abs(elem)] = State::TRUE;
    } else {
        val_states[abs(elem)] = State::FALSE;
    }
    
    if (!stackEmpty) {
        dpllStack.top().changedElems.push_back(elem);
    }
}

void proceedSingularClauses(const vVector& clauses, std::vector<State>& val_states) {
    for (auto& s : clauses) {
        int elem = getElemOfSingleClause(s, val_states);
        if (elem != 0) {
            updateValues(elem, val_states);
        }
    }
}

bool checkClausesTrue(const vVector& clauses, std::vector<State>& val_states) {
    for (const auto& s : clauses) {
        bool isTrue = false;
        for (auto i : s) {
            if (getState(i, val_states) == State::TRUE) {
                isTrue = true;
                break;
            }
        }
        if (!isTrue) {
            return false;
        }
    }

    return true;
}


void setFirstValInStack(std::vector<State>& val_states) {
    for (int i = 1; i < (int)val_states.size(); ++i) {
        if (getState(i, val_states) == State::UNDEFINED) {
            dpllStack.push({i, {}, State::TRUE});
            val_states[abs(i)] = State::TRUE;
            return;
        }
    }
}

bool checkFalse(const std::vector<int>& clause, std::vector<State>& val_states) {
    for (auto& i : clause) {
        if (getState(i, val_states) != State::FALSE) {
            return false;
        }
    }
    return true;
}

// нужен откат если клауза состоит только из FALSE переменных
bool needToRollback(const vVector& clauses, std::vector<State>& val_states) {
    for (const auto &c : clauses) {
        if (checkFalse(c, val_states)) {
            return true;
        }
    }
    return false;
}

void rollback(std::vector<State>& val_states) {
    while (true) {
        for (auto &v : dpllStack.top().changedElems) {
            val_states[abs(v)] = State::UNDEFINED;
        }
        dpllStack.top().changedElems.clear();

        if (dpllStack.top().val == State::TRUE) {

            dpllStack.top().val = State::FALSE;

            val_states[abs(dpllStack.top().elem)] = State::FALSE;
            break;
        } else {
            val_states[abs(dpllStack.top().elem)] = State::UNDEFINED;
            dpllStack.pop();
            if (dpllStack.empty()) {
                break;
            }
        }
    }
}

bool run(vVector& clauses, std::vector<State>& val_states) {
    while (true) {
        // распространить единичную клаузу
        proceedSingularClauses(clauses, val_states);
        
        if (needToRollback(clauses, val_states)) {
            if (!dpllStack.empty()) {
                rollback(val_states);
            }
            if (dpllStack.empty()) {
                return false;
            }
        } else {
            if (checkClausesTrue(clauses, val_states)) {
                return true;
            }
            setFirstValInStack(val_states);
        }
    }
    return false;
}

int main(void) {
    std::vector<std::pair<std::string, bool>> tests {
        {"aim-50-1_6-yes1-1.cnf", true},
        {"aim-50-1_6-yes1-2.cnf", true},
        {"aim-50-1_6-yes1-3.cnf", true},
        {"aim-50-1_6-yes1-4.cnf", true},
        // {"uuf100-01.cnf", false},
        // {"uuf100-02.cnf", false},
        // {"uf100-01.cnf", true},
        // {"uf100-02.cnf", true},
        // {"hole6.cnf", false},
        // {"hole7.cnf", false},
        // {"hole8.cnf", false},
        // {"hanoi4.cnf", true},
        
    };
    
    for (int i = 0; i < (int)tests.size(); ++i) {
        while (!dpllStack.empty()) {
            dpllStack.pop();
        }
        std::cout << "Run Test #" << i + 1 << " (" << tests[i].first << ")" << std::endl;
        
        vVector clauses;
        std::vector<State> val_states;
        
        // чтение файла
        process_cnf_file("tests/" + tests[i].first, clauses, val_states);
        bool res;
        clock_t start = clock();
        
        res = run(clauses, val_states);
        
        clock_t end = clock();
        assert(res == tests[i].second);
        double time = double(end - start) / CLOCKS_PER_SEC;
        std::cout << "Test " << i + 1 << " passed, time: " << time << " sec" << std::endl;
    }
    
    return 0;
}
