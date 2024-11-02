#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "xor.h"


neuron_lay* createResNeuron(size_t firstlen,size_t secondlen,size_t degree)
{
	size_t fullen = firstlen*secondlen;
	float* w = malloc(sizeof(float)*fullen); //weight of the hidden layer
	float* b = malloc(sizeof(float)*firstlen); //bias of the hidden layer
	float* w2 = malloc(sizeof(float)*firstlen); //weight of the output layer
	float* b2 = malloc(sizeof(float)*1); //bia of the output layer
	for (size_t i = 0; i < firstlen; i++)
	{
		for (size_t j = 0; j < secondlen; j++)
		{
			w[i*secondlen+j] = randommult(degree);
		}
		b[i] = randomrange(10)-5;
		
		w2[i] = randommult(degree);
	}
	b2[0]=randomrange(10)-5;
	//defining randomly the weight and bias of the hidden and output layer
	neuron_lay* res = malloc(sizeof(neuron_lay));
	res ->bias =b;
	/*printf("len: %u _ %u\n",firstlen, secondlen);
	for (size_t i = 0; i < firstlen; i++)
	{
		printf("b%f",b[i]);
	}
	printf("\n");
	for (size_t i = 0; i < firstlen; i++)
	{
		printf("b%f",res-> bias[i]);
	}
	printf("len: %lu _ %lu\n",firstlen, secondlen);*/
	res->weight =w;
	/*printf("len: %lu _ %lu\n",firstlen, secondlen);
	printf("\n");
	printf("\n");
	printf("len: %u _ %u\n",firstlen, secondlen);
	for (size_t i = 0; i < firstlen*secondlen; i++)
	{
		printf("w%f",w[i]);
	}
	printf("\n");
	for (size_t i = 0; i < firstlen*secondlen; i++)
	{
		printf("w%f",res->weight[i]);
	}
	printf("\n");
	printf("cre\n");*/
	res -> fweight = w2;
	res ->fbias = b2;
	//assign the list randomly defined to the result
	res -> len1 = firstlen;
	res -> len2 = secondlen;
	res -> len3 =1;
	//define the lenght of the hidend layer and the output layer
	return res;
}

neuron_lay* modifresneuron(neuron_lay* neuron,float degree)
{
	neuron_lay* res = createResNeuron(8,2,2);
	for (size_t i = 0; i < neuron->len1; i++)
	{
		for (size_t j = 0; j < neuron->len2; j++)
		{
			res->weight[neuron->len2 * i + j] = neuron->weight[neuron->len2 * i + j] * randommult(degree+1);
		}
		res->bias[i] =neuron->bias[i]*randommult(degree);
		res->fweight[i] = neuron->fweight[i]*randommult(degree);
	}
	//printneuron(res);
	res->fbias[0] = neuron->fbias[0]*randommult(degree);
	return res;
}
float xor(int* input,neuron_lay* resneuron)
{
	float output[resneuron->len1];
	for (size_t i = 0; i < resneuron -> len1; i++)
	{	
		float res =0;
		for (size_t j = 0; j < resneuron->len2; j++)
		{
			res+= resneuron->weight[j+i*resneuron->len2]*input[j];
		}
		output[i] = res+resneuron->bias[i];
	}
	float reso =0;
	for (size_t k = 0; k < resneuron->len1; k++)
	{
		reso+= resneuron->fweight[k]*output[k];
	}
	reso +=resneuron->fbias[0];
	return reso;
}
float test(neuron_lay* resneuron)
{
	int l[2] = {0,0};
	float diff = (xor(l,resneuron)-1);
	float res = diff*diff;
	l[0] = 0;
	l[1] = 1;
	diff =xor(l,resneuron);
	res += diff*diff;
	l[0] = 1;
	l[1] = 0;
	diff = xor(l,resneuron);
	res +=  diff*diff;
	
	l[0] = 1;
	l[1] = 1;
	diff = (xor(l,resneuron)-1);
	res += diff*diff;
	return res;
}
neuron_lay* train(size_t gen,size_t max_gen, neuron_lay* win,size_t numb)
{
	if (gen >= max_gen)
	{
		return win;
	}
	neuron_lay* trainer[numb];
	trainer[0] = win;
	for (size_t j = 1; j < numb; j++)
	{
		trainer[j] = modifresneuron(win,2);
	}
	size_t i=0;
	int accuracy[numb];
	while(i<numb)
	{
		accuracy[i] = test(trainer[i]);
		i+=1;
	}
	size_t k = getmin(accuracy,i);
	neuron_lay* winner = trainer[k];
	/*for (size_t l = 0; l < i; l++)
	{
		if (l!=k)
		{
			free(trainer[i]);
		}
	}*/
	return train(gen+1,max_gen,winner,numb);
}
size_t getmin(int* list,size_t k)
{
	size_t i=0;
	size_t j=0;
	while (i<k)
	{
		if (list[i]<list[j])
		{
			j=i;
		}
		i+=1;
	}
	return j;
}
float randomrange(const int max)
{
    return (float)(rand() % max);
}
float randommult(float mul)
{
	return (randomrange(1000)/1000)*mul*2-mul;
	//return (randomrange(1000)/1000)*(mul -(1/mul))+(1/mul);
}
void printneuron(neuron_lay* n)
{
	printf("\nlen = %u\nweight:\n",n->len1);
	printf("[");
	for (size_t i = 0; i < n -> len1* n->len2; i++)
	{
		printf("%f ,",n -> weight[i]);
	}
	printf("]");
	printf("\nbias:\n");
	
	printf("[");
	
	for (size_t j = 0; j < n -> len1; j++)
	{
		printf("%f ,",n -> bias[j]);
	}
	printf("]\n");
}
int main()
{
	srand(time(NULL));
	neuron_lay* best = createResNeuron(8,2,2);
	best = train(1,5000,best,15);
	int input[2]= {0,0};
	/*printf("A=? (0 or 1)\n");
	scanf("%i",&input[0]);
	printf("B=? (0 or 1)\n");
	scanf("%i",&input[1]);*/
	printneuron(best);
	printf("result 0,0: %f\n",xor(input,best));
	input[0]= 1;
	input[1]= 0;
	printf("result 1,0: %f\n",xor(input,best));
	input[0]= 0;
	input[1]= 1;
	printf("result 0,1: %f\n",xor(input,best));
	input[0]= 1;
	input[1]= 1;
	printf("result 1,1: %f\n",xor(input,best));
	//	free(input);
	
	return 0;
}
