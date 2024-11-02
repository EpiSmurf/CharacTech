#ifndef XOR_H
#define XOR_H
typedef struct	//neuron_lay* -> pointer to a layer of neuron
{							//neuron_lay**-> list of resonals network(1 seul layer caché)
	float* weight;	//matrice of len1 lists of len2 elements == list of len1*len2 floats
	size_t len1;	//number of neuron in the hidden layer
	size_t len2;	//number of neuron in the input layer
    size_t len3;    //number of neuron in the output layer
	float* bias;    //list of bias of size len1
	float* fweight; 
    float* fbias;
}neuron_lay;

neuron_lay* createResNeuron(size_t firstlen,size_t secondlen,size_t degree);
//create a neural network with random values

neuron_lay* modifresneuron(neuron_lay* neuron, float degree);
//modify a neural network with a degree of randomness
//more it is big more the value will change

float xor(int* input,neuron_lay* resneuron);
//process the xor operation with the neural network

float test(neuron_lay* resneuron);
//compare the values returned by the neural network with the values that should be returned
//plus le reseau de neurone correspond à ce qu'on veut, plus il retourne une valeur haute

neuron_lay* train(size_t gen,size_t max_gen, neuron_lay* win,size_t numb);
//pred des reseau de neuron, en crée des "clones" modifiés, renvois ceux avec les meilleurs resultats

size_t getmin(int* list,size_t k);
//return the index of the minimum of a list of int

float randomrange(const int max);
//return a random number between 0 and max

float randommult(float mul);
//renvoie une nombre aleatoire entre mul et 1/mul

void printneuron(neuron_lay* n);
#endif