/* Class structure based on code provided at:
http://www.theprojectspot.com/tutorial-post/creating-a-genetic-algorithm-for-beginners/ */

#include "Arduino.h"
using namespace std;

namespace arr
{
  template <typename T, byte arrSize>
  class array1
  {
    private:
      T arr[arrSize];
      byte _size=arrSize;
    public:
      byte getSize() const {return _size;}
      T& operator[](byte &index) {return arr[index];}
      T* getArr() const {return &arr[0];}
  };
}

class Individual
{ 
  private:
    int fitness=0;
    static constexpr int defaultGeneLength=64;
    static constexpr int const &dgl=defaultGeneLength;
    arr::array1<byte, dgl> genes;
    static byte generateRandomGene();

  public:
    void generateIndividual();
    static void setDefaultGeneLength(int &value);
    byte getGene(int &index);
    void setGene(int &index, byte &value);
    int getSize();
};

template <int populationSize>
class Population
{
  private:
    arr::array1<Individual, populationSize> individuals;
  public:
    Population(bool &initialize);
    Individual getIndividual(int &index);
    Individual getFittest(); // ADD STUFF HERE LATER
    int getSize();
    void saveIndividual(int &index, Individual &indiv);
};

class Algorithm
{
  private:
    static const double uniformRate=0.5, mutationRate=0.015;
    static const boolean elitism=true;
    static const int tournamentSize=5;

    static Individual crossover(Individual &indiv1, Individual &indiv2);
    static void mutate(Individual &indiv);
    static Individual tournamentSelection(Population &pop);

  public:
    static Population evolvePopulation(Population &pop);
};

class FitnessCalc
{
  private static arr::array<byte, 64> solution;

  public:
    static void setSolution(arr::array<byte, solution.getSize()> &newSolution);
    static int getFitness(Individual &individual);
    static int getMaxFitness(Population &pop);
};

Population::Population(bool &initialize)
{
  if (initialize)
  {
    for (int i=0; i<(individuals.getSize()); i++)
    {
      Individual newIndividual;
      newIndividual.generateIndividual();
      saveIndividual(i, newIndividual); 
    }
  }
}

Individual Population::getIndividual(int &index)
{
  return individuals[index];  
}

int Population::getSize()
{
  return individuals.getSize();
}

void Population::saveIndividual(int &index, Individual &indiv)
{
  individuals[index]=indiv;
}

void Individual::generateIndividual()
{
  for (int i=0; i<genes.getSize(); i++)
  {
    genes[i]=Individual.generateRandomGene();
  }  
}

static byte Individual::generateRandomGene()
{
	randomSeed(random(1000));
	byte gene=static_cast<byte> random(sizeof(byte));
	return gene;
}

static void Individual::setDefaultGeneLength(int &value)
{
  defaulGeneLength=value;
  genes.resize(defaultGeneLength);
}

byte Individual::getGene(int &index)
{
  return genes[index];  
}

void Individual::setGene(int &index, byte &value)
{
  genes[index]=value;
}

int Individual::getSize()
{
  return genes.getSize();
}

static Individual Algorithm::crossover(Individual &indiv1, Individual &indiv2)
{
  // ADD STUFF HERE  
}

static void Algorithm::mutate(Individual &indiv)
{
  // ADD STUFF HERE  
}

static Individual Algorithm::tournamentSelection(Population &pop)
{
  // ADD STUFF HERE 
}

static Population Algorithm::evolvePopulation(Population &pop)
{
  // ADD STUFF HERE  
}

static void FitnessCalc::setSolution(arr:array1<byte, solution.getSize()> &newSolution)
{
	memcpy(&solution, &newSolution, solution.getSize()*sizeof(byte));	
}

static int FitnessCalc::getFitness(Individual &indiv)
{
  // ADD STUFF HERE  
}

static int FitnessCalc::getMaxFitness(Population &pop)
{
  int maxFitness=0;
  int currentFitness=0;
  for (int i=0; i<solution.getSize(); i++)
  {
    currentFitness=pop.getIndividual(i).getFitness();
    if (currentFitness > maxFitness)
    {
        maxFitness=currentFitness;
    }
  }  
  return maxFitness;
}
