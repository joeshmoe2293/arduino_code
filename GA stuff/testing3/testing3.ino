#define ECHO 8
#define TRIG 9

#include <Entropy.h>
#include "Arduino.h"

namespace util
{
  template <typename T, byte arrSize>
  class array
  {
    private:
      T data[arrSize];
  
    public:
      constexpr byte getSize() const {
        return arrSize;
      }
      T& operator[](byte index) {
        return data[index];
      }
      T* getData() const {
        return &data[0];
      }
  };
}

namespace operations
{
float performOp(byte operation, float x, float y)
{
  switch (operation)
  {
    case 0:
      return x + y; // Case 0 = addition
    case 1:
      return x - y; // Case 1 = subtraction
    case 2:
      return x * y; // Case 2 = multiplication
    case 3:
      return x / y; // Case 3 = division
  }
}
}

int getMeasurement()
{
  digitalWrite(TRIG, LOW);
  delay(2);
  digitalWrite(TRIG, HIGH);
  delay(5);
  digitalWrite(TRIG, LOW);

  int result = pulseIn(ECHO, HIGH);

  return result;
}

class Individual
{
  private:
    byte fitness = 0;
    static constexpr byte geneLength = 6;
    util::array<int, geneLength> genes;

    void generateGene(byte index)
    {
      //Serial.println("GENERATING GENE AT " + index);

      int gene;

      switch (index)
      {
        case 1: case 3:
          gene=static_cast<int>(random(4));
          break;
        case 5:
          gene=static_cast<int>(random(10) + 1);
          break;
        default:
          gene=static_cast<int>(random(1, 256));
      }

      //Serial.println("WITH A VALUE OF " + gene);
      genes[index] = gene;
    }

  public:
    void generateIndividual()
    {
      //Serial.println("GENERATING GENES!!!");
      for (byte i = 1; i < 6; i++) // Gene 0 is the measured value, so we don't randomly generate that
      {
        generateGene(i);
      }
    }

    void setGene(byte index, int value) {
      genes[index] = value;
    }

    char convertToChar(int op) const
    {
      switch (op)
      {
        case 0:
          return '+';
        case 1:
          return '-';
        case 2:
          return '*';
        case 3:
          return '/';
      }
    }

    int& at(byte index) {
      return genes[index];
    }

    static constexpr byte getSize() {
      return geneLength;
    }

    String toString()
    {
      //Serial.println("In toString()");

      String measured = String(genes[0]);
      String op1 = String(convertToChar(genes[1]));
      String const1 = String(genes[2]);
      String op2 = String(convertToChar(genes[3]));
      String const2 = String(genes[4]);
      String const3 = String(genes[5]);

      String result = String("((" + measured + op1 + const1 + ")" + op2 + const2 + ") " + const3 + " times.");

      //Serial.println("And it returns: " + result);

      return result;
    }
};

template <byte popSize>
class Population
{
  private:
    util::array<Individual, popSize> individuals;

  public:
    Population(bool initialize)
    {
      if (initialize)
      {
        Serial.println("Generating Individuals!");
        for (byte i = 0; i < popSize; i++)
        {
          individuals[i].generateIndividual();
        }
      }
    }

    void reset()
    {
      for (byte i=0; i<popSize; i++)
      {
        individuals[i].generateIndividual();
      }
    }

    Individual& at(byte index) {
      return individuals[index];
    }

    constexpr byte getSize() {
      return popSize;
    }

    void saveIndividual(byte index, Individual indiv) {
      individuals[index] = indiv;
    }

    Individual getIndividual(byte index)
    {
      return individuals[index];
    }
};

class Algorithm
{
  private:
    byte mutationRate = 10, solution = 25; // When a number will be generated later, if it's less than this, mutation happens
    bool elitism;
    constexpr static byte tournamentSize = 3, elitismSize=2, rouletteSize=5, threshold=5;
    byte numTimesRun=0;
    unsigned long currentTime;
    float timeTaken;

    Individual crossover(Individual &indiv1, Individual &indiv2)
    {
      Serial.println("In crossover()");
      Individual result;
      byte randomVal;

      for (byte i = 0; i < indiv1.getSize(); i++)
      {
        randomVal=static_cast<byte>(random(100)); // Random value between 0 and 99, where 0 through 4 cause mutations

        if (randomVal < mutationRate)
        {
          Serial.println("CROSSOVER!!!!!");
          result.setGene(i, indiv1.at(i));
        }
        else {
          result.setGene(i, indiv2.at(i));
        }
      }

      return result;
    }

