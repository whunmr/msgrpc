#ifndef PROJECT_TEST_RUNNER_H
#define PROJECT_TEST_RUNNER_H

#define COMBINE1(X,Y) X##Y  // helper macro
#define COMBINE(X,Y) COMBINE1(X,Y)

////////////////////////////////////////////////////////////////////////////////
extern void run_next_testcase();

#define DEF_TESTCASE(testcase_name_)          \
struct testcase_name_ : TestCaseBase {        \
    void run() const override;                \
}COMBINE(testcase_intance_at_line_,__LINE__); \
void testcase_name_::run() const


////////////////////////////////////////////////////////////////////////////////
struct TestCaseBase {
    TestCaseBase();
    virtual void run() const = 0;
};

#endif //PROJECT_TEST_RUNNER_H
