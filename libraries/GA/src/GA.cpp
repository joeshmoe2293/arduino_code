#include "Arduino.h"
#include "GA.h"

Population::Population(bool initialize)
{
	if (initialize)
	{
		using namespace std; // MAKE SURE THAT THIS IS THE RIGHT FUNCTION TO BE USING...
		for (int i=0; i<(populationSize); i++)
		{
			Individual newIndividual();
			newIndividual.generateIndividual();
			saveIndividual(i, newIndividual);	
		}
	}
}

Individual Population::getIndividual(int index)
{
	return individuals[index];
}

Individual Population::getFittest(int index)
{
	// ADD STUFF HERE LATER
}

int Population::size()
{
	using namespace std;
	return constexpr size_t(individuals);
	// return sizeof(individuals); // CHECK TO SEE IF NEED TO USE SIZE_T
}

void Population::saveIndividual(int index, Individual indiv)
{
	individuals[index]=indiv;
}

void Individual::generateIndividual()
{
	for (int i=0; i<size(); i++)
	{
		byte gene; //ADD STUFF HERE LATER FOR GENERATING RANDOM VALUES
		genes[i]=gene;
	}
}

static void Individual::setDefaultGeneLength(int length)
{
	defaultGeneLength=length;
}

byte Individual::getGene(int index)
{
	return genes[index];
}

void Individual::setGene(int index, byte value)
{
	genes[index]=value;
}

int Individual::size()
{
	using namespace std; // CHECK THIS!!
	return constexpr size_t size(genes);
	// NOT SURE IF THIS IS RIGHT^^
}

static Individual Algorithm::crossover(Individual indiv1, Individual indiv2)
{
	// ADD STUFF HERE LATER
}

static void Algorithm::mutate(Individual indiv)
{
	// ADD STUFF HERE LATER
}

static Individual Algorithm::tournamentSelection(Population pop)
{
	// ADD STUFF HERE LATER
}

static Population Algorithm::evolvePopulation(Population pop)
{
	// ADD STUFF HERE LATER
}

static void FitnessCalc::setSolution(byte newSolution[])
{
	solution=newSolution;
}

static void FitnessCalc::setSolution(String newSolution)
{
	// ADD STUFF HERE LATER
}

static void FitnessCalc::getFitness(Individual individual)
{
	// ADD STUFF HERE LATER
}

static int FitnessCalc::getFitness(Individual individual)
{
	// ADD STUFF HERE LATER
}

// The code below might be a bit obsolete...
/*
static int FitnessCalc::getMaxFitness()
{
	byte maxFitness=0;
	for (int i=0; i<size(); i++)
	{
		if (getFitness(i)<maxFitness)
		{
			maxFitness=getFitness(i);
		}
	}
}*/
