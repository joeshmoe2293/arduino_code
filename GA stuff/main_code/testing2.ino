/* Class structure based on code provided at:
http://www.theprojectspot.com/tutorial-post/creating-a-genetic-algorithm-for-beginners/ 
HUGE thanks to fenneking and artemis in the c-plusplus channel on the "Programming" Discord server
for all the debugging and help*/

#include "Arduino.h"
using namespace std;

namespace util
{
  template <typename T, byte arrSize>
  class array
  {
    private:
      T arr[arrSize];
      
    public:
      constexpr byte getSize() const {return arrSize;}
      T& operator[](byte index) {return arr[index];}
      T* getArr() const {return &arr[0];}
  };

  class operations
  {
    private:
      static float add(float x, float y) {return x+y;}
      static float sub(float x, float y) {return x-y;}
      static float mult(float x, float y) {return x*y;}
      static float div(float x, float y) {return static_cast<byte>(x/y);}
      
    public:
      static constexpr float (*performOp[])(float, float){&add, &sub, &mult, &div};
      
      enum OPS
      {
        ADD=0,
        SUB,
        MULT,
        DIV
      };
  };
}

class Individual
{ 
  private:
    int fitness=0;
    static constexpr int defaultGeneLength=6;
    util::array<float, defaultGeneLength> genes;
    float generateRandomGene(byte index);

  public:
    enum positionalValue
    {
      MEASURED=0,
      OP1,
      CONSTANT1,
      OP2,
      CONSTANT2,
      CONSTANT3
    };
  
    void generateIndividual();
    float& getGene(byte index);
    void setGene(byte index, float value);
    int getSize() {return defaultGeneLength;}
};

template <byte populationSize>
class Population
{
  private:
    util::array<Individual, populationSize> individuals;
  public:
    Population(bool initialize);
    Individual getIndividual(byte index);
    Individual getFittest(); // ADD STUFF HERE LATER
    int getSize() {return populationSize;}
    void saveIndividual(byte index, Individual &indiv);
};

class Algorithm
{
  private:
    static constexpr float uniformRate=0.5, mutationRate=0.015;
    static constexpr boolean elitism=true;
    static constexpr int tournamentSize=10;

    static Individual crossover(Individual &indiv1, Individual &indiv2);
    static void mutate(Individual &indiv);
    static template <byte popSize> Individual tournamentSelection(Population<popSize> &pop);

  public:
    static template <byte popSize> Population<popSize> evolvePopulation(Population<popSize> &pop);
};

class FitnessCalc
{
  private: float solution;

  public:
    FitnessCalc(float startSolution=25):solution(startSolution){}
    void setSolution(float newSolution=25);
    int getFitness(Individual &individual);
    template <byte popSize> static int getFittest(Population<popSize> &pop);
};

template <byte popSize>
Population<popSize>::Population(bool initialize)
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

template <byte popSize>
Individual Population<popSize>::getIndividual(byte index)
{
  return individuals[index];  
}

template <byte popSize>
void Population<popSize>::saveIndividual(byte index, Individual &indiv)
{
  individuals[index]=indiv;
}

void Individual::generateIndividual()
{
  for (byte i=0; i<genes.getSize(); i++)
  {
    genes[i]=generateRandomGene(i);
  }  
}

float Individual::generateRandomGene(byte index)
{
	randomSeed(random(1000));
	float gene;
  
	switch (index)
  {
    case static_cast<byte>(OP1): case static_cast<byte>(OP2):
      gene=static_cast<byte> (random(4));
      break;
    case static_cast<byte>(MEASURED):
      return genes[index]; // We don't want to generate a random value instead of using the measured one
    default:
      gene=static_cast<float> (random(sizeof(byte)));
  }
  
	return gene;
}

float& Individual::getGene(byte index)
{
  return genes[index];  
}

void Individual::setGene(byte index, float value)
{
  genes[index]=value;
}

Individual Algorithm::crossover(Individual &indiv1, Individual &indiv2)
{
  randomSeed(random(1000));
  Individual result;
  float randomVal;

  for (byte i=0; i<indiv1.getSize(); i++)
  {
    randomVal=static_cast<float>(random());

    if (randomVal<mutationRate)
    {
      result.setGene(i, indiv1.getGene(i));
    }
    else
    {
      result.setGene(i, indiv2.getGene(i));  
    }
  }

  return result;
}

void Algorithm::mutate(Individual &indiv)
{
  randomSeed(random(1000));
  float randomVal, newOp;
  
  for (byte i=1; i<indiv.getSize(); i++)
  {
    randomVal=static_cast<float>(random());

    if (randomVal<mutationRate)
    {
      switch (i)
      {
        case static_cast<byte>(Individual::OP1):
        case static_cast<byte>(Individual::OP2):
          newOp=((static_cast<byte>(indiv.getGene(i)))%2==0)?indiv.getGene(i)+1:indiv.getGene(i)-1;
          indiv.setGene(i, newOp);
          break;
        default:
          indiv.setGene(i, static_cast<float>(random(sizeof(float))));
      }
    }
  }
}

template <byte popSize>
Individual Algorithm::tournamentSelection(Population<popSize> &pop)
{
  // ADD STUFF HERE 
}

template <byte popSize>
Population<popSize> Algorithm::evolvePopulation(Population<popSize> &pop)
{
  Population<popSize> newPop(false);
  byte elitismOffset=0;

  if (elitism)
  {
    newPop.saveIndividual(0, pop.getFittest());
    elitismOffset=1;
  }

  Individual indiv1, indiv2, newIndiv;
  
  for (int i=elitismOffset; i<pop.getSize(); i++)
  {
    indiv1=tournamentSelection(pop);
    indiv2=tournamentSelection(pop);
    newIndiv=crossover(indiv1, indiv2);
    newPop.saveIndividual(i, newIndiv);
  }

  for (int i=elitismOffset; i<newPop.getSize(); i++)
  {
    mutate(newPop.getIndividual(i));
  }

  return newPop;
}

void FitnessCalc::setSolution(float newSolution)
{
	solution=newSolution;
}

int FitnessCalc::getFitness(Individual &indiv)
{
  // ADD STUFF HERE  
}

template <byte popSize>
int FitnessCalc::getFittest(Population<popSize> &pop)
{
  int maxFitness=0;
  int currentFitness=0;
  for (int i=0; i<popSize; i++)
  {
    currentFitness=pop.getIndividual(i).getFitness();
    if (currentFitness > maxFitness)
    {
        maxFitness=currentFitness;
    }
  }  
  return maxFitness;
}
