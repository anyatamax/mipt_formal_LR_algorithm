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

class Grammar {
public:
    Grammar() {
        alphabet_.resize(2);
    }
    void InsertGrammar(char left_rule, std::string &&right_rule);
    [[nodiscard]] std::map<char, std::vector<std::string>> GetGrammar() const;
    [[nodiscard]] std::vector<std::set<char>> GetAlphabet() const;
    friend std::istream& operator>>(std::istream& in, Grammar& grammar);

private:
    std::map<char, std::vector<std::string>> grammar_;
    std::vector<std::set<char>> alphabet_;
};

class Algo {
public:
    void Fit(Grammar &gram);
    bool Predict(std::string &str_find);

private:
    std::vector<std::map<char, TableStatus>> LR_table_;
    std::vector<std::map<char, int>> go_to_;
    std::vector<std::set<Rule>> grammar_states_;
    std::map<char, std::vector<std::string>> grammar_;
    std::vector<std::set<char>> alphabet_;
    void SetTable();
    int FindGrammar(const Rule &rule);
    std::set<Rule> Closure(std::set<Rule> &set);
    std::vector<char> First(std::string beta, char alpha);
    void FindNextLetter(char state, std::vector<char> &terminals_next);
    std::set<Rule> GoTo(std::set<Rule> &set, char symbol);
    void BuildGrammarSets(Grammar &gram);
};
