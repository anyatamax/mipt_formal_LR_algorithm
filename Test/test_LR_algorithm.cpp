#include "test_LR_algorithm.h"
#include <sstream>

TEST_F(TestAlgorithm, GrammarTest) {
    Grammar gram_construct;
    EXPECT_EQ(gram_construct.GetAlphabet().size(), 2);
    Grammar gram;
    char left_first = 'S';
    char left_second = 'C';
    std::string right_first = "CC";
    std::string right_second = "dd";
    gram.InsertGrammar(left_first, std::move(right_first));
    gram.InsertGrammar(left_second, std::move(right_second));
    std::map<char, std::vector<std::string>> gram_cur = gram.GetGrammar();
    std::vector<std::set<char>> alphabet_cur = gram.GetAlphabet();
    EXPECT_EQ(gram_cur['S'].size(), 1);
    EXPECT_EQ(gram_cur['C'].size(), 1);
    EXPECT_EQ(gram_cur['S'][0], "CC");
    EXPECT_EQ(gram_cur['C'][0], "dd");
    EXPECT_EQ(alphabet_cur[0].size(), 0);
    EXPECT_EQ(alphabet_cur[1].size(), 0);
}

TEST_F(TestAlgorithm, First) {
    std::map<char, std::vector<std::string>> gram;
    gram['S'].push_back("CC");
    gram['C'].push_back("cC");
    gram['C'].push_back("d");
    std::vector<std::set<char>> alph(2);
    alph[0].insert('c');
    alph[0].insert('d');
    alph[1].insert('S');
    alph[1].insert('C');
    moc_algo.SetAlphabet(alph);
    moc_algo.SetGrammar(gram);
    std::vector<char> next;
    next.push_back('$');
    std::vector<char> next_cur = moc_algo.TestFirst("", '$');
    EXPECT_EQ(next_cur, next);
    next.clear();
    next.push_back('d');
    next_cur = moc_algo.TestFirst("dC", 'c');
    EXPECT_EQ(next_cur, next);
    next.clear();
    next.push_back('c');
    next.push_back('d');
    next_cur = moc_algo.TestFirst("C", '$');
    EXPECT_EQ(next_cur, next);
}

TEST_F(TestAlgorithm, Closure) {
    std::map<char, std::vector<std::string>> gram;
    gram['S'].push_back("CC");
    gram['C'].push_back("cC");
    gram['C'].push_back("d");
    std::vector<std::set<char>> alph(2);
    alph[0].insert('c');
    alph[0].insert('d');
    alph[1].insert('S');
    alph[1].insert('C');
    moc_algo.SetAlphabet(alph);
    moc_algo.SetGrammar(gram);
    std::set<Rule> start_rule;
    start_rule.insert(Rule{'!', "S", '$', 0});
    std::set<Rule> state = moc_algo.TestClosure(start_rule);
    EXPECT_EQ(state.size(), 6);
    EXPECT_TRUE(state.find({'S', "CC", '$', 0}) != state.end());
    EXPECT_TRUE(state.find({'C', "cC", 'c', 0}) != state.end());
    EXPECT_TRUE(state.find({'C', "cC", 'd', 0}) != state.end());
    EXPECT_TRUE(state.find({'C', "d", 'c', 0}) != state.end());
    EXPECT_TRUE(state.find({'C', "d", 'd', 0}) != state.end());
    EXPECT_TRUE(state.find({'C', "cC", 'c', 1}) == state.end());
}

TEST_F(TestAlgorithm, GoTo) {
    std::map<char, std::vector<std::string>> gram;
    gram['S'].push_back("CC");
    gram['C'].push_back("cC");
    gram['C'].push_back("d");
    std::vector<std::set<char>> alph(2);
    alph[0].insert('c');
    alph[0].insert('d');
    alph[1].insert('S');
    alph[1].insert('C');
    moc_algo.SetAlphabet(alph);
    moc_algo.SetGrammar(gram);
    std::set<Rule> state;
    state.insert({'!', "S", '$', 1});
    for (int i = 0; i < 2; ++i) {
        for (auto &symbol : alph[i]) {
            std::set<Rule> state_cur = moc_algo.TestGoTo(state, symbol);
            EXPECT_TRUE(state_cur.empty());
        }
    }
    state.clear();
    state.insert({'S', "CC", '$', 1});
    state.insert({'C', "cC", '$', 0});
    state.insert({'C', "d", '$', 0});
    std::set<Rule> go_to = moc_algo.TestGoTo(state, 'C');
    EXPECT_EQ(go_to.size(), 1);
    EXPECT_TRUE(go_to.find({'S', "CC", '$', 2}) != go_to.end());
}

