#define defaultOut 9
#define defaultIn 10

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

  namespace operations
  {
    namespace 
    {
      static float add(float x, float y) {return x+y;}
      static float sub(float x, float y) {return x-y;}
      static float mult(float x, float y) {return x*y;}
      static float div(float x, float y) {return x/y;}
    } 
    
    static float (*performOp[])(float, float){add, sub, mult, div};

    enum OPS:byte
    {
      ADD=0,
      SUB=1,
      MULT=2,
      DIV=3
    };
  }
}

/* This is assuming that we're using ultrasonic sensors (might make this better/bigger in future) */
class Individual
{ 
  private:
    int fitness=0, pin1, pin2;
    static constexpr int defaultGeneLength=6;
    util::array<float, defaultGeneLength> genes;
    float generateRandomGene(byte index);
    char convertToChar(byte op);

  public:
    enum positionalValue:byte
    {
      MEASURED=0,
      OP1=1,
      CONSTANT1=2,
      OP2=3,
      CONSTANT2=4,
      CONSTANT3=5
    };

    Individual():Individual(defaultOut, defaultIn){};
    Individual(int pinOut, int pinIn);
    void generateIndividual();
    float& getGene(byte index);
    void setGene(byte index, float value);
    static constexpr int getSize() {return defaultGeneLength;}
    void getMeasured();
    int getPinOut() {return pin1;}
    int getPinIn() {return pin2;}
    String toString();
};

template <byte populationSize>
class Population
{
  private:
    util::array<Individual, populationSize> individuals;
    int pin1, pin2;
  public:
    Population(int pinOut, int pinIn, bool initialize);
    Individual& getIndividual(byte index);
    Individual& getFittest();
    constexpr int getSize() {return populationSize;}
    void saveIndividual(byte index, Individual &indiv);
    int getPinOut() {return pin1;}
    int getPinIn() {return pin2;}
    //const Individual* begin() const {return &individuals[0];}
    //const Individual* end() const {return &individuals[0]+individuals.getSize();}
};

class Algorithm
{
  private:
    static constexpr float uniformRate=0.5, mutationRate=0.015;
    static constexpr boolean elitism=true;
    static constexpr int tournamentSize=10;

    static Individual crossover(Individual &indiv1, Individual &indiv2);
    static void mutate(Individual &indiv);
    template <byte popSize> Individual tournamentSelection(Population<popSize> &pop);

  public:
    template <byte popSize> Population<popSize> evolvePopulation(Population<popSize> &pop);
};

class FitnessCalc
{
  private: 
    float solution;

