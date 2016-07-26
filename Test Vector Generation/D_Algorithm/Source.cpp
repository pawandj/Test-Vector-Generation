#include"Header.h"

Combinational_Circuit *node = NULL;

void main(int argc, char *argv[])
{
	int noOfnodes = 0;
	FILE *fp = NULL;
	bool isGate = false;
	int i = 0;
	int addr = 0;
	int primaryInputCount = 0;
	int nodeCount = 0;
	
	string line;
	ifstream myfile("C:\\Users\\PDJ\\Documents\\Visual Studio 2015\\Projects\\D_Algorithm\\C17.txt");

	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			if ((line.at(0) != '*') && (line.length() > 6))
			{
				noOfnodes++;
			}
		}
		node = new Combinational_Circuit[noOfnodes + 100];
	}

	myfile.clear();
	myfile.seekg(0);

	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			string word;
			stringstream stream(line);

			if ((line.at(0) != '*') && isGate == false)
			{
				stream >> word;
				addr = atoi(word.c_str());
				node[addr].address = atoi(word.c_str());

				addressesOfNodes[nodeCount] = addr;
				nodeCount++;

				stream >> word;
				node[addr].name = word;

				stream >> word;
				node[addr].type = word;

				if (node[addr].type != "from")
				{
					stream >> word;
					node[addr].fanout = atoi(word.c_str());

					if (node[addr].fanout == 0)
					{
						node[addr].isPrimaryOutput = true;
						numberOfPrimaryOutputs++;
					}
					else
						node[addr].isPrimaryOutput = false;

					stream >> word;
					node[addr].fanin = atoi(word.c_str());

					if (node[addr].fanin == 0)
					{
						addressOfprimaryInputs[primaryInputCount] = addr;
						primaryInputCount++;
						node[addr].isPrimaryInput = true;
					}
					else
						node[addr].isPrimaryInput = false;
				}
				else
				{
					int z = 0;
					string temp = "";
					int tmp;
					stream >> word;
					node[addr].faninFrom = word;
					for (unsigned int k = 0; k < word.length(); k++)
					{
						if (isdigit(word[k]))
							temp = temp + word[k];
					}
					tmp = atoi(temp.c_str());
					node[addr].input_addr[0] = tmp;
					while (true)
					{
						if (node[tmp].outputAddress[z] == 0)
						{
							node[tmp].outputAddress[z] = addr;
							break;
						}
						z++;
					}
				}

				stream >> word;
				if (word == "sa0")
					node[addr].sa0 = true;
				else if (word == "sa1")
				{
					node[addr].sa0 = false;
					node[addr].sa1 = true;
				}
				else
				{
					node[addr].sa0 = false;
					node[addr].sa1 = false;
				}
				stream >> word;
				if (word == "sa1")
					node[addr].sa1 = true;

				if (node[addr].type == "inpt" || node[addr].type == "from")
				{
					isGate = false;
				}
				else
				{
					numberOfGates++;
					isGate = true;
				}
			}
			else if (isGate == true)
			{
				int tmp;
				int z = 0;
				for (int j = 0; j < node[addr].fanin; j++)
				{
					stream >> word;

					tmp = atoi(word.c_str());
					node[addr].input_addr[j] = tmp;

					while (true)
					{
						if (node[tmp].outputAddress[z] == 0)
						{
							node[tmp].outputAddress[z] = addr;
							break;
						}
						z++;
					}

				}
				isGate = false;
			}
		}
		myfile.close();
		numberOfPrimaryInputs = primaryInputCount;
		numberOdNodes = nodeCount;

		cout << "Total Number of Primary Inputs\t: " << numberOfPrimaryInputs << endl;
		cout << "Total Number of primary Outputs\t: " << numberOfPrimaryOutputs << endl;
		cout << "Total Number of Nodes\t: " << noOfnodes << endl;
		cout << "Total Number of Gates\t: " << numberOfGates << endl;
	}
	else
		cout << "Unable to open file";

	int maxAddr = addr;
	int counter = 0;
	int result;

	while (counter <= numberOdNodes) 
	{
		if (node[addressesOfNodes[counter]].sa0 == true) 
		{
			result = generateTestVectors(addressesOfNodes[counter], "sa0");
			if (result == 0)
			{
				displayTestVectors(addressesOfNodes[counter], "sa0");
			}
			else if (result == -2)
			{
				cout << "UNABLE TO GENERATE FAULT" << endl;
			}
			else
			{
				cout << "UNABLE TO PROPOGATE FAULT" << endl;
			}
			flushAllValues();
		}

		if (node[addressesOfNodes[counter]].sa1 == true)
		{
			result = generateTestVectors(addressesOfNodes[counter], "sa1");
			if (result == 0)
			{
				displayTestVectors(addressesOfNodes[counter], "sa1");
			}
			else if (result == -2)
			{
				cout << "UNABLE TO GENERATE FAULT" << endl;
			}
			else
			{
				cout << "UNABLE TO PROPOGATE FAULT" << endl;
			}
			flushAllValues();
		}

		counter++;
	}
	system("pause");
}

