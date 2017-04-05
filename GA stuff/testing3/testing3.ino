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

      randomSeed(random(analogRead(A4))^analogRead(A0));

      int gene;

      switch (index)
      {
        case 1: case 3:
          gene = random(4);
          break;
        case 5:
          gene = random(10) + 1;
          break;
        default:
          gene = random(1, 256);
      }

      //Serial.println("WITH A VALUE OF " + gene);
      genes[index] = gene;
    }

  public:
    void generateIndividual()
    {
      Serial.println("GENERATING GENES!!!");
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

    Individual& at(byte index) {
      return individuals[index];
    }

    constexpr byte getSize() {
      return popSize;
    }

    void saveIndividual(byte index, Individual indiv) {
      individuals[index] = indiv;
    }
};

class Algorithm
{
  private:
    byte mutationRate = 10, solution = 25, maxFitness = 100; // When a number will be generated later, if it's less than this, mutation happens
    bool elitism;
    constexpr static byte tournamentSize = 3;

    Individual crossover(Individual &indiv1, Individual &indiv2)
    {
      Serial.println("In crossover()");
      randomSeed(random(analogRead(A4))^analogRead(A0));
      Individual result;
      byte randomVal;

      for (byte i = 0; i < indiv1.getSize(); i++)
      {
        randomVal = random(100); // Random value between 0 and 99, where 0 through 4 cause mutations

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
      randomSeed(random(analogRead(A4))^analogRead(A0));

      byte randomVal;

      for (byte i = 1; i < indiv.getSize(); i++)
      {
        randomVal = random(100);

        if (randomVal < mutationRate)
        {
          Serial.println("MUTATION!!!");
          switch (i)
          {
            case 1: case 3:
              indiv.setGene(i, random(4));
              break;
            case 5:
              indiv.setGene(i, random(10) + 1);
              break;
            default:
              indiv.setGene(i, random(1, 256));
          }
        }
      }
    }

  public:
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
        Serial.print("Fitness of indivdiual ");
        Serial.print(i);
        Serial.print(" is ");
        Serial.println(fitness);

        if (fitness < maxFitness)
        {
          Serial.println("NEW MAX FITNESS!!!");
          maxFitness = fitness;
          fittestIndex = i;
        }
      }

      return pop.at(fittestIndex);
    }

    template <byte popSize> Individual tournamentSelection(Population<popSize> &pop)
    {
      Serial.println("In tournamentSelection()");
      randomSeed(random(analogRead(A4))^analogRead(A0));

      Population<tournamentSize> tournamentPop(false);

      for (byte i = 0; i < tournamentSize; i++)
      {
        byte randomID = random(popSize);
        Serial.println("ID chosen at random is: " + String(randomID));

        tournamentPop.saveIndividual(i, pop.at(randomID));
      }

      Individual fittest = getFittest(tournamentPop);
      Serial.println("We've gotten the fittest member of tournamentPop!");

      return fittest;
    }

    template <byte popSize> Population<popSize> evolvePopulation(Population<popSize> &pop)
    {
      Serial.println("In evolvePopulation()");
      Population<popSize> newPop(false);
      byte elitismOffset = 0;

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
int led = 13, generation = 1, finalFitness;
bool foundSolution = false;
Individual fittest;

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
  Serial.begin(9600);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(led, OUTPUT);
  for (byte i = 0; i < formulae.getSize(); i++)
  {
    formulae.at(i).setGene(0, getMeasurement());
  }
}

void loop()
{
  if (!foundSolution)
  {
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
  
      Serial.println("Generation: " + String(generation));
  
      Serial.println("Best Solution so far: ");
      fittest=alg.getFittest(formulae);
  
      if (alg.getFitness(fittest) < 5)
      {
        Serial.println("Exiting loop!");
        foundSolution = true;
        finalFitness=alg.getFitness(fittest);
        break;
      }
      
      Serial.print(fittest.toString());
      Serial.print("With a fitness of: ");
      Serial.println(alg.getFitness(fittest));
  
      delay(100);
  
      blink();
  
      formulae = alg.evolvePopulation(formulae);
  
      delay(100);
  
      blink();
  
      fittest=alg.getFittest(formulae);
  
      if (alg.getFitness(fittest) < 5)
      {
        Serial.println("Exiting loop!");
        foundSolution = true;
        finalFitness=alg.getFitness(fittest);
        break;
      }
  
      generation++;
    }
  
    delay(100);
  
    blink();
  
    Serial.println("\n\n\n");
  
    Serial.print("Final Solution's fitness is: ");
    Serial.println(finalFitness);
    Serial.print("And its formula is: ");
    Serial.println(fittest.toString());
  
    delay(100);
  
    blink();
  }
}

/*NOTE, good solution generated was: ((measured-159)/55) 8 times*/