  public:
    FitnessCalc(float startSolution=25):solution(startSolution){}
    void setSolution(float newSolution=25);
    float getFitness(Individual &individual);
    template <byte popSize> float getFittest(Population<popSize> &pop);
    template <byte popSize> Individual* getFittest(Population<popSize> &pop, bool value);
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
Individual& Population<popSize>::getIndividual(byte index)
{
  return individuals[index];  
}

template <byte popSize>
Individual& Population<popSize>::getFittest()
{
  Serial.println("Fitness calc initialized");
  FitnessCalc fitnessCalc(25);
  Serial.println("Getting result from fitnessCalc.getFittest()");
  float maxFitness=fitnessCalc.getFittest(*this);
  for (byte i=0; i<getSize(); i++)
  {
    Serial.println("Getting fitness of individual " + (i+1));
    if (fitnessCalc.getFitness(getIndividual(i))==maxFitness)
    {
      return getIndividual(i);
    }
  }
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
  Serial.println("Generating an RNG");
  randomSeed(random(1000));
  float gene;

  Serial.print("For gene ");
  Serial.print(index);
  Serial.println(" the randomly generated value is: ");
  switch (index)
  {
    case Individual::OP1: case Individual::OP2:
      gene=static_cast<float>(static_cast<byte>(random(4)));
      break;
    case Individual::MEASURED:
      gene=static_cast<float>(genes[index]); // We don't want to generate a random value instead of using the measured one
      break;
    case Individual::CONSTANT3:
      gene=static_cast<float>(static_cast<byte>(random(10)+1));
      break;
    default:
      gene=static_cast<float>(random(sizeof(byte))+1);
  }
  Serial.println(gene);
  delay(20);
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

  setGene(0, 1409); // FIX THIS LATER
}

char Individual::convertToChar(byte op)
{
  using namespace util;
  switch(op)
  {
    case operations::ADD:
      return '+';
    case operations::SUB:
      return '-';
    case operations::MULT:
      return '*';
    case operations::DIV:
      return '/';
  }
}

String Individual::toString()
{
  String measured(getGene(MEASURED), 3);
  String operation1(convertToChar(getGene(OP1)));
  String constant1(getGene(CONSTANT1));
  String operation2(convertToChar(getGene(OP2)));
  String constant2(getGene(CONSTANT2));
  String constant3(getGene(CONSTANT3));
  return String("((" + measured + operation1 + constant1 + ")" + operation2 + constant2 + ") " + constant3 + " times");
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
  Serial.println("Generating RNG");
  randomSeed(random(1000));
  float randomVal, newOp;

  Serial.println("Begining random gene generation");
  for (byte i=1; i<indiv.getSize(); i++)
  {
    Serial.println("Random value:");
    randomVal=static_cast<float>(random(1));
    Serial.println(randomVal);

    if (randomVal<mutationRate)
    {
      Serial.println("MUTATION");
      switch (i)
      {
        case Individual::OP1:
        case Individual::OP2:
          newOp=static_cast<byte>(random(4));
          indiv.setGene(i, newOp);
          break;
        case Individual::CONSTANT3:
          newOp=static_cast<byte>(random(10)+1);
        default:
          indiv.setGene(i, static_cast<float>(random(sizeof(byte))));
      }
    }
  }
  Serial.println("Returning from mutate method");
}

template <byte popSize>
Individual Algorithm::tournamentSelection(Population<popSize> &pop)
{
  Serial.println("Generating random number generator...");
  randomSeed(random(1000));
  Serial.println("Generating tournamentPop...");
  Population<popSize> tournamentPop(pop.getPinOut(), pop.getPinIn(), false);
  Serial.println("Generating tournament population...");
  for (byte i=0; i<popSize; i++)
  {
    byte randID=static_cast<int>(random()*popSize);
    tournamentPop.saveIndividual(i, pop.getIndividual(randID));
  }

  Serial.println("Getting fittest");
  Individual fittest=tournamentPop.getFittest();
  Serial.println("Returning fittest from tournamentSelection");
  return fittest;
}

template <byte popSize>
Population<popSize> Algorithm::evolvePopulation(Population<popSize> &pop)
{
  Serial.println("Generating newPop...");
  Population<popSize> newPop(pop.getPinOut(), pop.getPinIn(), false);
  byte elitismOffset=0;

  Serial.println("Elitism: ");
  Serial.println(elitism);
  if (elitism)
  {
    Serial.println("Saving result of pop.getFittest()");
    newPop.saveIndividual(0, pop.getFittest());
    elitismOffset=1;
  }

  Serial.println("Generating individuals in evolvePopulation...");
  Individual indiv1, indiv2, newIndiv;

  Serial.println("Beginning selection process");
  for (byte i=elitismOffset; i<pop.getSize(); i++)
  {
    Serial.println("Getting individual 1");
    indiv1=tournamentSelection(pop);
    Serial.println("Getting individual 2");
    indiv2=tournamentSelection(pop);
    Serial.println("Crossing over...");
    newIndiv=crossover(indiv1, indiv2);
    Serial.println("Saving individual...");
    newPop.saveIndividual(i, newIndiv);
  }

  Serial.println("Mutating...");
  for (byte j=elitismOffset; j<newPop.getSize(); j++)
  {
    mutate(newPop.getIndividual(j));
  }

  Serial.println("Returning newPop...");
  return newPop;
}

void FitnessCalc::setSolution(float newSolution)
{
	solution=newSolution;
}

float FitnessCalc::getFitness(Individual &indiv)
{
  Serial.println("In fitnessCalc.getFitness()");
  using namespace util;
  float fitness=0, calculated=0;
  byte OP1=static_cast<byte>(indiv.getGene(Individual::OP1));
  Serial.print("OP1 is ");
  Serial.println(OP1);
  byte OP2=static_cast<byte>(indiv.getGene(Individual::OP2));
  Serial.print("OP2 is ");
  Serial.println(OP2);
  auto func1=operations::performOp[OP1];
  Serial.print("Setting func1...");
  auto func2=operations::performOp[OP2];
  Serial.print("Setting func2...");
  for (byte i=0; i<indiv.getGene(Individual::CONSTANT3); i++)
  {
    indiv.getMeasured();
    Serial.print("func1(measured, constant1)=");
    Serial.println(func1(indiv.getGene(Individual::MEASURED), indiv.getGene(Individual::CONSTANT1)));
    Serial.print("func2(previous result)=");
    Serial.println(func2(func1(indiv.getGene(Individual::MEASURED), indiv.getGene(Individual::CONSTANT1)), indiv.getGene(Individual::CONSTANT2)));
    delay(20);
    calculated+=func2(func1(indiv.getGene(Individual::MEASURED), indiv.getGene(Individual::CONSTANT1)), indiv.getGene(Individual::CONSTANT2));
  }
  calculated/=indiv.getGene(Individual::CONSTANT3);

  Serial.print("Returning fitness of: ");
  Serial.println(abs(calculated-solution));
  return abs(calculated-solution);
}

template <byte popSize>
float FitnessCalc::getFittest(Population<popSize> &pop)
{
  float maxFitness=0, currentFitness=0;
  for (byte i=0; i<pop.getSize(); i++)
  {
    currentFitness=getFitness(pop.getIndividual(i));
    if ((100-currentFitness) > maxFitness)
    {
        maxFitness=(100-currentFitness);
    }
  }
  return maxFitness;
}

/* BELOW IS THE ACTUAL RUNNING CODE */

int ping(int trig, int echo)
{
  digitalWrite(trig, LOW);
  delay(2);
  digitalWrite(trig, HIGH);
  delay(5);
  digitalWrite(trig, LOW);
  delay(2);

  return 1409;
}

int trig1=9, echo1=10, generation=1;
bool foundSolution=false;

Population<10> Sensor(trig1, echo1, true);
Algorithm algorithm;
FitnessCalc fitnessCalc(25);

void setup()
{
  Serial.begin(9600);
  pinMode(trig1, OUTPUT);
  pinMode(echo1, INPUT);
  pinMode(13, OUTPUT);
}

void loop()
{
  while(!foundSolution)
  {
    Serial.print("Ping returns: ");
    Serial.println(ping(trig1, echo1));
    
    Serial.println("Generation: " + generation);

    Serial.println("Best solution so far: ");
    Serial.println(fitnessCalc.getFittest(Sensor));
    Serial.println("With a formula of: ");
    Serial.println(Sensor.getFittest().toString());

    Sensor=algorithm.evolvePopulation(Sensor);

    if ((fitnessCalc.getFittest(Sensor)>95))
    {
      Serial.println("Exiting loop");
      foundSolution=true;
    }

    generation++;
  }

  Serial.println("Solution!");
  Serial.println("Best solution's fitness is: ");
  Serial.println(fitnessCalc.getFittest(Sensor));
  Serial.println("Solution's formula is: ");
  Serial.println(Sensor.getFittest().toString());

  digitalWrite(13, HIGH);
  delay(1000);
  digitalWrite(13, LOW);
  delay(1000);
}