int generateTestVectors(int address, string fault)
{
	int result;

	if (fault == "sa0") {
		//node[address].value = "~D";
		result = generateValue(address, "1");
		if (result == -1)
		{
			return -2;
		}
		else if(node[address].isPrimaryOutput == false)
		{
			for (int i = 0; i < node[address].fanout; i++)
			{
				result = propogateFault(node[address].outputAddress[i], "~D", address);
				if (0 == result)
					break;
			}
		}
	} 
	else 
	{
		//node[address].value = "D";
		result = generateValue(address, "0");
		if (result == -1)
		{
			return -2;
		}
		else if (node[address].isPrimaryOutput == false)
		{
			for (int i = 0; i < node[address].fanout; i++)
			{
				result = propogateFault(node[address].outputAddress[i], "D", address);
				if (0 == result)
					break;
			}
		}
	}
	if (-1 == result)
		return -3;

	return SUCCESS;
}

/* 
generates perticular value given as arguement at the given node address
*/
int generateValue(int address, string value)
{
	int storedAddr = address;
	int result;

	if (node[address].isPrimaryInput == true)
	{
		if (node[address].value == "x" || node[address].value == value)
		{
			node[address].value = value;
			return SUCCESS;
		}
		return FAILURE;;
	}
	else if (node[address].type == "from")
	{
		return generateValue(node[address].input_addr[0], value);
	}
	else if (node[address].type == "and")
	{
		if (value == "0")
		{
			for (int i = 0; i < node[address].fanin; i++)
			{
				result = generateValue(node[address].input_addr[i], value);
				if (0 == result)
					return SUCCESS;
			}
			return FAILURE;
		}
		else
		{
			for (int i = 0; i < node[address].fanin; i++)
			{
				result = generateValue(node[address].input_addr[i], value);
				if (-1 == result)
					return FAILURE;
			}
			return SUCCESS;
		}
	}
	else if (node[address].type == "or")
	{
		if (value == "0")
		{
			for (int i = 0; i < node[address].fanin; i++)
			{
				result = generateValue(node[address].input_addr[i], value);
				if (-1 == result)
					return FAILURE;
			}
			return SUCCESS;
		}
		else
		{
			for (int i = 0; i < node[address].fanin; i++)
			{
				result = generateValue(node[address].input_addr[i], value);
				if (0 == result)
					return SUCCESS;
			}
			return FAILURE;
		}
	}
	else if (node[address].type == "nand")
	{
		if (value == "0")
		{
			for (int i = 0; i < node[address].fanin; i++)
			{
				result = generateValue(node[address].input_addr[i], "1");
				if (-1 == result)
					return FAILURE;
			}
			return SUCCESS;
		}
		else
		{
			for (int i = 0; i < node[address].fanin; i++)
			{
				result = generateValue(node[address].input_addr[i], "0");
				if (0 == result)
					return SUCCESS;
			}
			return FAILURE;
		}
	}
	else if (node[address].type == "nor")
	{
		if (value == "1")
		{
			for (int i = 0; i < node[address].fanin; i++)
			{
				result = generateValue(node[address].input_addr[i], "0");
				if (-1 == result)
					return FAILURE;
			}
			return SUCCESS;
		}
		else
		{
			for (int i = 0; i < node[address].fanin; i++)
			{
				result = generateValue(node[address].input_addr[i], "1");
				if (0 == result)
					return SUCCESS;
			}
			return FAILURE;
		}
	}
	else if (node[address].type == "xor")
	{
		if (value == "0")
		{
			int parity = 0;
			for (int i = 0; i < node[address].fanin; i++)
			{
				result = generateValue(node[address].input_addr[i], "1");
				if (-1 == result)
					result = generateValue(node[address].input_addr[i], "0");
				else
				{
					parity++;
				}
			}
			if (parity % 2 == 0)
				return SUCCESS;

			return FAILURE;
		}
		else
		{
			int parity = 0;
			for (int i = 0; i < node[address].fanin; i++)
			{
				result = generateValue(node[address].input_addr[i], "1");
				if (-1 == result)
					result = generateValue(node[address].input_addr[i], "0");
				else
				{
					parity++;
				}
			}
			if (parity % 2 == 0)
				return FAILURE;

			return SUCCESS;
		}
	}
	else if (node[address].type == "xnor")
	{
		if (value == "1")
		{
			int parity = 0;
			for (int i = 0; i < node[address].fanin; i++)
			{
				result = generateValue(node[address].input_addr[i], "1");
				if (-1 == result)
					result = generateValue(node[address].input_addr[i], "0");
				else
				{
					parity++;
				}
			}
			if (parity % 2 == 0)
				return SUCCESS;

			return FAILURE;
		}
		else
		{
			int parity = 0;
			for (int i = 0; i < node[address].fanin; i++)
			{
				result = generateValue(node[address].input_addr[i], "1");
				if (-1 == result)
					result = generateValue(node[address].input_addr[i], "0");
				else
				{
					parity++;
				}
			}
			if (parity % 2 == 0)
				return FAILURE;

			return SUCCESS;
		}
	}
	else if (node[address].type == "not")
	{
		if (value == "1")
		{
			result = generateValue(node[address].input_addr[0], "0");
			if (-1 == result)
				return FAILURE;
			return SUCCESS;
		}
		else
		{
			result = generateValue(node[address].input_addr[0], "1");
			if (-1 == result)
				return FAILURE;
			return SUCCESS;
		}
	}
	else
		return FAILURE;
}

