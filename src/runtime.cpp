#include <runtime.hpp>

namespace sable
{

TestResult compare_runtime(
    const stats::SingleVarStats& rt1,
    const stats::SingleVarStats& rt2,
    const float alpha
)
{
    // run t test with two runtimes

    stats::TDistribution tdist_diff(
        rt1.mean, rt1.variance, rt1.n,
        rt2.mean, rt2.variance, rt2.n
    );

    double stat = tdist_diff.statistic(0.0);

    double prob = tdist_diff.cdf(stat);

    unsigned int result = HypothesisTest::Null;

    if ((1 - prob) < alpha / 2 || prob < alpha / 2) result |= HypothesisTest::AlternateNEq;
    if (prob < alpha)       result |= HypothesisTest::AlternateLt;
    if ((1 - prob) < alpha) result |= HypothesisTest::AlternateGt;

    return TestResult{
        rt1, rt2, // runtimes
        prob, // probability
        stat, // test statistic
        static_cast<HypothesisTest>(result) // hypotheses test result
    };
}



TestResult compare_runtime(
    void (*func1)(), void (*func2)(), 
    const float alpha, const size_t n
)
{
    core::Runner run1(func1), run2(func2);

    run1.run(n);
    run2.run(n);

    return compare_runtime(run1.runtime(), run2.runtime(), alpha);
}

TestResult compare_runtime_multithreaded(
    void (*func1)(), void (*func2)(),
    const float alpha, const size_t n,
    const size_t threads
)
{
    core::Runner run1(func1), run2(func2);
    run1.run(n, true);
    run2.run(n, true);

    return compare_runtime(run1.runtime(), run2.runtime(), alpha);
}

void output_test_result(const TestResult& result)
{
    std::cout << "Test results: \n";

    std::cout << 
        "Runtime 1: mean: " << result.runtime1.mean << 
        "ns, variance: " << result.runtime1.variance << 
        "ns, n: " << result.runtime1.n << "\n";
    std::cout << 
        "Runtime 2: mean: " << result.runtime2.mean << 
        "ns, variance: " << result.runtime2.variance << 
        "ns, n: " << result.runtime2.n << "\n";

    std::cout << "T-Test statistic: " << result.test_statistic << "\n";
    std::cout << "Probability: " << result.prob << "\n";

    auto test_results = static_cast<unsigned int>(result.hypotheses);

    if (test_results == 0)
    {
        std::cout << "Failed to reject null hypothesis\n";
    }

    if ((test_results & 0b001) == 0b001)
    {
        std::cout << "Alternate hypothesis not equal - pass\n";
    }

    if ((test_results & 0b010) == 0b010)
    {
        std::cout << "Alternate hypothesis greater than - pass\n";
    }

    if ((test_results & 0b100) == 0b100)
    {
        std::cout << "Alternate hypothesis less than - pass\n";
    }

}

}