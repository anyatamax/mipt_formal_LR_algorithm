#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <cctype>
#include <algorithm>
#include <stack>
#include <utility>

struct TableStatus {
    std::string action;
    int index;
    char start_symbol = '.';
};

struct Rule {
    char left_path;
    std::string right_path;
    char next;
    int dot_position = 0;
    friend bool operator<(const Rule &lhs, const Rule &rhs);
    friend bool operator==(const Rule &lhs, const Rule &rhs);
};

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

class Grammar {
public:
    Grammar() {
        alphabet_.resize(2);
    }
    void InsertGrammar(char left_rule, std::string &&right_rule) {
        grammar_[left_rule].push_back(right_rule);
    }
    [[nodiscard]] std::map<char, std::vector<std::string>> GetGrammar() const {
        return grammar_;
    }
    [[nodiscard]] std::vector<std::set<char>> GetAlphabet() const {
        return alphabet_;
    }
    friend std::istream& operator>>(std::istream& in, Grammar& grammar);

private:
    std::map<char, std::vector<std::string>> grammar_;
    std::vector<std::set<char>> alphabet_;
};

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


class Algo {
public:
    std::set<Rule> Closure(std::set<Rule> &set) {
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
    std::vector<char> First(std::string beta, char alpha) {
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
    void FindNextLetter(char state, std::vector<char> &terminals_next) {
        // epsilon i need to do
        for (auto &rule : grammar_[state]) {
            if (std::islower(rule[0])) {
                terminals_next.push_back(rule[0]);
            } else {
                FindNextLetter(rule[0], terminals_next);
            }
        }
    }
    std::set<Rule> GoTo(std::set<Rule> &set, char symbol) {
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
    void BuildGrammarSets(Grammar &gram) {
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
    void BuildLRTable(Grammar &gram) {
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
    }
    void SetTable() {
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
    int FindGrammar(const Rule &rule) {
        for (int i = 0 ; i < grammar_[rule.left_path].size(); ++i) {
            if (grammar_[rule.left_path][i] == rule.right_path) {
                return i;
            }
        }
        return -1;
    }
    bool AlgorithmLR(std::string &str) {
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

private:
    std::vector<std::map<char, TableStatus>> LR_table_;
    std::vector<std::map<char, int>> go_to_;
    std::vector<std::set<Rule>> grammar_states_;
    std::map<char, std::vector<std::string>> grammar_;
    std::vector<std::set<char>> alphabet_;
};


int main() {
    Grammar gram;
    int n;
    std::cin >> n;
    std::string str;
    std::cin >> str;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    for (int i = 0; i < n; ++i) {
        std::cin >> gram;
    }
    Algo a;
    a.BuildLRTable(gram);
    if (a.AlgorithmLR(str)) {
        std::cout << 1;
    } else {
        std::cout << 0;
    }
    return 0;
}