TEST_F(TestAlgorithm, BuildGrammarStates) {
    std::map<char, std::vector<std::string>> gram;
    gram['S'].push_back("Cc");
    gram['C'].push_back("d");
    std::vector<std::set<char>> alph(2);
    alph[0].insert('c');
    alph[0].insert('d');
    alph[1].insert('S');
    alph[1].insert('C');
    moc_algo.SetAlphabet(alph);
    moc_algo.SetGrammar(gram);
    std::vector<std::set<Rule>> gram_states(5);
    std::set<Rule> cur_rule;
    cur_rule.insert({'!', "S", '$', 0});
    cur_rule.insert({'S', "Cc", '$', 0});
    cur_rule.insert({'C', "d", 'c', 0});
    gram_states[0] = cur_rule;
    cur_rule.clear();
    cur_rule.insert({'!', "S", '$', 1});
    gram_states[3] = cur_rule;
    cur_rule.clear();
    cur_rule.insert({'C', "d", 'c', 1});
    gram_states[1] = cur_rule;
    cur_rule.clear();
    cur_rule.insert({'S', "Cc", '$', 1});
    gram_states[2] = cur_rule;
    cur_rule.clear();
    cur_rule.insert({'S', "Cc", '$', 2});
    gram_states[4] = cur_rule;
    cur_rule.clear();
    Grammar moc_gram;
    moc_gram.SetGrammar(gram, alph);
    EXPECT_EQ(moc_algo.TestGrammarStates(moc_gram), gram_states);
}

TEST_F(TestAlgorithm, TestFitFirst) {
    std::vector<std::set<char>> alph(2);
    alph[0].insert('c');
    alph[0].insert('d');
    alph[1].insert('S');
    alph[1].insert('C');
    std::map<char, std::vector<std::string>> gram;
    gram['S'].push_back("CC");
    gram['C'].push_back("cC");
    gram['C'].push_back("d");
    Grammar grammar;
    grammar.SetGrammar(gram, alph);
    std::vector<std::map<char, TableStatus>> result_table(10);
    for (int i = 0; i < result_table.size(); ++i) {
        for (auto &symbol : alph[0]) {
            result_table[i][symbol] = {"Error"};
        }
        result_table[i]['$'] = {"Error"};
    }
    result_table[0]['c'] = {"Shift", 1};
    result_table[0]['d'] = {"Shift", 2};
    result_table[1]['c'] = {"Shift", 1};
    result_table[1]['d'] = {"Shift", 2};
    result_table[2]['c'] = {"Reduce", 1, 'C'};
    result_table[2]['d'] = {"Reduce", 1, 'C'};
    result_table[3]['c'] = {"Shift", 6};
    result_table[3]['d'] = {"Shift", 7};
    result_table[4]['$'] = {"Accept"};
    result_table[5]['c'] = {"Reduce", 0, 'C'};
    result_table[5]['d'] = {"Reduce", 0, 'C'};
    result_table[6]['c'] = {"Shift", 6};
    result_table[6]['d'] = {"Shift", 7};
    result_table[7]['$'] = {"Reduce", 1, 'C'};
    result_table[8]['$'] = {"Reduce", 0, 'S'};
    result_table[9]['$'] = {"Reduce", 0, 'C'};
    std::vector<std::map<char, int>> go_to(10);
    for (int i = 0; i < result_table.size(); ++i) {
        for (auto &symbol : alph[1]) {
            go_to[i][symbol] = -1;
        }
    }
    go_to[0]['C'] = 3;
    go_to[0]['S'] = 4;
    go_to[1]['C'] = 5;
    go_to[3]['C'] = 8;
    go_to[6]['C'] = 9;
    EXPECT_EQ(moc_algo.TestFit(grammar), result_table);
    EXPECT_EQ(moc_algo.GetGoTo(), go_to);
}

