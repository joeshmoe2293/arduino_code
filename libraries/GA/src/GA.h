#ifndef GA_h
#define GA_h

#include "Arduino.h"

/* Class structure based on code provided at:
http://www.theprojectspot.com/tutorial-post/creating-a-genetic-algorithm-for-beginners/ */

/* I don't Think this is needed...
class GA
{
  // Add stuff here later   
}; */

template <int populationSize>
class Population
{
  private:
    Individual individuals[populationSize];
  
  public:
	Population(bool initialize);
    Individual getIndividual(int index);
    Individual getFittest();
    int size();
    void saveIndividual(int index, Individual indiv);
};

class Individual
{ 
  private:
    byte genes[];
    int fitness=0;
    static int defaultGeneLength=64;

  public:
    void generateIndividual();
    static void setDefaultGeneLength();
    byte getGene(int index);
    void setGene(int index, byte value);
    int size();
};

class Algorithm
{
  private:
	static final double uniformRate=0.5;
    static final double mutationRate=0.015;
    static final int tournamentSize=5;
    static final boolean elitism=true;

    static Individual crossover(Individual indiv1, Individual indiv2);
    static void mutate(Individual indiv);
    static Individual tournamentSelection(Population pop);

  public:
    static Population evolvePopulation(Population pop);
};

class FitnessCalc
{
  private static byte solution[];

  public:
    static void setSolution(byte newSolution[]);
    static void setSolution(String newSolution);
    static int getFitness(Individual individual);
    static int getMaxFitness();
};

#endif
