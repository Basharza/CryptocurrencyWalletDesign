//
// Created by Bashar Zahalka on 11/04/2023.
//

#ifndef GENETICALGO_UTILS_H
#define GENETICALGO_UTILS_H

#include <vector>
#include <random>

using namespace std;

typedef enum {SAFE = 0, LOSS = 1, LEAK = 2, THEFT = 3} KeyState;
typedef enum {OWNER = 0, ADVERSARY = 1} Role;

/**
 * @brief Represents the system parameters shared by the different classes in Utils.
 */
struct SystemParameters
{
    int numberOfKeys;
    double randomScenarioSampleSize; /**< Size of random sample used to calculate success heuristic. */
    double scenarioSpaceSize;
    vector<float> probabilities; /**< Safety, loss, leakage and theft probabilities associated with the keys. */
    random_device randomDevice;
    mt19937 mersenneTwisterEngine;
    uniform_int_distribution<> uniform;
    bernoulli_distribution bernoulli;

    /**
     * @brief Constructs a new SystemParameters object.
     *
     * @param numberOfKeys The number of keys in the system.
     * @param probabilities The probabilities associated with the system.
     * @param weight The weight of the success heuristic
     */
    SystemParameters(int numberOfKeys, const vector<float>& probabilities, float weight);

    /**
     * @brief Generates a random number from a uniform distribution between a and b.
     *
     * @param a The lower bound of the distribution.
     * @param b The upper bound of the distribution.
     * @return The generated random number.
     */
    int uniformDistribution(int a, int b);

    /**
     * @brief Generates a random boolean value based on a Bernoulli distribution with the given probability.
     *
     * @param probability The probability of success (true).
     * @return True if the random value is a success (true), false otherwise.
     */
    bool bernoulliDistribution(float probability);
};

extern SystemParameters* context; /**< Pointer to the shared system parameters object. */

/**
 * @brief Represents a scenario in the system.
 */
class Scenario
{
    vector<KeyState> keyStates; /**< The states of the keys in the scenario. */
public:
    /**
     * @brief Constructs a new Scenario object.
     */
    explicit Scenario();

    /**
     * @brief Generates the next scenario.
     *
     * @return True if a new scenario was generated, false if no more scenarios are available.
     */
    bool nextScenario();

    /**
     * @brief Randomizes the states of the keys in the scenario.
     */
    void randomize();

    /**
     * @brief Calculates the probability of the current scenario.
     *
     * @return The probability of the current scenario.
     */
    float probability();

    /**
     * @brief Retrieves the availability vectors of the owner and the adversary in the current scenario.
     *
     * @return The availability matrix representing the availability of keys in the current scenario.
     */
    [[nodiscard]] vector<vector<bool>> availability() const;
};

/**
 * @brief Represents a wallet in the system.
 */
class Wallet
{
    vector<vector<bool>> dnf; /**< The disjunctive normal form (DNF) representing the wallet predicate. */

    /**
     * @brief Checks if the given availability vector (of either the owner or the adversary) satisfies the wallet.
     *
     * @param availability The availability of keys.
     * @return True if the wallet satisfies the availability, false otherwise.
     */
    bool satisfiesWallet(const vector<bool>& availability);

    /**
     * @brief Checks if the wallet succeeds in the given scenario.
     *
     * @param scenario The scenario to evaluate.
     * @return True if the wallet succeeds in the scenario, false otherwise.
     */
    bool walletSuccess(const Scenario& scenario);
public:
    /**
     * @brief Constructs a new Wallet object.
     */
    explicit Wallet();

    /**
     * @brief Constructs a new Wallet object from an existing wallet.
     *
     * @param wallet The wallet to copy.
     */
    Wallet(const Wallet& wallet);

    /**
     * @brief Perturbs the wallet with the given perturbation factor.
     *
     * @param perturb The perturbation factor.
     */
    void perturb(float perturb);

    /**
     * @brief Calculates the success probability of the wallet.
     *
     * @return The success probability of the wallet.
     */
    float successProbability();

    /**
     * @brief Calculates the success heuristic on a random scenario sample instead of the entire scenario space.
     *
     * @return The success heuristic of the wallet.
     */
    float successHeuristic();
};







#endif //GENETICALGO_UTILS_H