TEST_F(TestAlgorithm, TestFitSecond) {
    std::vector<std::set<char>> alph(2);
    alph[0].insert('a');
    alph[0].insert('b');
    alph[0].insert('.');
    alph[1].insert('S');
    std::map<char, std::vector<std::string>> gram;
    gram['S'].push_back("SaSb");
    gram['S'].push_back(".");
    Grammar grammar;
    grammar.SetGrammar(gram, alph);
    std::vector<std::map<char, TableStatus>> result_table(8);
    for (int i = 0; i < result_table.size(); ++i) {
        for (auto &symbol : alph[0]) {
            result_table[i][symbol] = {"Error"};
        }
        result_table[i]['$'] = {"Error"};
    }
    result_table[0]['a'] = {"Reduce", 1, 'S'};
    result_table[0]['$'] = {"Reduce", 1, 'S'};
    result_table[1]['a'] = {"Shift", 2};
    result_table[1]['$'] = {"Accept"};
    result_table[2]['a'] = {"Reduce", 1, 'S'};
    result_table[2]['b'] = {"Reduce", 1, 'S'};
    result_table[3]['a'] = {"Shift", 4};
    result_table[3]['b'] = {"Shift", 5};
    result_table[4]['a'] = {"Reduce", 1, 'S'};
    result_table[4]['b'] = {"Reduce", 1, 'S'};
    result_table[5]['a'] = {"Reduce", 0, 'S'};
    result_table[5]['$'] = {"Reduce", 0, 'S'};
    result_table[6]['a'] = {"Shift", 4};
    result_table[6]['b'] = {"Shift", 7};
    result_table[7]['a'] = {"Reduce", 0, 'S'};
    result_table[7]['b'] = {"Reduce", 0, 'S'};
    std::vector<std::map<char, int>> go_to(8);
    for (int i = 0; i < result_table.size(); ++i) {
        for (auto &symbol : alph[1]) {
            go_to[i][symbol] = -1;
        }
    }
    go_to[0]['S'] = 1;
    go_to[2]['S'] = 3;
    go_to[4]['S'] = 6;
    EXPECT_EQ(moc_algo.TestFit(grammar), result_table);
    EXPECT_EQ(moc_algo.GetGoTo(), go_to);
}

TEST_F(TestAlgorithm, TestFindGrammar) {
    std::vector<std::set<char>> alph(2);
    alph[0].insert('a');
    alph[0].insert('b');
    alph[0].insert('.');
    alph[1].insert('S');
    std::map<char, std::vector<std::string>> gram;
    gram['S'].push_back("SaSb");
    gram['S'].push_back(".");
    moc_algo.SetGrammar(gram);
    moc_algo.SetAlphabet(alph);
    Rule rule = {'S', "Sa", '$', 0};
    EXPECT_EQ(moc_algo.TestFindGrammar(rule), -1);
}

TEST_F(TestAlgorithm, TestAlgorithmFirst) {
    std::vector<std::set<char>> alph(2);
    alph[0].insert('a');
    alph[0].insert('b');
    alph[0].insert('.');
    alph[1].insert('S');
    std::map<char, std::vector<std::string>> gram;
    gram['S'].push_back("SaSb");
    gram['S'].push_back(".");
    Grammar grammar;
    grammar.SetGrammar(gram, alph);
    moc_algo.Fit(grammar);
    EXPECT_TRUE(moc_algo.TestAlgorithm("ab"));
    EXPECT_TRUE(moc_algo.TestAlgorithm("aabbab"));
    EXPECT_TRUE(moc_algo.TestAlgorithm("aabababb"));
    EXPECT_FALSE(moc_algo.TestAlgorithm("aababab"));
    EXPECT_FALSE(moc_algo.TestAlgorithm("aa"));
    EXPECT_FALSE(moc_algo.TestAlgorithm("aabbb"));
    EXPECT_FALSE(moc_algo.TestAlgorithm("b"));
    EXPECT_FALSE(moc_algo.TestAlgorithm("babab"));
    EXPECT_FALSE(moc_algo.TestAlgorithm("aabbabaaabb"));
    EXPECT_FALSE(moc_algo.TestAlgorithm("aabbabaaabc"));
}

