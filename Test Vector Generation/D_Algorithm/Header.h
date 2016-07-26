#include<iostream>
#include<fstream>
#include<string>
#include <sstream>  

using namespace std;

#define	_input	0;
#define _and	1;
#define	_nand	2;
#define _or		3;
#define _nor	4;
#define	_xor	5;
#define _xnor	6;
#define _buffer	7;
#define _not	8;
#define _from	9;

#define MAX_FANIN 5;
#define FAILURE -1;
#define SUCCESS 0;

struct Combinational_Circuit
{
	int address = 0;
	int outputAddress[5] = {};
	string name;
	string type;
	string faninFrom = "";
	int fanout = 0;
	int fanin = 0;
	bool sa0 = false;
	bool sa1 = false;
	bool isPrimaryOutput = false;
	bool isPrimaryInput = false;
	int input_addr[5] = {0};
	string value = "x";
};

int addressOfprimaryInputs[1000] = {0};
int numberOfPrimaryInputs;

int numberOfPrimaryOutputs = 0;
int numberOfGates = 0;

int numberOdNodes;
int addressesOfNodes[1000] = {0};


int generateTestVectors(int address, string fault);
int generateValue(int address, string value);
int propogateFault(int addr, string value, int inputAddress);
void displayTestVectors(int address, string fault);
void flushAllValues();