    void mutate(Individual &indiv)
    {
      Serial.println("In mutate()");

      byte randomVal;

      for (byte i = 1; i < indiv.getSize(); i++)
      {
        randomVal=static_cast<byte>(random(100));

        if (randomVal < mutationRate)
        {
          Serial.println("MUTATION!!!");
          switch (i)
          {
            case 1: case 3:
              indiv.setGene(i, static_cast<int>(random(4)));
              break;
            case 5:
              indiv.setGene(i, static_cast<int>(random(10) + 1));
              break;
            default:
              indiv.setGene(i, static_cast<int>(random(1, 256)));
          }
        }
      }
    }

  public:
    void reset()
    {
      numTimesRun=0;
      currentTime=0;
      timeTaken=0;
    }
  
    float getAvgRunTime()
    {
      return timeTaken/numTimesRun;
    }

    byte getThreshold()
    {
      return threshold;
    }
  
    float getFitness(Individual indiv)
    {
      Serial.println("In getFitness()");
      float result=0;

      for (byte i=0; i<indiv.at(5); i++) // Index 5 in genes is the number of times the reading should be taken
      {
        indiv.at(0) = getMeasurement();
        float op1 = operations::performOp(indiv.at(1), indiv.at(0), indiv.at(2));
        Serial.println("Formula:");
        Serial.println(String(indiv.at(0)) + indiv.convertToChar(indiv.at(1)) + String(indiv.at(2)) + "=" + String(op1));
        float addedTerm = operations::performOp(indiv.at(3), op1, indiv.at(4));
        result += addedTerm;
        Serial.println(String(op1) + indiv.convertToChar(indiv.at(3)) + String(indiv.at(4)) + "=" + String(addedTerm));
        //Serial.print("Added ");
        //Serial.println(addedTerm);
      }
      result /= indiv.at(5);
      Serial.print("Result=");
      Serial.println(result);

      return abs(solution - result);
    }

    template <byte popSize> Individual getFittest(Population<popSize> &pop)
    {
      //Serial.println("In getFittest()");
      float fitness = 100, maxFitness = getFitness(pop.at(0));
      byte fittestIndex = 0;

      for (byte i = 0; i < popSize; i++)
      {
        fitness = getFitness(pop.at(i));
        //Serial.print("Fitness of indivdiual ");
        //Serial.print(i);
        //Serial.print(" is ");
        //Serial.println(fitness);

        if (fitness < maxFitness)
        {
          //Serial.println("NEW MAX FITNESS!!!");
          maxFitness = fitness;
          fittestIndex = i;
        }
      }

      return pop.at(fittestIndex);
    }

    template <byte popSize> Individual getFittest(Population<popSize> &pop, Individual &ignored)
    {
      float fitness = 100, maxFitness = getFitness(pop.at(0)), ignoredFitness=getFitness(ignored);
      byte fittestIndex = 0;

      for (byte i=0; i<popSize; i++)
      {
        fitness=getFitness(pop.at(i));

        if (fitness<=threshold)
        {
          return pop.getIndividual(i);
        }

        if (fitness==ignoredFitness)
        {
          continue;
        }

        if (fitness < maxFitness)
        {
          maxFitness=fitness;
          fittestIndex=i;
        }
      }
      return pop.getIndividual(fittestIndex);
    }

    template <byte popSize> Individual tournamentSelection(Population<popSize> &pop)
    {
      currentTime=millis();
      Serial.println("In tournamentSelection(), starting clock for measuring time taken...");

      Population<tournamentSize> tournamentPop(false);

      for (byte i = 0; i < tournamentSize; i++)
      {
        byte randomID=static_cast<byte>(random(popSize));
        Serial.println("ID chosen at random is: " + String(randomID));

        tournamentPop.saveIndividual(i, pop.at(randomID));
      }

      Individual fittest = getFittest(tournamentPop);
      Serial.println("We've gotten the fittest member of tournamentPop!");

      timeTaken+=static_cast<float>(millis()-currentTime);
      
      return fittest;
    }

    template <byte popSize> Individual elitismSelection(Population<popSize> &pop)
    {
      currentTime=millis();
      Serial.println("In elitismSelection()");

      Population<elitismSize> tournamentPop(false);

      tournamentPop.saveIndividual(0, getFittest(pop));
      tournamentPop.saveIndividual(1, getFittest(pop, tournamentPop.at(0)));

      Individual fittest=getFittest(tournamentPop);
      Serial.println("We've gotten the fittest member of elitismPop!");

      timeTaken+=static_cast<float>(millis()-currentTime);
      
      return fittest;
    }