TEST_F(TestAlgorithm, TestPredictFirst) {
    std::map<char, std::vector<std::string>> gram;
    gram['S'].push_back("Cc");
    gram['C'].push_back("d");
    std::vector<std::set<char>> alph(2);
    alph[0].insert('c');
    alph[0].insert('d');
    alph[1].insert('S');
    alph[1].insert('C');
    Grammar grammar;
    grammar.SetGrammar(gram, alph);
    moc_algo.Fit(grammar);
    std::string str_first = "dc";
    std::string str_second = "dd";
    std::ostringstream oss;
    std::streambuf* p_cout_streambuf = std::cout.rdbuf();
    std::cout.rdbuf(oss.rdbuf());
    moc_algo.Predict(str_first);
    std::cout.rdbuf(p_cout_streambuf);
    EXPECT_TRUE(oss && oss.str() == "YES\n");
}

TEST_F(TestAlgorithm, TestPredictSecond) {
    std::map<char, std::vector<std::string>> gram;
    gram['S'].push_back("Cc");
    gram['C'].push_back("d");
    std::vector<std::set<char>> alph(2);
    alph[0].insert('c');
    alph[0].insert('d');
    alph[1].insert('S');
    alph[1].insert('C');
    Grammar grammar;
    grammar.SetGrammar(gram, alph);
    moc_algo.Fit(grammar);
    std::string str_first = "dc";
    std::string str_second = "dd";
    std::ostringstream oss;
    std::streambuf* p_cout_streambuf = std::cout.rdbuf();
    std::cout.rdbuf(oss.rdbuf());
    moc_algo.Predict(str_second);
    std::cout.rdbuf(p_cout_streambuf);
    EXPECT_TRUE(oss && oss.str() == "NO\n");
}

TEST_F(TestAlgorithm, TestStream) {
    Grammar gram;
    std::stringstream str;
    str << "S -> CC\n";
    str >> gram;
    std::map<char, std::vector<std::string>> gram_cur = gram.GetGrammar();
    std::vector<std::set<char>> alph_cur = gram.GetAlphabet();
    std::vector<std::set<char>> alph(2);
    std::map<char, std::vector<std::string>> grammar;
    alph[1].insert('S');
    alph[1].insert('C');
    grammar['S'].push_back("CC");
    EXPECT_EQ(gram_cur, grammar);
    EXPECT_EQ(alph_cur, alph);
    std::stringstream str_fail;
    try {
        str_fail << "c -> Cd\n";
        str_fail >> gram;
        FAIL();
    }
    catch (const LRException &expected) {
        ASSERT_STREQ("Invalid input", expected.what());
    }
}

TEST_F(TestAlgorithm, TestStream1) {
    Grammar gram;
    std::stringstream str_fail;
    try {
        str_fail << "Cd -> Cd\n";
        str_fail >> gram;
        FAIL();
    }
    catch (const LRException &expected) {
        ASSERT_STREQ("Invalid input", expected.what());
    }
}

TEST_F(TestAlgorithm, TestStream2) {
    Grammar gram;
    std::stringstream str_fail;
    try {
        str_fail << "C\n";
        str_fail >> gram;
        FAIL();
    }
    catch (const LRException &expected) {
        ASSERT_STREQ("Invalid input", expected.what());
    }
}

TEST_F(TestAlgorithm, TestStream3) {
    Grammar gram;
    std::stringstream str_fail;
    try {
        str_fail << "C --> D\n";
        str_fail >> gram;
        FAIL();
    }
    catch(const LRException& expected) {
        ASSERT_STREQ("Invalid input", expected.what());
    }
}

TEST_F(TestAlgorithm, TestStream4) {
    Grammar gram;
    std::stringstream str_fail;
    try {
        str_fail << "C ->\n";
        str_fail >> gram;
        FAIL();
    }
    catch(const LRException& expected) {
        ASSERT_STREQ("Invalid input", expected.what());
    }
}

TEST_F(TestAlgorithm, TestStream5) {
    Grammar gram;
    std::stringstream str_fail;
    try {
        str_fail << "C ->  \n";
        str_fail >> gram;
        FAIL();
    }
    catch(const LRException& expected) {
        ASSERT_STREQ("Invalid input", expected.what());
    }
}

TEST_F(TestAlgorithm, TestStream6) {
    Grammar gram;
    std::stringstream str_fail;
    try {
        str_fail << "C ->  D$\n";
        str_fail >> gram;
        FAIL();
    }
    catch(const LRException& expected) {
        ASSERT_STREQ("Invalid input", expected.what());
    }
}