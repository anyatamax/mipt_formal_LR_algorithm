#include "../include/LR_algorithm.h"

bool operator<(const Rule &lhs, const Rule &rhs) {
    if (lhs.left_path == rhs.left_path) {
        if (rhs.right_path == lhs.right_path) {
            if (lhs.next == rhs.next) {
                return lhs.dot_position < rhs.dot_position;
            }
            return lhs.next < rhs.next;
        }
        return rhs.right_path < lhs.right_path;
    }
    return lhs.left_path < rhs.left_path;
}

bool operator==(const Rule &lhs, const Rule &rhs) {
    return (lhs.left_path == rhs.left_path &&
            lhs.right_path == rhs.right_path &&
            lhs.next == rhs.next &&
            lhs.dot_position == rhs.dot_position);
}

void Grammar::InsertGrammar(char left_rule, std::string &&right_rule) {
    grammar_[left_rule].push_back(right_rule);
}

std::vector<std::set<char>> Grammar::GetAlphabet() const {
    return alphabet_;
}

std::map<char, std::vector<std::string>> Grammar::GetGrammar() const {
    return grammar_;
}

std::istream& operator>>(std::istream& in, Grammar& grammar) {
    std::string rule;
    std::getline(in, rule);
    int index = 0;
    while (rule[index] != '-' && rule[index + 1] != '>') {
        if (std::isalpha(rule[index])) {
            if (std::islower(rule[index])) {
                grammar.alphabet_[0].insert(rule[index]);
            }
            if (std::isupper(rule[index])) {
                grammar.alphabet_[1].insert(rule[index]);
            }
        }
        ++index;
    }
    index += 2;
    while (rule[index] == ' ') {
        ++index;
    }
    grammar.InsertGrammar(rule[0], rule.substr(index, rule.size()));
    for (int i = index; i < rule.size(); ++i) {
        if (std::isalpha(rule[i])) {
            if (std::islower(rule[i])) {
                grammar.alphabet_[0].insert(rule[i]);
            }
            if (std::isupper(rule[i])) {
                grammar.alphabet_[1].insert(rule[i]);
            }
        }
    }
    return in;
}

void Algo::Fit(Grammar &gram) {
    BuildGrammarSets(gram);
    SetTable();
    for (int i = 0; i < LR_table_.size(); ++i) {
        for (auto &rule : grammar_states_[i]) {
            if (rule.dot_position == rule.right_path.size() && rule.left_path != '!') {
                LR_table_[i][rule.next] = {"Reduce", FindGrammar(rule), rule.left_path};
            } else if (rule.dot_position == rule.right_path.size() && rule.left_path == '!') {
                LR_table_[i]['$'] = {"Accept"};
            } else {
                char symbol = rule.right_path[rule.dot_position];
                if (std::islower(symbol)) {
                    std::set<Rule> move_to = GoTo(grammar_states_[i], symbol);
                    int index = -1;
                    for (int j = 0; j < grammar_states_.size(); ++j) {
                        if (move_to == grammar_states_[j]) {
                            index = j;
                            break;
                        }
                    }
                    if (index != -1) {
                        LR_table_[i][symbol] = {"Shift", index};
                    }
                }
                if (std::isupper(symbol)) {
                    std::set<Rule> move_to = GoTo(grammar_states_[i], symbol);
                    int index = -1;
                    for (int j = 0; j < grammar_states_.size(); ++j) {
                        if (move_to == grammar_states_[j]) {
                            index = j;
                            break;
                        }
                    }
                    if (index != -1) {
                        go_to_[i][symbol] = index;
                    }
                }
            }
        }
    }
//    for (int i = 0; i < LR_table_.size(); ++i) {
//        for (auto &sym : alphabet_[0]) {
//            std::cout << sym << ":" << LR_table_[i][sym].action << LR_table_[i][sym].start_symbol << LR_table_[i][sym].index << " ";
//        }
//        char sym = '$';
//        std::cout << sym << ":" << LR_table_[i][sym].action << LR_table_[i][sym].start_symbol << LR_table_[i][sym].index << "\n";
//    }
//    for (int i = 0; i < go_to_.size(); ++i) {
//        for (auto &sym : alphabet_[1]) {
//            std::cout << sym << go_to_[i][sym] << " ";
//        }
//        std::cout << "\n";
//    }
}
bool Algo::Predict(std::string &str_find) {
    std::string str = str_find;
    str += '$';
    int index = 0;
    std::stack<int> stack_algo;
    stack_algo.push(0);
    while (index < str.size()) {
        int cur_state = stack_algo.top();
        if (alphabet_[0].find(str[index]) == alphabet_[0].end()) {
            if (str[index] != '$') {
                return false;
            }
        }
        if (LR_table_[cur_state][str[index]].action == "Shift") {
            stack_algo.push(LR_table_[cur_state][str[index]].index);
            ++index;
        } else if (LR_table_[cur_state][str[index]].action == "Reduce") {
            std::string cur_rule_right = grammar_[LR_table_[cur_state][str[index]].start_symbol][LR_table_[cur_state][str[index]].index];
            if (cur_rule_right == ".") {
                cur_rule_right = "";
            }
            for (int i = 0; i < cur_rule_right.size(); ++i) {
                stack_algo.pop();
            }
            int s = stack_algo.top();
            if (go_to_[s][LR_table_[cur_state][str[index]].start_symbol] == -1) {
                return false;
            }
            stack_algo.push(go_to_[s][LR_table_[cur_state][str[index]].start_symbol]);
        } else if (LR_table_[cur_state][str[index]].action == "Accept" && index == str.size() - 1) {
            return true;
        } else if (LR_table_[cur_state][str[index]].action == "Error") {
            return false;
        }
    }
    return false;
}

