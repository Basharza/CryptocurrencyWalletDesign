//
// Created by Bashar Zahalka on 11/04/2023.
//

#ifndef GENETICALGO_GENETICALGORITHM_H
#define GENETICALGO_GENETICALGORITHM_H

#include <vector>
#include <random>

using namespace std;

/**
 * @brief Represents a genetic algorithm.
 *
 * @tparam Chromosome The type of chromosome used in the genetic algorithm.
 */
template <class Chromosome>
class GeneticAlgorithm
{
    int populationSize;
    int selectionSize;
    int iterationsToHalt; /**< The number of iterations with no improvement to halt the genetic algorithm. */
    float perturbProbability; /**< The probability of perturbing a chromosome in each iteration. */
    float (*fitness)(Chromosome*); /**< The function pointer to calculate the fitness of a chromosome. */
    void (*perturb)(Chromosome*, float); /**< The function pointer to perturb a chromosome. */
    vector<Chromosome*> population;
    Chromosome bestChromosome;

    /**
     * @brief Retrieves the fittest chromosome from a group of chromosomes.
     *
     * @param group The group of chromosomes.
     * @return The fittest chromosome.
     */
    Chromosome* getFittest(const vector<Chromosome*>& group);

    /**
     * @brief Selects chromosomes uniformly at random (UAR) from the population.
     *
     * @param size The number of chromosomes to select.
     * @return The selected chromosomes.
     */
    vector<Chromosome*> selectUAR(int size);

public:
    /**
     * @brief Constructs a new GeneticAlgorithm object.
     *
     * @param populationSize The size of the population in the genetic algorithm.
     * @param selectionSize The size of the selection in each iteration.
     * @param iterationsToHalt The number of iterations to halt the genetic algorithm.
     * @param perturbProbability The probability of perturbing a chromosome in each iteration.
     * @param perturb The function pointer to perturb a chromosome.
     * @param fitness The function pointer to calculate the fitness of a chromosome.
     */
    GeneticAlgorithm(int populationSize, int selectionSize, int iterationsToHalt, float perturbProbability,
                     void (*perturb)(Chromosome*, float), float (*fitness)(Chromosome*));

    /**
     * @brief Runs the genetic algorithm.
     */
    void run();

    /**
     * @brief Retrieves the best chromosome found during the genetic algorithm.
     *
     * @return The best chromosome.
     */
    Chromosome getSolution();

    /**
     * @brief Destroys the GeneticAlgorithm object.
     */
    ~GeneticAlgorithm();
};



///     GeneticAlgorithm implementation

template<class Chromosome>
GeneticAlgorithm<Chromosome>::GeneticAlgorithm(int populationSize, int selectionSize, int iterationsToHalt,
        float perturbProbability, void (*perturb)(Chromosome*, float), float (*fitness)(Chromosome*)):
        populationSize(populationSize), selectionSize(selectionSize), iterationsToHalt(iterationsToHalt),
        perturbProbability(perturbProbability), perturb(perturb), fitness(fitness),
        population(vector<Chromosome*>(populationSize, nullptr))
{
    for(int i=0; i < populationSize; i++)
        population[i] = new(Chromosome);
}

template<class Chromosome>
void GeneticAlgorithm<Chromosome>::run()
{
    int delta = 0;
    float best_score = 0;
    while(delta < iterationsToHalt)
    {

        Chromosome* fittest = getFittest(population);
        float score = fittest? fitness(fittest) : 0;
        if(score > best_score)
        {
            bestChromosome = Chromosome(*fittest);
            best_score = score;
            delta = 0;
        }
        else
            delta++;


        vector<Chromosome*> selected(populationSize, nullptr);
        selected[0] = new Chromosome(*getFittest(population));
        for(int i=1; i < populationSize; i++)
        {
            auto sel = selectUAR(selectionSize);
            auto *fit = getFittest(sel);
            if(fit)
            {
                selected[i] = new Chromosome(*fit);
                perturb(selected[i], perturbProbability);
            }
        }

        for(int i=0; i < populationSize; i++)
        {
            delete population[i];
            population[i] = selected[i];
        }
    }
}

template<class Chromosome>
Chromosome GeneticAlgorithm<Chromosome>::getSolution()
{
    return bestChromosome;
}

template<class Chromosome>
Chromosome* GeneticAlgorithm<Chromosome>::getFittest(const vector<Chromosome*>& group)
{
    float bestScore = 0;
    Chromosome* fittestChromosome = nullptr;
    for(Chromosome* w: group)
    {
        float f = w? fitness(w): 0;
        if(f > bestScore)
        {
            fittestChromosome = w;
            bestScore = f;
        }
    }
    return fittestChromosome;
}

template<class Chromosome>
vector<Chromosome*> GeneticAlgorithm<Chromosome>::selectUAR(int size)
{
    vector<int> nums(populationSize);
    iota(nums.begin(), nums.end(), 0);
    random_device rd;
    mt19937 gen(rd());
    shuffle(nums.begin(), nums.end(), gen);
    vector<int> result(size);
    copy(nums.begin(), nums.begin()+size, result.begin());
    vector<Chromosome*> chromosomes;
    for(auto i: result)
        chromosomes.push_back(population[i]);
    return chromosomes;
}

template<class Chromosome>
GeneticAlgorithm<Chromosome>::~GeneticAlgorithm()
{
    for(int i=0; i<populationSize; i++)
        delete population[i];
}

#endif //GENETICALGO_GENETICALGORITHM_H
