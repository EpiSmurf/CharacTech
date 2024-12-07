<<<<<<< HEAD
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "xor.h"


neuron_lay* createResNeuron(size_t firstlen,size_t secondlen,size_t degree)
{
	//weight of the hidden layer
	float* w = malloc(sizeof(float)*firstlen*secondlen);
	//bias of the hidden layer
	float* b = malloc(sizeof(float)*firstlen);
	//weight of the output layer
	float* w2 = malloc(sizeof(float)*firstlen);
	//bia of the output layer
	float* b2 = malloc(sizeof(float)*1);
	//setting all the values randomely
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
	//define the attribute of the neural_network
	neuron_lay* res = malloc(sizeof(neuron_lay));
	res ->bias =b;
	res->weight =w;
	res -> fweight = w2;
	res ->fbias = b2;
	res -> len1 = firstlen;
	res -> len2 = secondlen;
	res -> len3 =1;
	return res;
}
neuron_lay* train(size_t max_gen, neuron_lay* res_neur)
{
	//stock all the possible input and their corrinsponding results
	float input[4][2] = {{0.0f,0.0f},{1.0f,0.0f},{0.0f,1.0f},{1.0f,1.0f}};
	float result[4] = {1.0f, 0.0f, 0.0f, 1.0f};
	//train the neural network max_gen times
	for (size_t gen = 0; gen < max_gen; gen++)
	{
		float moy_cost = 0;
		//modify the neural network with all possible input
		for (size_t i = 0; i < 4; i++)
		{
			res_neur = modifresneuron(res_neur,input[i],result[i]);
			//calcul the cost of the neural_network
			float output = xor(input[i], res_neur);
			moy_cost += (output - result[i])*(output - result[i]);
		}
		//every 10000 generation, it shows the cost
		if (gen % 10000 == 0)
		{
			moy_cost =moy_cost/4.0f;
			printf("Gen %zu : Cost moy. = %f\n", gen, moy_cost);
		}
	}
	//return the neural network trained
	return res_neur;
}

neuron_lay* modifresneuron(neuron_lay* neuron, float* input, float result)
{
	//calcul the outputs of the hidden and final layer
	float val = xor(input, neuron);
	float* half_val = half_xor(input, neuron);
	//calcul the error of the final layer
	float ferror = (val - result) * dSigmoid(val);
	//modify the bia and weights of the final layer
	neuron->fbias[0] -= ferror * 0.1;
	for (size_t i = 0; i < neuron->len1; i++)
	{
		neuron->fweight[i] -= half_val[i] * ferror * 0.1;
	}
	for (size_t i = 0; i < neuron->len1; i++)
	{
		//calcul the error of the hidden layer
		float error = ferror*neuron->fweight[i]*dSigmoid(half_val[i]);
		//modify the bias and output of the hidden layer
		neuron->bias[i] -= error * 0.1;
		for (size_t j = 0; j < neuron->len2; j++)
		{
			neuron->weight[j+i*neuron->len2]-=input[j]*error*0.1;
		}
	}
	free(half_val);
	return neuron;
}


float xor(float* input,neuron_lay* resneuron)
{
	//create the output list of the hidden layer
	float* output=malloc(sizeof(float)*resneuron->len1);
	for (size_t i = 0; i < resneuron -> len1; i++)
	{
		//calcul the ouputs of the hidden layer
		float res =0;
		for (size_t j = 0; j < resneuron->len2; j++)
		{
			res+= resneuron->weight[j+i*resneuron->len2]*input[j];
		}
		output[i] = sigmoid(res+resneuron->bias[i]);
	}
	//calcul the output of the final layer
	float reso =0;
	for (size_t k = 0; k < resneuron->len1; k++)
	{
		reso+= resneuron->fweight[k]*output[k];
	}
	reso +=resneuron->fbias[0];
	free(output);
	return sigmoid(reso);
}
float* half_xor(float* input,neuron_lay* resneuron)
{
	//create the output list of the hidden layer
	float* output=malloc(sizeof(float)*resneuron->len1);
	for (size_t i = 0; i < resneuron -> len1; i++)
	{
		//calcul the outputs of the hidden layer
		float res =0;
		for (size_t j = 0; j < resneuron->len2; j++)
		{
			res+= resneuron->weight[j+i*resneuron->len2]*input[j];
		}
		output[i] = sigmoid(res+resneuron->bias[i]);
	}
	return output;
}