    template <byte popSize> Individual rouletteSelection(Population<popSize> &pop)
    {
      currentTime=millis();
      Serial.println("In rouletteSelection()");

      float totalFitness=0, partialFitness=0;
      Population<rouletteSize> tournamentPop(false);

      for (byte i=0; i<rouletteSize; i++)
      {
        byte randomID=static_cast<byte>(random(popSize));
        tournamentPop.saveIndividual(i, pop.at(randomID));
        partialFitness=getFitness(tournamentPop.at(i));

        if (partialFitness<threshold)
        {
          return tournamentPop.getIndividual(i);
        }
        
        totalFitness+=partialFitness;
      }

      byte minBound=random(static_cast<int>(totalFitness/rouletteSize));
      partialFitness=0;

      for (byte i=rouletteSize-1; i>=0; i--)
      {
        partialFitness=getFitness(tournamentPop.at(i));
        if (partialFitness<=minBound)
        {
          return tournamentPop.getIndividual(i);
        }
      }
    }

    template <byte popSize> Population<popSize> evolvePopulation(Population<popSize> &pop, byte selectionType)
    {
      Serial.println("In evolvePopulation()");
      Population<popSize> newPop(false);
      byte elitismOffset = 0;

      switch(selectionType)
      {
        case 1:
          if (elitism)
          {
            Serial.println("Saving fittest individual...");
            newPop.saveIndividual(0, getFittest(pop));
            elitismOffset = 1;
          }
    
          Serial.println("Beginning tournament selection...");
          for (byte i = elitismOffset; i < popSize; i++)
          {
            Serial.println("Tournament Selection " + String(i));
            Individual indiv1 = tournamentSelection(pop);
    
            delay(100);
            
            Serial.println("Tournament Selection " + String(i + 1));
            Individual indiv2 = tournamentSelection(pop);
    
            delay(100);
            
            Individual crossed = crossover(indiv1, indiv2);
    
            delay(100);
            
            newPop.saveIndividual(i, crossed);
          }
          break;
        case 2:
          if (elitism)
          {
            Serial.println("Saving fittest individual...");
            newPop.saveIndividual(0, getFittest(pop));
            elitismOffset = 1;
          }
    
          Serial.println("Beginning elitism selection...");
          for (byte i = elitismOffset; i < popSize; i++)
          {
            Serial.println("Elitism Selection " + String(i));
            Individual indiv1 = elitismSelection(pop);
    
            delay(100);
            
            Serial.println("Elitism Selection " + String(i + 1));
            Individual indiv2 = elitismSelection(pop);
    
            delay(100);
            
            Individual crossed = crossover(indiv1, indiv2);
    
            delay(100);
            
            newPop.saveIndividual(i, crossed);
          }
          break;
        case 3:
          if (elitism)
          {
            Serial.println("Saving fittest individual...");
            newPop.saveIndividual(0, getFittest(pop));
            elitismOffset = 1;
          }
    
          Serial.println("Beginning elitism selection...");
          for (byte i = elitismOffset; i < popSize; i++)
          {
            Serial.println("Roulette Selection " + String(i));
            Individual indiv1 = rouletteSelection(pop);
    
            delay(100);
            
            Serial.println("Roulette Selection " + String(i + 1));
            Individual indiv2 = rouletteSelection(pop);
    
            delay(100);
            
            Individual crossed = crossover(indiv1, indiv2);
    
            delay(100);
            
            newPop.saveIndividual(i, crossed);
          }
          break;
      }

      Serial.println("Mutating all the contestants in newPop...");
      for (byte j = elitismOffset; j < popSize; j++)
      {
        mutate(newPop.at(j));
      }

      delay(100);

      return newPop;
    }
};

Algorithm alg;
Population<10> formulae(true);
int led = 13, finalFitness;
bool foundSolution = false;
Individual fittest;

struct foundSolutions
{
  Individual solution;
  int fitness;
  float runTime;
};

foundSolutions solutions[3];

void blink()
{
  digitalWrite(led, HIGH);
  delay(250);
  digitalWrite(led, LOW);
  delay(250);
  digitalWrite(led, HIGH);
  delay(250);
  digitalWrite(led, LOW);
  delay(250);
}

void setup()
{
  blink();
  
  Serial.begin(9600);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(led, OUTPUT);
  for (byte i = 0; i < formulae.getSize(); i++)
  {
    Serial.println("Setting initial genes...");
    formulae.at(i).setGene(0, getMeasurement());
  }

  Entropy.Initialize();
  randomSeed(Entropy.random()^Entropy.random(analogRead(A3)));
}

