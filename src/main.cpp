#include "Utils.h"
#include "GeneticAlgorithm.h"
#include <iostream>
#include <chrono>




/// implement fitness function. Default fitness is set to Wallet::successHeuristic
float fitness1(Wallet* w)
{
    return w->successProbability();

}
float fitness2(Wallet *w)
{
    return w->successHeuristic();
}

/// implement perturb function. Default is Wallet::perturb
void perturb(Wallet* w, float perturbProbability)
{
    w->perturb(perturbProbability);
}

Wallet bestSolution(vector<Wallet>& solutions)
{
    float bestScore = 0;
    Wallet bestWallet;
    for(auto & solution : solutions)
    {
        float score = solution.successProbability();
        if(score > bestScore)
        {
            bestScore = score;
            bestWallet = solution;
        }
    }
    return bestWallet;
}

void run(int keys) {

    vector<Wallet> topWallets;
    /// Enter desired safety, loss, leak and theft probabilities here
    vector<float> probabilities = {0.7, 0.1, 0.2, 0.0};
    /// Enter desired weight for heuristic (Light heuristic 0.0 - Heavy heuristic 1.0)
    float weight = 0.001;
    /// Enter true for heuristic success or false for success probability
    float heuristic = false;
    /// Enter number of random restarts
    int randomRestarts = 1;

    context = new SystemParameters(keys, probabilities, weight);
    auto start = chrono::high_resolution_clock::now();
    for(int i=0; i < randomRestarts; i++)
    {
        ///*    Set genetic algorithm parameters here (population size, selection size, iterations til halt,
        ///     perturb probability) *///
        GeneticAlgorithm<Wallet> algorithm(50, 2, 10,0.3,
                                           (&perturb), (heuristic ? fitness2: fitness1));
        algorithm.run();
        topWallets.push_back(algorithm.getSolution());
    }
    auto duration = chrono::high_resolution_clock::now() - start;
    float score = bestSolution(topWallets).successProbability();
    delete context;
    cout << "Number of keys: " << keys << ", Duration: " << chrono::duration_cast<chrono::seconds>(duration).count() << ", Score: " << score << endl;
}

int main()
{
    for(int keys = 2; keys < 10; keys++)
        run(keys);
    return 0;
}