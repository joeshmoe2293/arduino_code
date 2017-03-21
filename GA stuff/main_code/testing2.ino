#include "Arduino.h"

using namespace std;

template <int populationSize>
class Population
{
	private:
		vector<Individual> individuals;

	public:
		Population(bool initialize)
		{
			if (initialize)
			{
				for (int i=0; i<populationSize; i++)
				{
					Individual newIndividual;
					newIndividual.generateIndividual();
					saveIndividual(i, newIndividual);
				}
			}
		}
		getIndividual(int index) : {return individuals.at(i);}
}