void loop()
{
  if(!foundSolution)
  {
    /*
    while (!foundSolution)
    {
      for (byte i = 0; i < formulae.getSize(); i++)
      {
        Serial.print("Formulae ");
        Serial.print(i);
        Serial.print(" has a formula of ");
        Serial.println(formulae.at(i).toString());
      }
  
      delay(100);
  
      blink();
  
      Serial.print("Ping returns: ");
      Serial.println(getMeasurement());
  
      delay(100);
  
      blink();
  
      delay(100);
  
      blink();
  
      formulae = alg.evolvePopulation(formulae, 1);
  
      delay(100);
  
      blink();
  
      fittest=alg.getFittest(formulae);
  
      if (alg.getFitness(fittest) < alg.getThreshold())
      {
        Serial.println("Exiting loop!");
        foundSolution = true;
        finalFitness=alg.getFitness(fittest)+25;
        break;
      }
    }
    
    delay(100);
  
    blink();
  
    Serial.println("\n\n\n");
  
    Serial.print("Final Solution's output measurement is: ");
    Serial.println(finalFitness);
    Serial.print("And its formula is: ");
    Serial.println(fittest.toString());

    Serial.println("\n\n\n");

    Serial.print("Time taken for tournamentPop to run completely is: ");
    Serial.println(alg.getAvgRunTime());
  
    delay(100);
  
    blink();
  
    solutions[0]={fittest, finalFitness, alg.getAvgRunTime()};

    
    formulae.reset();
    alg.reset();
    foundSolution=false;
  
    while (!foundSolution)
    {
      for (byte i = 0; i < formulae.getSize(); i++)
      {
        Serial.print("Formulae ");
        Serial.print(i);
        Serial.print(" has a formula of ");
        Serial.println(formulae.at(i).toString());
      }
  
      delay(100);
  
      blink();
  
      Serial.print("Ping returns: ");
      Serial.println(getMeasurement());
  
      delay(100);
  
      blink();
  
      delay(100);
  
      blink();
  
      formulae = alg.evolvePopulation(formulae, 2);
  
      delay(100);
  
      blink();
  
      fittest=alg.getFittest(formulae);
  
      if (alg.getFitness(fittest) < alg.getThreshold())
      {
        Serial.println("Exiting loop!");
        foundSolution = true;
        finalFitness=alg.getFitness(fittest)+25;
        break;
      }
    }
    
    delay(100);
  
    blink();
  
    Serial.println("\n\n\n");
  
    Serial.print("Final Solution's output measurement is: ");
    Serial.println(finalFitness);
    Serial.print("And its formula is: ");
    Serial.println(fittest.toString());

    Serial.println("\n\n\n");

    Serial.print("Time taken for tournamentPop to run completely is: ");
    Serial.println(alg.getAvgRunTime());
  
    delay(100);
  
    blink();
    
    solutions[1]={fittest, finalFitness, alg.getAvgRunTime()};
    formulae.reset();
    alg.reset();
    foundSolution=false;*/

    while (!foundSolution)
      {
        for (byte i = 0; i < formulae.getSize(); i++)
        {
          Serial.print("Formulae ");
          Serial.print(i);
          Serial.print(" has a formula of ");
          Serial.println(formulae.at(i).toString());
        }
    
        delay(100);
    
        blink();
    
        Serial.print("Ping returns: ");
        Serial.println(getMeasurement());
    
        delay(100);
    
        blink();
  
        delay(100);
    
        blink();
    
        formulae = alg.evolvePopulation(formulae, 3);
    
        delay(100);
    
        blink();
    
        fittest=alg.getFittest(formulae);
    
        if (alg.getFitness(fittest) < alg.getThreshold())
        {
          Serial.println("Exiting loop!");
          foundSolution = true;
          finalFitness=alg.getFitness(fittest)+25;
          break;
        }
      }
  
    delay(100);
  
    blink();
  
    Serial.println("\n\n\n");
  
    Serial.print("Final Solution's output measurement is: ");
    Serial.println(finalFitness);
    Serial.print("And its formula is: ");
    Serial.println(fittest.toString());

    Serial.println("\n\n\n");

    Serial.print("Time taken for tournamentPop to run completely is: ");
    Serial.println(alg.getAvgRunTime());
  
    delay(100);
  
    blink();

    solutions[2]={fittest, finalFitness, alg.getAvgRunTime()};
    formulae.reset();
    alg.reset();
    foundSolution=false;

    Serial.println("\n\n\n\n\n\n");
    Serial.println("Solutions:");

    for (byte i=0; i<3; i++)
    {
      Serial.print("Solution's formula is: ");
      Serial.print(solutions[i].solution.toString());
      Serial.print(" which returns a value of ");
      Serial.print(solutions[i].fitness);
      Serial.print(" and took a total time of ");
      Serial.println(solutions[i].runTime);
    }
  }
}
/*NOTE, good solutions generated include: ((measured-159)/55) 8 times,
(measured+144)/70 5 times, (measured+195)/80, (measured/125)+12 6 times
(measured/20)-50 4 times
*/
