#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <cctype>
#include <algorithm>
#include <vector>
#include <set>

using namespace std;

static enum Operators {
	add, subtract, multiply
};

struct storeOperation {
	string firstRegister;
	string whatOperator;
	string secondRegister;
};

//Map for switch statement to work
static std::map<std::string, Operators> stringToOperatorMap;

//Function declaration
static void Initialize();
string toLower(static string fixString);
bool isStringInt(const string& s);
static int evaluate(vector<storeOperation>& allOperation, string& whatRegister);
static int evaluate(vector<storeOperation>& allOperation, string& whatRegister, set<string>& dependencies, map<string, int>& mapOfValues);


int main(int argc, char** argv)
{
	istream* inputFromStream;
	ifstream inputFile;
	bool isFile = false;
	int NOlines = 0;

	if (argc > 1) {
		//read from the file
		inputFile.open(argv[1]);
		if (!inputFile) {
			cout << "File not found \n";
			return 0;
		}
		inputFromStream = &inputFile;
		isFile = true;
		cout << "Read from file \n";
	}
	else {
		cout << "Enter commands here: \n";
		inputFromStream = &cin;
	}

	vector<storeOperation> allOperations; // Store all operations here
	string lineFromStream;

	Initialize();

	//Read input
	while (getline(*inputFromStream, lineFromStream)) {

		string registerInput;
		string operatorSign;
		string registerOrValue;
		istringstream ss(lineFromStream);
		string word;
		int i = 0;
		bool print = false;

		while (ss >> word) 
		{ 
			//Register 1
			if (i == 0) { registerInput = toLower(word); }
			if (registerInput == "quit") { return 0; }
			
			//If print- the second input is a register
			if (registerInput == "print") {
				print = true;
				if (i == 1) { operatorSign = toLower(word); }
			}

			//Operator
			if (i == 1 && print == false) { operatorSign = toLower(word); }

			//Register 2
			if (i == 2) { registerOrValue = toLower(word); }
			i++;
		}

		//Evaluate if print is true
		if (print == true) {
			int res = evaluate(allOperations, operatorSign );
			cout << res << "\n";
		}
		print = false;

		//Add to the array containing all operations 
		storeOperation tmp;
		tmp.firstRegister = registerInput;
		tmp.whatOperator = operatorSign;
		tmp.secondRegister = registerOrValue;
		allOperations.push_back(tmp);
	}
	
	if (isFile == true) inputFile.close();
	return 0;
}

void Initialize() {
	stringToOperatorMap["add"] = add;
	stringToOperatorMap["subtract"] = subtract;
	stringToOperatorMap["multiply"] = multiply;
}

static int evaluate(vector<storeOperation>& allOperation, string& whatRegister)
{
	set<string> dependencies;
	map<string, int> mapOfValues;
	return evaluate(allOperation, whatRegister, dependencies, mapOfValues );
}

static int evaluate(vector<storeOperation>& allOperation, string& whatRegister, set<string>& dependencies, map<string, int>& mapOfValues)
{
	int finalValue = 0;

	//If reg is evaluated return its int (makes assumption that a reg is not a int)
	if (isStringInt(whatRegister) == true) {
		int result = std::stoi(whatRegister);
		return result;
	}

	//If evaluated- just return the value;
	if (mapOfValues.size() != 0 && mapOfValues.find(whatRegister) != mapOfValues.end()) {
		auto it = mapOfValues.find(whatRegister);
		return it->second;
	}

	//Print if cycle
	if (dependencies.size() != 0 && dependencies.find(whatRegister) != dependencies.end()) {
		cout << "Cycle found \n";
		return 0;
	}

	dependencies.insert(whatRegister);

	for (int i = 0; i < allOperation.size(); i++) {

		string theOperator = allOperation[i].whatOperator;
		string reg1 = allOperation[i].firstRegister;
		string reg2 = allOperation[i].secondRegister;
		if (reg1 != whatRegister) { continue; }

		auto it2 = stringToOperatorMap.find(theOperator);
		if (it2 == stringToOperatorMap.end()) { cout << "invalid operator \n";  continue; }

		switch (stringToOperatorMap[theOperator]) {
		case add:
			finalValue += evaluate(allOperation, reg2, dependencies, mapOfValues);
			break;

		case subtract:
			finalValue -= evaluate(allOperation, reg2, dependencies, mapOfValues);
			break;

		case multiply:
			finalValue *= evaluate(allOperation, reg2, dependencies, mapOfValues);
			break;

		default:
			cout << "'" << theOperator << " is an invalid operation" << endl;
			break;
		}

		//Update value of register
		auto it = mapOfValues.find(whatRegister);

		if (it == mapOfValues.end()) {
			mapOfValues.insert({ whatRegister, finalValue });
		}
		else {
			it->second = finalValue;
		}
	}
	return finalValue;
}

bool isStringInt(const string& a)
{
	std::string::const_iterator it = a.begin();
	while (it != a.end() && std::isdigit(*it)) ++it;
	return !a.empty() && it == a.end();
}

string toLower(static string fixString) {
	std::transform(fixString.begin(), fixString.end(), fixString.begin(), ::tolower);
	return fixString;
}
