#include <stdio.h>
#include <stdlib.h>
#include <time.h>
typedef struct neuron_lay	//neuron_lay* -> pointer to a layer of neuron
{							//neuron_lay**-> list de reseau de neuron (1 seul layer caché)
	neuron_lay* final_neuron;
	float* weight;	//tableau de len1 lists de len2 elements
	size_t len1;	//number of neuron in the layer
	size_t len2;	//number of neuron in the previous layer
	float* bias;
	float* activation;
}neuron_lay;

neuron_lay* createResNeuron(size_t firstlen,size_t secondlen,size_t degree) //crée un reseau de neurone avec des valeurs aleatoires
{
	neuron_lay* res;
	res -> len1 = firstlen;
	res -> len2 = secondlen;
	res -> final_neuron -> len1 =1;
	res -> final_neuron -> len2 = firstlen;
	res -> final_neuron -> final_neuron = NULL;
	size_t fullen = firstlen*secondlen;
	float* w = malloc(sizeof(float)*fullen);
	float* b = malloc(sizeof(float)*firstlen);
	float* w2 = malloc(sizeof(float)*firstlen);
	float* b2 = malloc(sizeof(float)*1);
	
	for (size_t i = 0; i < firstlen; i++)
	{
		for (size_t j = 0; j < secondlen; j++)
		{
			w[i*firstlen+j] = randommult(degree);
		}
		b[i] = randomrange(10)-5;
		w2[i] = randommult(degree);
	}
	b2[0]=randomrange(10)-5;
	res ->bias =b;
	res->weight =w;
	res -> final_neuron ->weight = w2;
	res ->final_neuron->bias = b2;
	
	free(w);
	free(b);
	free(w2);
	free(b2);
	return res;
}

neuron_lay* modifresneuron(neuron_lay* neuron, size_t degree)	//modifie un reseau de neurone avec un degrée d'aleatoire(+le degre est grans, +la valeur va changer)
{
	for (size_t i = 0; i < neuron->len1; i++)
	{
		for (size_t j = 0; j < neuron->len2; i++)
		{
			neuron->weight[neuron->len1 * i + j] = neuron->weight[neuron->len1 * i + j] * randommult(degree+1);
		}
		neuron->bias[i] =neuron->bias[i]*randommult(degree+1);
		neuron->final_neuron ->weight[i] = neuron->final_neuron ->weight[i]*randommult(degree+1);
	}
	neuron->final_neuron->bias[0] = neuron->final_neuron->bias[0]*randommult(degree+1);
	return neuron;
}
int xor(int A, int B,neuron_lay* resneuron)	//process l'operation xor
{
	size_t i = 0;
	int res =0;
	for (size_t i = 0; i < resneuron -> len1; i++)
	{	

	}
	
	
}
int test(neuron_lay* resneuron)	//plus le reseau de neurone correspond à ce qu'on veut, plus il retourne une valeur haute
{
	int res =0;
	if (xor(0,0,resneuron) == 1)
	{
		res++;
	}
	if (xor(1,0,resneuron) == 0)
	{
		res++;
	}
	if (xor(0,1,resneuron) == 0)
	{
		res++;
	}
	if (xor(1,1,resneuron) == 1)
	{
		res++;
	}
	return res;
}
neuron_lay** train(size_t gen,size_t max_gen, neuron_lay** win) //pred des reseau de neuron, en crée des "clones" modifiés, renvois ceux avec les meilleurs resultats
{
	if (gen == max_gen)
	{
		return win;
	}
	neuron_lay** trainer=malloc(12*sizeof(neuron_lay**));
	size_t i =0;
	while(win[i] != NULL)
	{
		for (size_t j = 0; j < 4; j++)
		{
			trainer[i*3+j] = modifresneuron(win[i],max_gen-gen);
		}
		i++;
	}
	i=0;
	int* accuracy;
	while(trainer[i]!=NULL)
	{
		accuracy[i] = test(trainer[i]);
	}
	size_t numwinner = 3;
	neuron_lay** winner=malloc(numwinner*sizeof(neuron_lay*));
	for (size_t j = 0; j < numwinner; j++)
	{
		size_t k = getmax(accuracy);
		winner[j] = trainer[k];
		trainer[k] =-1;
	}
	return train(gen+1,max_gen,winner);
}
size_t getmax(int* list)
{
	size_t i=0;
	size_t j=0;
	while (list[i]!=NULL)
	{
		if (list[i]>list[j])
		{
			j=i;
		}
	}
	return j;
}
float randomrange(const int max)
{
    return (float)(rand() % max);
}
float randommult(float mul)		//renvoie une nombre aleatoire entre mul et 1/mul
{
	return (randomrange(1000)/1000)*(mul -(1/mul))+(1/mul);
}
int main()
{
	srand(time(NULL));
	float A;
	float B;
	printf("A=? (0 or 1)\n");
	//scanf("%f",&A);
	printf("B=? (0 or 1)\n");
	//scanf("%f",&B);
	return 0;
}