void Algo::SetTable() {
    LR_table_.resize(grammar_states_.size());
    go_to_.resize(grammar_states_.size());
    for (int i = 0; i < LR_table_.size(); ++i) {
        for (auto &symbol : alphabet_[0]) {
            LR_table_[i][symbol] = {"Error"};
        }
        LR_table_[i]['$'] = {"Error"};
        for (auto &symbol : alphabet_[1]) {
            go_to_[i][symbol] = -1;
        }
    }
}
int Algo::FindGrammar(const Rule &rule) {
    for (int i = 0 ; i < grammar_[rule.left_path].size(); ++i) {
        if (grammar_[rule.left_path][i] == rule.right_path || (grammar_[rule.left_path][i] == "." && rule.right_path.empty())) {
            return i;
        }
    }
    return -1;
}
std::set<Rule> Algo::Closure(std::set<Rule> &set) {
    bool changed;
    std::set<Rule> cur_set(set);
    do {
        changed = false;
        for (auto &situation : cur_set) {
            int pos = situation.dot_position;
            if (pos == situation.right_path.size()) {
                continue;
            }
            char start_state = situation.right_path[pos];
            if (std::isupper(start_state)) {
                std::vector<char> next_situation;
                std::string beta;
                char alpha = situation.next;
                if (pos + 1 < situation.right_path.size()) {
                    beta = situation.right_path.substr(pos + 1, situation.right_path.size());
                }
                next_situation = First(beta, alpha);
                for (auto &rule : grammar_[start_state]) {
                    for (auto &next : next_situation) {
                        Rule new_rule = {start_state, rule};
                        if (rule == ".") {
                            new_rule.right_path.clear();
                        }
                        new_rule.dot_position = 0;
                        new_rule.next = next;
                        if (cur_set.find(new_rule) == cur_set.end()) {
                            cur_set.insert(new_rule);
                            changed = true;
                        }
                    }
                }
            }
        }
    } while (changed);
    return cur_set;
}
std::vector<char> Algo::First(std::string beta, char alpha) {
    std::vector<char> first_terminals;
    if (beta.empty()) {
        first_terminals.push_back(alpha);
        return first_terminals;
    }
    if (std::isupper(beta[0])) {
        FindNextLetter(beta[0], first_terminals);
        return first_terminals;
    }
    first_terminals.push_back(beta[0]);
    return first_terminals;
}
void Algo::FindNextLetter(char state, std::vector<char> &terminals_next) {
    // epsilon i need to do
    for (auto &rule : grammar_[state]) {
        if (rule[0] == '.') {
            continue;
        }
        if (std::islower(rule[0])) {
            terminals_next.push_back(rule[0]);
        } else {
            FindNextLetter(rule[0], terminals_next);
        }
    }
}
std::set<Rule> Algo::GoTo(std::set<Rule> &set, char symbol) {
    std::set<Rule> cur_set;
    for (auto &situations : set) {
        if (situations.dot_position == situations.right_path.size()) {
            continue;
        }
        if (situations.right_path[situations.dot_position] == symbol) {
            Rule new_rule = {situations.left_path, situations.right_path, situations.next, situations.dot_position + 1};
            cur_set.insert(new_rule);
        }
    }
    return Closure(cur_set);
}
void Algo::BuildGrammarSets(Grammar &gram) {
    grammar_ = gram.GetGrammar();
    alphabet_ = gram.GetAlphabet();
    std::set<Rule> start_rule;
    start_rule.insert(Rule{'!', "S", '$', 0});
    grammar_states_.push_back(Closure(start_rule));
    bool changed;
    do {
        changed = false;
        std::vector<std::set<Rule>> cur_grammar_states;
        for (auto &cur_set : grammar_states_) {
            for (int i = 0; i < 2; ++i) {
                for (auto &symbol_grammar : alphabet_[i]) {
                    std::set<Rule> next_set = GoTo(cur_set, symbol_grammar);
                    if (!next_set.empty()) {
                        if (std::find(grammar_states_.begin(), grammar_states_.end(), next_set) ==
                            grammar_states_.end()) {
                            cur_grammar_states.push_back(next_set);
                            changed = true;
                        }
                    }
                }
            }
        }
        for (auto &new_states : cur_grammar_states) {
            grammar_states_.push_back(new_states);
        }
    } while (changed);
}