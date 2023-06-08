//
// Created by Bashar Zahalka on 28/05/2023.
//

#include "Utils.h"



SystemParameters* context;

///     SystemParameters implementation

SystemParameters::SystemParameters(int numberOfKeys, const vector<float>& probabilities, float weight):
numberOfKeys(numberOfKeys), probabilities(probabilities), mersenneTwisterEngine(randomDevice()),
scenarioSpaceSize(pow(4, numberOfKeys))
{
    randomScenarioSampleSize = 1000 * (numberOfKeys > 4? float(numberOfKeys)/2: 1);
}

int SystemParameters::uniformDistribution(int a, int b)
{
    uniform.param(uniform_int_distribution<>::param_type(a,b));
    return uniform(mersenneTwisterEngine);
}

bool SystemParameters::bernoulliDistribution(float probability)
{
    bernoulli.param(bernoulli_distribution::param_type(probability));
    return bernoulli(mersenneTwisterEngine);
}



///     Scenario implementation

Scenario::Scenario(): keyStates(vector<KeyState>(context->numberOfKeys, SAFE)){}

bool Scenario::nextScenario()
{
    int carry = 1;
    for(int i = keyStates.size() - 1; i >=0 && carry; i--)
    {
        int sum = keyStates[i] + carry;
        keyStates[i] = KeyState(sum % 4);
        carry = sum/4;
    }
    return carry == 0;
}

void Scenario::randomize()
{
    int stateNumber = context->uniformDistribution(0, context->scenarioSpaceSize - 1);
    for(int i = keyStates.size() - 1; stateNumber > 0; i--, stateNumber/=4)
        keyStates[i] = KeyState(stateNumber % 4);
}

float Scenario::probability()
{
    float prob = 1;
    for(const auto& i: keyStates)
        prob *= context->probabilities[i];
    return prob;
}

vector<vector<bool>> Scenario::availability() const {
    vector<vector<bool>> availability(2, vector<bool>(keyStates.size()));
    for(int i=0; i<keyStates.size(); i++)
    {
        availability[OWNER][i] = !((keyStates[i] == LOSS) || (keyStates[i] == THEFT));
        availability[ADVERSARY][i] = ((keyStates[i] == LEAK) || (keyStates[i] == THEFT));
    }
    return availability;
}



///     Wallet implementation

Wallet::Wallet()
{
    int minWalletSize = 1;
    int maxWalletSize = (context->numberOfKeys * 3)/2;

    int walletSize = context->uniformDistribution(minWalletSize, maxWalletSize);
    dnf = vector<vector<bool>>(walletSize, vector<bool>(context->numberOfKeys));
    for (auto& inner : dnf) {
        for (auto && i : inner) {
            i = (context->bernoulliDistribution(0.5));
        }
    }
}

Wallet::Wallet(const Wallet& wallet)
{
    dnf = wallet.dnf;
}

bool Wallet::satisfiesWallet(const vector<bool>& availability)
{
    for (const auto& combination : dnf)
    {
        bool satisfied = true;
        for(int key = 0; key < combination.size(); key++)
        {
            if(combination[key] && !availability[key])
            {
                satisfied = false;
                break;
            }
        }
        if(satisfied)
            return true;
    }
    return false;
}

bool Wallet::walletSuccess(const Scenario& scenario)
{
    const auto& availability = scenario.availability();
    return satisfiesWallet(availability[OWNER]) && (!satisfiesWallet(availability[ADVERSARY]));
}

void Wallet::perturb(float perturb)
{
    int randNum = context->uniformDistribution(0, dnf.size() - 1);
    vector<bool>& combination = dnf[randNum];
    for(auto && i : combination)
    {
        if(context->bernoulliDistribution(perturb))
            i = 1 - i;
    }
}



float Wallet::successProbability()
{
    float score = 0;
    Scenario scenario;
    do {
        if(walletSuccess(scenario))
            score += scenario.probability();
    } while(scenario.nextScenario());
    return score;
}

float Wallet::successHeuristic()
{
    float score = 0;
    Scenario scenario;
    for(int i=0; i<context->randomScenarioSampleSize; i++)
    {
        scenario.randomize();
        if(walletSuccess(scenario))
            score += scenario.probability();
    }
    return score;
}