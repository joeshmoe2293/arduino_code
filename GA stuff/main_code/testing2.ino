#define defaultOut 12
#define defaultIn 13

/* Class structure based on code provided at:
http://www.theprojectspot.com/tutorial-post/creating-a-genetic-algorithm-for-beginners/ 
HUGE thanks to fenneking, artemism, and TheBev123 in the c-plusplus channel on the "Programming" Discord server
for all the debugging and help*/

#include "Arduino.h"

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

/* This is assuming that we're using ultrasonic sensors (might make this better/bigger in future) */
class Individual
{ 
  private:
    int fitness=0, pin1, pin2;
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

    Individual():Individual(defaultOut, defaultIn){};
    Individual(int pinOut, int pinIn);
    void generateIndividual();
    float& getGene(byte index);
    void setGene(byte index, float value);
    int getSize() {return defaultGeneLength;}
    void getMeasured();
    int getPinOut() {return pin1;}
    int getPinIn() {return pin2;}
};

template <byte populationSize>
class Population
{
  private:
    util::array<Individual, populationSize> individuals;
    int pin1, pin2;
  public:
    Population(int pinOut, int pinIn, bool initialize);
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
    template <byte popSize> static Individual tournamentSelection(Population<popSize> &pop);

  public:
    template <byte popSize> static Population<popSize> evolvePopulation(Population<popSize> &pop);
};

class FitnessCalc
{
  private: 
    float solution;

  public:
    FitnessCalc(float startSolution=25):solution(startSolution){}
    void setSolution(float newSolution=25);
    float getFitness(Individual &individual);
    template <byte popSize> static float getFittest(Population<popSize> &pop);
};

template <byte popSize>
Population<popSize>::Population(int pinOut, int pinIn, bool initialize):pin1(pinOut),pin2(pinIn)
{
  if (initialize)
  {
    for (int i=0; i<(individuals.getSize()); i++)
    {
      Individual newIndividual(pin1, pin2);
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

Individual::Individual(int pinOut, int pinIn):pin1(pinOut), pin2(pinIn)
{
  pinMode(pin1, OUTPUT);
  pinMode(pin2, INPUT);
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
      gene=static_cast<int>(random(4));
      break;
    case static_cast<byte>(MEASURED):
      return genes[index]; // We don't want to generate a random value instead of using the measured one
    case static_cast<byte>(CONSTANT3):
      gene=static_cast<int>(random(10)+1);
      break;
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

void Individual::getMeasured()
{
  digitalWrite(pin1, LOW);
  delayMicroseconds(2);
  digitalWrite(pin1, HIGH);
  delayMicroseconds(5);
  digitalWrite(pin1, LOW);

  setGene(0, static_cast<float>(pulseIn(pin2, HIGH)));
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
  randomSeed(random(1000));
  Population<popSize> tournamentPop(false);
  for (byte i=0; i<popSize; i++)
  {
    byte randID=static_cast<int>(random()*popSize);
    tournamentPop.saveIndividual(i, pop.getIndividual(randID));
  }

  Individual fittest=tournamentPop.getFittest();
  return fittest;
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

float FitnessCalc::getFitness(Individual &indiv)
{
  using namespace util;
  float fitness=0, calculated=0;
  auto OP1=static_cast<byte>(indiv.getGene(Individual::OP1));
  auto OP2=static_cast<byte>(indiv.getGene(Individual::OP2));
  auto func1=operations::performOp[OP1];
  auto func2=operations::performOp[OP2];
  for (byte i=0; i<indiv.getGene(static_cast<byte>(Individual::CONSTANT3)); i++)
  {
    indiv.getMeasured();
    calculated+=func2(func1(indiv.getGene(Individual::MEASURED), indiv.getGene(Individual::CONSTANT1)), indiv.getGene(Individual::CONSTANT2));
  }
  calculated/=indiv.getGene(static_cast<byte>(Individual::CONSTANT3));

  return abs(calculated-solution);
}

template <byte popSize>
float FitnessCalc::getFittest(Population<popSize> &pop)
{
  float maxFitness=0, currentFitness=0;
  for (int i=0; i<popSize; i++)
  {
    currentFitness=pop.getIndividual(i).getFitness();
    if ((100-currentFitness) > maxFitness)
    {
        maxFitness=(100-currentFitness);
    }
  }  
  return maxFitness;
}

/* BELOW IS THE ACTUAL RUNNING CODE */

int trig1=9, echo1=10, trig2=12, echo2=13, generation=1;

void setup()
{
  Serial.begin(9600);
  Population<10> Sensor1(trig1, echo1, true);
  Population<10> Sensor2(trig2, echo2, true);
}

void loop()
{
  while (FitnessCalc<Sensor1.getSize()>.getFittest(Sensor1)<95 && FitnessCalc<Sensor2.getSize()>.getFittest(Sensor2)<95)
  {
    Serial.println("Generation: " + generation);

    generation++;
  }
}
