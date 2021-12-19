#pragma once

#include "LR_algorithm.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

class MockAlgo : public Algo {
public:
    MockAlgo() : Algo() {};
    void SetGrammar(std::map<char, std::vector<std::string>> &gram) {
        grammar_ = gram;
    }
    void SetAlphabet(std::vector<std::set<char>> &alph) {
        alphabet_ = alph;
    }
    std::set<Rule> TestClosure(std::set<Rule> &set) {
        return Closure(set);
    }
    std::vector<char> TestFirst(std::string beta, char alpha) {
        return First(beta, alpha);
    }
    std::set<Rule> TestGoTo(std::set<Rule> &set, char symbol) {
        return GoTo(set, symbol);
    }
    std::vector<std::set<Rule>> TestGrammarStates(Grammar &gram) {
        BuildGrammarSets(gram);
        return grammar_states_;
    }
    std::vector<std::map<char, TableStatus>> TestFit(Grammar &gram) {
        Fit(gram);
        return LR_table_;
    }
    std::vector<std::map<char, int>> GetGoTo() {
        return go_to_;
    }
    bool TestAlgorithm(std::string str) {
        return Predict(str);
    }
    int TestFindGrammar(const Rule &rule) {
        return FindGrammar(rule);
    }

private:

};

class TestAlgorithm : public ::testing::Test {
public:
    MockAlgo moc_algo;
private:

};

