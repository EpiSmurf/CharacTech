<<<<<<< HEAD
#ifndef XOR_H
#define XOR_H
typedef struct	//neuron_lay* -> pointer to the neural network
{
	float* weight;	//weight of the hidden layer of size len1*len2
	//matrice of len1 lists of len2 elements == list of len1*len2 floats
	size_t len1;	//number of neuron in the hidden layer
	size_t len2;	//number of neuron in the input layer
	size_t len3;	//number of neuron in the output layer
	float* bias;	//bias of the hidden layer of size len1
	float* fweight;	//weight of the final layer of size len1
	float* fbias;	//bia of the final layer of size 1
}neuron_lay;

neuron_lay* createResNeuron(size_t firstlen,size_t secondlen,size_t degree);
//create a neural network with random values

neuron_lay* modifresneuron(neuron_lay* neuron,float* input,float result);
//calcul the difference between the returned values and the wanted values
//then modify the values depending of the errors
float sigmoid(float x); //the sigmoid function

float dSigmoid(float x);//the derivate of the sigmoid

float xor(float* input,neuron_lay* resneuron);
//process the xor operation with the neural network

float* half_xor(float* input,neuron_lay* resneuron);
//process the xor but stop at the hidden layer

neuron_lay* train(size_t max_gen, neuron_lay* res_neur);
//take a neural network and modify with all possible input

float randomrange(const int max);
//return a random integer, converted to float, between 0 and max

float randommult(float mul);
//return a random float between -mul and mul

void printneuron(neuron_lay* n);
//print the weight and bias of the neural network
#endif
=======
#ifndef XOR_H
#define XOR_H
typedef struct	//neuron_lay* -> pointer to the neural network
{
	float* weight;	//weight of the hidden layer of size len1*len2
	//matrice of len1 lists of len2 elements == list of len1*len2 floats
	size_t len1;	//number of neuron in the hidden layer
	size_t len2;	//number of neuron in the input layer
	size_t len3;	//number of neuron in the output layer
	float* bias;	//bias of the hidden layer of size len1
	float* fweight;	//weight of the final layer of size len1
	float* fbias;	//bia of the final layer of size 1
}neuron_lay;

neuron_lay* createResNeuron(size_t firstlen,size_t secondlen,size_t degree);
//create a neural network with random values

neuron_lay* modifresneuron(neuron_lay* neuron,float* input,float result);
//calcul the difference between the returned values and the wanted values
//then modify the values depending of the errors
float sigmoid(float x); //the sigmoid function

float dSigmoid(float x);//the derivate of the sigmoid

float xor(float* input,neuron_lay* resneuron);
//process the xor operation with the neural network

float* half_xor(float* input,neuron_lay* resneuron);
//process the xor but stop at the hidden layer

neuron_lay* train(size_t max_gen, neuron_lay* res_neur);
//take a neural network and modify with all possible input

float randomrange(const int max);
//return a random integer, converted to float, between 0 and max

float randommult(float mul);
//return a random float between -mul and mul

void printneuron(neuron_lay* n);
//print the weight and bias of the neural network
#endif
>>>>>>> b0a8fc2e8cace7371abc55b3c39fbb9682d63c88