/*
	Function is used to propogate D through the gate
	addr is the output address where value has to be propogated from inputAddress
*/
int propogateFault(int addr, string value, int inputAddress)
{
	int z = 0;
	int result;

	if (node[inputAddress].isPrimaryOutput)
	{
		return SUCCESS;
	}
	///////
	else if(node[addr].type == "from")
	{
		node[addr].value = "D";
		return propogateFault(node[addr].outputAddress[0], node[addr].value, addr);
	}
	////////
	else if (node[addr].type == "and")
	{
		node[addr].value = value;
		for (int i = 0; i < node[addr].fanin; i++)
		{
			if (node[addr].input_addr[i] != inputAddress)
			{
				result = generateValue(addr, "1");
				if (-1 == result)
				{
					return FAILURE
				}
			}
		}
		for (int i = 0; i < node[addr].fanout; i++)
		{
			result = propogateFault(node[addr].outputAddress[i], node[addr].value, addr);
			if (0 == result)
				break;
		}
		if (-1 == result)
			return FAILURE;
	}
	////////
	else if (node[addr].type == "or")
	{
		node[addr].value = value;
		for (int i = 0; i < node[addr].fanin; i++)
		{
			if (node[addr].input_addr[i] != inputAddress)
			{
				result = generateValue(addr, "0");
				if (-1 == result)
				{
					return FAILURE
				}
			}
		}
		for (int i = 0; i < node[addr].fanout; i++)
		{
			result = propogateFault(node[addr].outputAddress[i], node[addr].value, addr);
			if (0 == result)
				break;
		}
		if (-1 == result)
			return FAILURE;
	}
	////////
	else if (node[addr].type == "nand")
	{
		if (value == "D")
			node[addr].value = "~D";
		else
			node[addr].value = "D";

		for (int i = 0; i < node[addr].fanin; i++)
		{
			if (node[addr].input_addr[i] != inputAddress)
			{
				result = generateValue(node[addr].input_addr[i], "1");
				if (result == -1)
					return FAILURE;
			}
		}
		for (int i = 0; i < node[addr].fanout; i++)
		{
			result = propogateFault(node[addr].outputAddress[i], node[addr].value, addr);
			if (0 == result)
				break;
		}
		if (-1 == result)
			return FAILURE;
	}
	////////
	else if (node[addr].type == "nor")
	{
		if (value == "D")
			node[addr].value = "~D";
		else
			node[addr].value = "D";

		for (int i = 0; i < node[addr].fanin; i++)
		{
			if (node[addr].input_addr[i] != inputAddress)
			{
				result = generateValue(addr, "0");
				if (-1 == result)
				{
					return FAILURE
				}
			}
		}
		for (int i = 0; i < node[addr].fanout; i++)
		{
			result = propogateFault(node[addr].outputAddress[i], node[addr].value, addr);
			if (0 == result)
				break;
		}
		if (-1 == result)
			return FAILURE;
	}
	//////
	else if (node[addr].type == "xor")
	{

		int parity = 0;
		for (int i = 0; i < node[addr].fanin; i++)
		{
			result = generateValue(node[addr].input_addr[i], "1");
			if (0 == result)
				parity++;
			else
				result = generateValue(node[addr].input_addr[i], "0");
		}
		if (parity % 2 != 0)
			if (value == "D")
				node[addr].value = "~D";
			else
				node[addr].value = "D";

		for (int i = 0; i < node[addr].fanout; i++)
		{
			result = propogateFault(node[addr].outputAddress[i], node[addr].value, addr);
			if (0 == result)
				break;
		}
		if (-1 == result)
			return FAILURE;
	}
	///////
	else if (node[addr].type == "xnor")
	{

		int parity = 0;
		for (int i = 0; i < node[addr].fanin; i++)
		{
			result = generateValue(node[addr].input_addr[i], "1");
			if (-1 == result)
				parity++;
			else
				result = generateValue(node[addr].input_addr[i], "0");
		}
		if (parity % 2 == 0)
			if (value == "D")
				node[addr].value = "~D";
			else
				node[addr].value = "D";

		for (int i = 0; i < node[addr].fanout; i++)
		{
			result = propogateFault(node[addr].outputAddress[i], node[addr].value, addr);
			if (0 == result)
				break;
		}
		if (-1 == result)
			return FAILURE;
	}
	//////
	else if (node[addr].type == "not")
	{
		if (value == "D")
			node[addr].value = "~D";
		else
			node[addr].value = "D";

		for (int i = 0; i < node[addr].fanout; i++)
		{
			result = propogateFault(node[addr].outputAddress[i], node[addr].value, addr);
			if (0 == result)
				break;
		}
		if (-1 == result)
			return FAILURE;
	}
	return SUCCESS;
}

void displayTestVectors(int address, string fault)
{
	cout << "Test Vector to detect " << fault << " at the address " << address << " is:\t";
	for (int i = 0; i < numberOfPrimaryInputs; i++)
	{
		cout << node[addressOfprimaryInputs[i]].value;
	}
	cout << endl;
}

void flushAllValues()
{
	for (int i = 0; i < numberOdNodes; i++)
	{
		node[addressesOfNodes[i]].value = "x";
	}
}

void initialization()
{
	for (int i = 0; i < 1000; i++) {
		addressOfprimaryInputs[i] = 0;
		addressesOfNodes[i] = 0;
	}
	
}