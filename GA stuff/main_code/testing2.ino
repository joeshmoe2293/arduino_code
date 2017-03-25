/* Class structure based on code provided at:
http://www.theprojectspot.com/tutorial-post/creating-a-genetic-algorithm-for-beginners/ 
HUGE thanks to fenneking and artemis in the c-plusplus channel on the "Programming" Discord server
for all the debugging and help*/

#include "Arduino.h"
using namespace std;

namespace arr
{
  template <typename T, byte arrSize>
  class array1
  {
    private:
      T arr[arrSize];
    public:
      constexpr byte getSize() const {return arrSize;}
      T& operator[](byte index) {return arr[index];}
      T* getArr() const {return &arr[0];}
  };
}

class Individual
{ 
  private:
    int fitness=0;
    static constexpr int defaultGeneLength=64;
    arr::array1<byte, defaultGeneLength> genes;
    static byte generateRandomGene();

  public:
    void generateIndividual();
    byte getGene(byte index);
    void setGene(byte index, byte value);
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
    template <int popSize> static Individual tournamentSelection(Population<popSize> &pop);

  public:
    template <int popSize> static Population<popSize> evolvePopulation(Population<popSize> &pop);
};

class FitnessCalc
{
  private: static arr::array1<byte, 64> solution;

  public:
    static void setSolution(arr::array1<byte, solution.getSize()> &newSolution);
    static int getFitness(Individual &individual);
    template <int popSize> static int getMaxFitness(Population<popSize> &pop);
};

template <int popSize>
Population<popSize>::Population(bool &initialize)
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

template <int popSize>
Individual Population<popSize>::getIndividual(int &index)
{
  return individuals[index];  
}

template <int popSize>
int Population<popSize>::getSize()
{
  return individuals.getSize();
}

template <int popSize>
void Population<popSize>::saveIndividual(int &index, Individual &indiv)
{
  individuals[index]=indiv;
}

void Individual::generateIndividual()
{
  for (byte i=0; i<genes.getSize(); i++)
  {
    genes[i]=generateRandomGene();
  }  
}

static byte Individual::generateRandomGene()
{
	randomSeed(random(1000));
	byte gene=static_cast<byte> (random(sizeof(byte)));
	return gene;
}

byte Individual::getGene(byte index)
{
  return genes[index];  
}

void Individual::setGene(byte index, byte value)
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

template <int popSize>
static Individual Algorithm::tournamentSelection(Population<popSize> &pop)
{
  // ADD STUFF HERE 
}

template <int popSize>
static Population<popSize> Algorithm::evolvePopulation(Population<popSize> &pop)
{
  // ADD STUFF HERE  
}

static void FitnessCalc::setSolution(arr::array1<byte, solution.getSize()> &newSolution)
{
	memcpy(&solution, &newSolution, solution.getSize()*sizeof(byte));	
}

static int FitnessCalc::getFitness(Individual &indiv)
{
  // ADD STUFF HERE  
}

template <int popSize>
static int FitnessCalc::getMaxFitness(Population<popSize> &pop)
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