float sigmoid(float x)
{ 
	return 1/(1 + exp(-x));
}
float dSigmoid(float x)
{
	return sigmoid(x)*(1-sigmoid(x));
}
float randomrange(const int max)
{
	return (float)(rand() % max);
}
float randommult(float mul)
{
	return (randomrange(1000)/1000)*mul*2-mul;
}
void printneuron(neuron_lay* n)
{
	//print the weight of the hidden layer
	printf("\nhidden weight:\n[");
	for (size_t i = 0; i < n -> len1* n->len2; i++)
	{
		printf("%f ,",n -> weight[i]);
	}
	//print the bias of the hidden layer
	printf("]\nhidden bias:\n[");
	for (size_t j = 0; j < n -> len1; j++)
	{
		printf("%f ,",n -> bias[j]);
	}
	//print the weight of the hidden layer
	printf("]\n\nfinal weight:\n[");
	for (size_t i = 0; i < n -> len1; i++)
	{
		printf("%f ,",n -> fweight[i]);
	}
	//print the bia of the final layer
	printf("]\n final bia:\n[%f]\n\n",n -> fbias[0]);

}
int main()
{
	//start the random function
	srand(time(NULL));
	//create a neural network and train it
	neuron_lay* best = createResNeuron(10,2,2);
	best = train(80000,best);
	float input[2]= {0,0};
	//print the weight, bias and output of the neural network
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
	
	return 0;
}
=======
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "xor.h"


neuron_lay* createResNeuron(size_t firstlen,size_t secondlen,size_t degree)
{
	//weight of the hidden layer
	float* w = malloc(sizeof(float)*firstlen*secondlen);
	//bias of the hidden layer
	float* b = malloc(sizeof(float)*firstlen);
	//weight of the output layer
	float* w2 = malloc(sizeof(float)*firstlen);
	//bia of the output layer
	float* b2 = malloc(sizeof(float)*1);
	//setting all the values randomely
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
	//define the attribute of the neural_network
	neuron_lay* res = malloc(sizeof(neuron_lay));
	res ->bias =b;
	res->weight =w;
	res -> fweight = w2;
	res ->fbias = b2;
	res -> len1 = firstlen;
	res -> len2 = secondlen;
	res -> len3 =1;
	return res;
}
neuron_lay* train(size_t max_gen, neuron_lay* res_neur)
{
	//stock all the possible input and their corrinsponding results
	float input[4][2] = {{0.0f,0.0f},{1.0f,0.0f},{0.0f,1.0f},{1.0f,1.0f}};
	float result[4] = {1.0f, 0.0f, 0.0f, 1.0f};
	//train the neural network max_gen times
	for (size_t gen = 0; gen < max_gen; gen++)
	{
		float moy_cost = 0;
		//modify the neural network with all possible input
		for (size_t i = 0; i < 4; i++)
		{
			res_neur = modifresneuron(res_neur,input[i],result[i]);
			//calcul the cost of the neural_network
			float output = xor(input[i], res_neur);
			moy_cost += (output - result[i])*(output - result[i]);
		}
		//every 10000 generation, it shows the cost
		if (gen % 10000 == 0)
		{
			moy_cost =moy_cost/4.0f;
			printf("Gen %zu : Cost moy. = %f\n", gen, moy_cost);
		}
	}
	//return the neural network trained
	return res_neur;
}

