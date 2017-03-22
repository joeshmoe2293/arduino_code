/* Class structure based on code provided at:
http://www.theprojectspot.com/tutorial-post/creating-a-genetic-algorithm-for-beginners/ */

#include "Arduino.h"
using namespace std;

template <typename T, byte arrSize>
class array2
{
  private:
    T arr[arrSize];
  public:
    byte getSize() const {return size;}
    T& operator[](byte index) {return arr[index];}
    T* getArr() const {return &data[0];}
    void setValue(byte index, T value) {arr[index]=value;}
}

template <int populationSize>
class Population
{
  private:
    std::vector<Individual> individuals;
  
  public:
    Population(bool initialize);
    Individual getIndividual(int index);
    Individual getFittest(); // ADD STUFF HERE LATER
    int size();
    void saveIndividual(int index, Individual indiv);
};

class Individual
{ 
  private:
    vector<byte> genes;
    int fitness=0;
    static int defaultGeneLength=64;

  public:
    Individual(){genes.resize(defaultGeneLength);}
    void generateIndividual();
    static void setDefaultGeneLength(int value);
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
  private static vector<byte> solution;

  public:
    static void setSolution(byte newSolution[]);
    //static void setSolution(String newSolution);
    static int getFitness(Individual individual);
    static int getMaxFitness(Population pop);
};

Population::Population(bool initialize)
{
  if (initialize)
  {
    for (int i=0; i<(populationSize); i++)
    {
      Individual newIndividual;
      newIndividual.generateIndividual();
      saveIndividual(i, newIndividual); 
    }
  }
}

Individual Population::getIndividual(int index)
{
  return individuals.at(index);  
}

int Population::size()
{
  return individuals.size();
}

void Population::saveIndividual(int index, Individual indiv)
{
  individuals.insert(index, indiv);
}

void Individual::generateIndividual()
{
  for (int i=0; i<genes.size(); i++)
  {
    byte gene;
    genes.insert(i, gene);  
  }  
}

static void Individual::setDefaultGeneLength(int value)
{
  defaulGeneLength=value;
  genes.resize(defaultGeneLength);
}

byte Individual::getGene(int index)
{
  return genes.at(index);  
}

void Individual::setGene(int index, byte value)
{
  genes.at(index)=value;
}

int Individual::size()
{
  return genes.size();
}

static Individual Algorithm::crossover(Individual indiv1, Individual indiv2)
{
  // ADD STUFF HERE  
}

static void Algorithm::mutate(Individual indiv)
{
  // ADD STUFF HERE  
}

static Individual Algorithm::tournamentSelection(Population pop)
{
  // ADD STUFF HERE  
}

static Population Algorithm::evolvePopulation(Population pop)
{
  // ADD STUFF HERE  
}

static void FitnessCalc::setSolution(byte newSolution[])
{
  solution=vector<byte> newSolutionVector(newSolution, newSolution+sizeof(newSolution)/sizeof(newSolution[0]));
}

static int FitnessCalc::getFitness(Individual indiv)
{
  // ADD STUFF HERE  
}

static int FitnessCalc::getMaxFitness(Population pop)
{
  int maxFitness=0;
  int currentFitness=0;
  for (int i=0; i<solution.size(); i++)
  {
    currentFitness=pop.getIndividual(i).getFitness
    if (currentFitness > maxFitness)
    {
        maxFitness=currentFitness;
    }
  }  
  return maxFitness;
}