neuron_lay* modifresneuron(neuron_lay* neuron, float* input, float result)
{
	//calcul the outputs of the hidden and final layer
	float val = xor(input, neuron);
	float* half_val = half_xor(input, neuron);
	//calcul the error of the final layer
	float ferror = (val - result) * dSigmoid(val);
	//modify the bia and weights of the final layer
	neuron->fbias[0] -= ferror * 0.1;
	for (size_t i = 0; i < neuron->len1; i++)
	{
		neuron->fweight[i] -= half_val[i] * ferror * 0.1;
	}
	for (size_t i = 0; i < neuron->len1; i++)
	{
		//calcul the error of the hidden layer
		float error = ferror*neuron->fweight[i]*dSigmoid(half_val[i]);
		//modify the bias and output of the hidden layer
		neuron->bias[i] -= error * 0.1;
		for (size_t j = 0; j < neuron->len2; j++)
		{
			neuron->weight[j+i*neuron->len2]-=input[j]*error*0.1;
		}
	}
	free(half_val);
	return neuron;
}


float xor(float* input,neuron_lay* resneuron)
{
	//create the output list of the hidden layer
	float* output=malloc(sizeof(float)*resneuron->len1);
	for (size_t i = 0; i < resneuron -> len1; i++)
	{
		//calcul the ouputs of the hidden layer
		float res =0;
		for (size_t j = 0; j < resneuron->len2; j++)
		{
			res+= resneuron->weight[j+i*resneuron->len2]*input[j];
		}
		output[i] = sigmoid(res+resneuron->bias[i]);
	}
	//calcul the output of the final layer
	float reso =0;
	for (size_t k = 0; k < resneuron->len1; k++)
	{
		reso+= resneuron->fweight[k]*output[k];
	}
	reso +=resneuron->fbias[0];
	free(output);
	return sigmoid(reso);
}
float* half_xor(float* input,neuron_lay* resneuron)
{
	//create the output list of the hidden layer
	float* output=malloc(sizeof(float)*resneuron->len1);
	for (size_t i = 0; i < resneuron -> len1; i++)
	{
		//calcul the outputs of the hidden layer
		float res =0;
		for (size_t j = 0; j < resneuron->len2; j++)
		{
			res+= resneuron->weight[j+i*resneuron->len2]*input[j];
		}
		output[i] = sigmoid(res+resneuron->bias[i]);
	}
	return output;
}

float sigmoid(float x)
{ 
	return 1/(1 + exp(-x));
}
float dSigmoid(float x)
{
	return sigmoid(x)*(1-sigmoid(x));
}
float randomrange(const int max)
{
	return (float)(rand() % max);
}
float randommult(float mul)
{
	return (randomrange(1000)/1000)*mul*2-mul;
}
void printneuron(neuron_lay* n)
{
	//print the weight of the hidden layer
	printf("\nhidden weight:\n[");
	for (size_t i = 0; i < n -> len1* n->len2; i++)
	{
		printf("%f ,",n -> weight[i]);
	}
	//print the bias of the hidden layer
	printf("]\nhidden bias:\n[");
	for (size_t j = 0; j < n -> len1; j++)
	{
		printf("%f ,",n -> bias[j]);
	}
	//print the weight of the hidden layer
	printf("]\n\nfinal weight:\n[");
	for (size_t i = 0; i < n -> len1; i++)
	{
		printf("%f ,",n -> fweight[i]);
	}
	//print the bia of the final layer
	printf("]\n final bia:\n[%f]\n\n",n -> fbias[0]);

}
int main()
{
	//start the random function
	srand(time(NULL));
	//create a neural network and train it
	neuron_lay* best = createResNeuron(10,2,2);
	best = train(80000,best);
	float input[2]= {0,0};
	//print the weight, bias and output of the neural network
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
	
	return 0;
}
>>>>>>> b0a8fc2e8cace7371abc55b3c39fbb9682d63c88
