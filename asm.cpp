//Animesh Kumar Sinha
//Roll No: 2001CS07

#include<bits/stdc++.h>
using namespace std;

struct table{
// This table will store ASM code in the given syntax

// [Label, Mnemonic, Operand, Operand Type(Hex, Oct, ...)]

    string label, mnemonic, operand;
	int oprType;
	bool labelPresent;
};

vector<table> data;						//Data vector in Table form
map<string, pair<string, int>> OPC;		//Stores mnemonic and OPCode
vector<pair<int, string>> errors;		//Stores all the errors
vector<string> cleaned;					//Contains the code line in clean form w/o spaces
map<string, int> labels;				//Stores the labels and their declaration line
vector<pair<int, string>> machineCode;	//Stores the machine code and extra info for generating listCode
vector<int> programCounter;				//This vector maintains Program counter on every line
bool haltPresent = false;				// Check if HALT is present in the Code
string file_name;						// Name of the file
bool exit_flag;                         //Check whether to exit the program


// Helper functions
bool isOctal(string s);
bool isHexadecimal(string s);
bool isDecimal(string s);
bool validName(string cur);
//Function Prototyping

string decToHex(int number, int add = 24)
{
// This functions converts decimal number to Hexadecimal
	if(add == 32){
		unsigned int num = number;
		stringstream ss;
    	ss << hex << num;
    	return ss.str();
	}
	if(number < 0)
		number += (1 << add);
	stringstream ss;
    ss << hex << number;
    return ss.str();
}

void init()
{
//Initialization of the Mnemonic, OPCode
// 0 = Takes no operands like add
// 1 = Takes 1 operand and is value, Ex: ldc 5
// 2 = Takes 1 operand and is offset, Ex: call return
// No OPCODE for data and SET
//SET is pseudo instruction
    OPC["data"] = {"", 1};
    OPC["ldc"] = {"00", 1};
    OPC["adc"] = {"01", 1};
    OPC["ldl"] = {"02", 2};
    OPC["stl"] = {"03", 2};
    OPC["ldnl"] = {"04", 2};
    OPC["stnl"] = {"05", 2};
    OPC["add"] = {"06", 0};
    OPC["sub"] = {"07", 0};
    OPC["shl"] = {"08", 0};
    OPC["shr"] = {"09", 0};
    OPC["adj"] = {"0A" , 1};
    OPC["a2sp"] = {"0B" , 0};
    OPC["sp2a"] = {"0C" , 0};
    OPC["call"] = {"0D" , 2};
    OPC["return"] = {"0E" , 0};
    OPC["brz"] = {"0F" , 2};
    OPC["brlz"] = {"10" , 2};
    OPC["br"] = {"11", 2};
    OPC["HALT"] = {"12", 0};
    OPC["SET"] = {"" , 1};

    return;
}

void genErrors(int line, string type)
{
// This functions stores the generated errors in vector errors
	errors.push_back({line + 1, "Error at line: " + to_string(line) + " -> Type: " + type});
}

string clean(string s, int line)
{
// This function removes extra spaces to produce a uniform cleaned file
	for(int i = 0; i < 2; ++i)
    {
		reverse(s.begin(), s.end());
		while(s.back() == ' ' || s.back() == '\t')
		{
			s.pop_back();
		}
	}

	string temp = "";
	int j, spaces=0;
	for(int i = 0; i < (int)s.size(); ++i)
    {
		if(s[i] == ';')
            break;

		if((s[i] != ' ' && s[i] != '\t') || (s[i] == ':'))
        {
			temp += s[i];
			if(s[i] == ':')
            {
                if(i == (int)s.size() - 1 || s[i + 1] != ' ')
                    temp += " ";
            }
			continue;
		}

		temp += " ";
		j = i;
		while(s[i] == s[j] && j < (int)s.size())
            j++;
		i = j - 1;
	}

	while(!temp.empty() && (temp.back() == ' ' || temp.back() == '\t'))
		temp.pop_back();

	for(auto to: temp)
		spaces += (to == ' ');
	if(spaces > 2)
		genErrors(line + 1, "Invalid syntax");
	return temp;
}

void pushSETinstructions(vector<string> &temp, string token, string s, int j)
{
// Following mnemonic are used in implementing SET mnemonic in assembler
	if(s.size() <= j + 5){
		return;
	}
	temp.push_back("adj 10000");			//stack pointer at random memory
	temp.push_back("stl -1");				//Here to load A
	temp.push_back("stl 0");				//Here B
	temp.push_back("ldc " + s.substr(j + 6, s.size() - (j + 6)));
	temp.push_back("ldc " + token.substr(0, j));
	temp.push_back("stnl 0");				//Load A, B
	temp.push_back("ldl 0");
	temp.push_back("ldl -1");
	temp.push_back("adj -10000");			// Adjust stack pointer
}

void implementSET()
{
// This function implements SET mnemonic given.
// Since SET is pseudo instruction, we implement it with other mnemonics
	vector<string> temp;
	for(int i = 0; i < (int) cleaned.size(); ++i)
    {
		string curr;
		bool state = false;
		for(int j = 0; j < (int) cleaned[i].size(); ++j)
		{
			curr += cleaned[i][j];
			if(cleaned[i][j] == ':')
			{
				curr.pop_back();
				if(cleaned[i].size() > j + 5 && cleaned[i].substr(j + 2, 3) == "SET")
				{
					state = true;
					if(abs(labels[curr]) == i)
					{
						labels[curr] = (int)temp.size() - 1;
						temp.push_back(cleaned[i].substr(0, j + 1) + " data " + cleaned[i].substr(j + 6, (int)cleaned[i].size() - (j + 6)));
					}
					else
					{
						pushSETinstructions(temp, curr, cleaned[i], j);
					}
					break;
				}
			}
		}
		if(!state and !cleaned[i].empty())
			temp.push_back(cleaned[i]);
	}
	cleaned = temp;
	return;
}


void processLabel()
{
// This function processes labels and stores the labels map, with their corresponding positions (where they are declared)
	for(int i = 0; i < (int) cleaned.size(); ++i)
    {
		string curr;
		for(int j = 0; j < (int) cleaned[i].size(); ++j)
		{
			if(cleaned[i][j] == ':')
			{
				bool is_flag = validName(curr);
				if(!is_flag)
				{
					genErrors(i + 1, "Invalid label name");
					break;
				}

				if(labels.find(curr) != labels.end())
                {
					if(cleaned[i].size() > j + 4 && cleaned[i].substr(j + 2, 3) == "SET")
                    {
						continue;
					}
					if(cleaned[i].size() > j + 5 && cleaned[i].substr(j + 2, 4) == "data" && labels[curr] < 0)
					{
						labels[curr] = i;
						continue;
					}

					genErrors(i + 1, "Multiple declarations of label: " + curr);
				}

				if(cleaned[i].size() > j + 4 && cleaned[i].substr(j + 2, 3) == "SET")
                {
					labels[curr] = -i;
					continue;
				}

				labels[curr] = i;
				break;
			}
			curr += cleaned[i][j];
		}
	}
}

void fillData(int i, string one, string two, string three, int type)
{
// Fills the data vector to reduce code size and make it cleaner
	data[i].label = one;
	data[i].mnemonic = two;
	data[i].operand = three;
	data[i].oprType = type;
}

int calType(string s)
{
// This function returns whether the operand is label/Hex value/Decimal value/Octal value
	if(s.empty())
        return 0;
	if(s[0] == '+' || s[0] == '-')
	{
		reverse(s.begin(), s.end());
		s.pop_back();
		reverse(s.begin(), s.end());
	}
	if(s.empty())
		return -1;
	else if(isDecimal(s))
        return 10;
	else if(isOctal(s))
        return 8;
	else if(isHexadecimal(s))
        return 16;
	else if(validName(s))
        return 1;

	return -1;
}

void tableForm()
{
// This function processes the data as:
// [Label, Mnemonic, Operand, Operand Type(Hex, Oct, ...)]
// Stores the data in above form in table vector
	int pc = 0;
	for(int i = 0; i < (int) cleaned.size(); ++i)
    {
		string ans[10] = {"", "", "", ""};
		string curr = "";
		int ptr = 1;
		for(int j = 0; j < (int) cleaned[i].size(); ++j)
        {
			if(cleaned[i][j] == ':')
			{
				ans[0] = curr;
				curr = "";
				j++;
				continue;
			}
			else if(cleaned[i][j] == ' ')
			{
				ans[ptr++] = curr;
				curr = "";
				continue;
			}

			curr += cleaned[i][j];
			if(j == (int)cleaned[i].size() - 1)
				ans[ptr++] = curr;
		}

		if(!ans[1].empty())
        {
			data[i].labelPresent = true;
		}
		else
		{
			data[i].labelPresent = false;
		}

		if(ans[1] == "HALT")
			haltPresent = true;
		if(!ans[0].empty())
			labels[ans[0]] = pc;
		programCounter[i] = pc;
		if(ptr == 1){
			fillData(i, ans[0], "", "", 0);
			continue;
		}
		pc++;
		if(!OPC.count(ans[1])){
			genErrors(i + 1, "Invalid Mnemonic");
			continue;
		}
		if(min(OPC[ans[1]].second, 1) != ptr - 2){
			genErrors(i + 1, "Invalid OPCode-Syntax combination");
			continue;
		}
		fillData(i, ans[0], ans[1], ans[2], calType(ans[2]));
		if(data[i].oprType == 1 and !labels.count(data[i].operand))
        {
			genErrors(i + 1, "No such label or data variable");
		}
		else if(data[i].oprType == -1)
		{
			genErrors(i + 1, "Invalid operand type");
		}
	}
}

void makeDataSegment()
{
// This function, separates the code in DATA segment for proper execution of instructions
	vector<string> dataseg, instr;
	for(int i = 0; i < (int)cleaned.size(); ++i)
    {
		bool state = false;
		for(int j = 0; j < cleaned[i].size(); ++j)
		{
			bool flag = false;
			if((cleaned[i].substr(j, 4) == "data" and j + 4 < cleaned[i].size())
                || (cleaned[i].back() == ':' and i + 1 < (int)cleaned.size() and cleaned[i + 1].substr(0, 4) == "data"))
			{
				flag = true;
			}

			if(flag)
			{
				dataseg.push_back(cleaned[i]);
				state = true;
				break;
			}
		}

		if(!state)
			instr.push_back(cleaned[i]);
	}
	instr.insert(instr.end(), dataseg.begin(), dataseg.end());
	cleaned = instr;
}

void secondPass();
void writeToFile();

bool noErrors()
{
// Stores the errors or warnings in the file: logFile.log
	ofstream outErrors("logFile.log");
	outErrors << "Log code generated in: logFile.log" << endl;
	if(errors.empty())
    {
		cout << "No errors found!" << endl;
		if(haltPresent == false)
		{
			cout << "1 warning detected" << endl;
			outErrors << "Warning: HALT not present!" << endl;
		}
		outErrors << "Machine code generated in: machineCode.o" << endl;
		outErrors << "Listing code generated in: listCode.l" << endl;
		outErrors.close();
		return true;
	}
	sort(errors.begin(), errors.end());
	cout << (int)errors.size() << " errors encountered! See logFile.log" << endl;
	for(auto to: errors){
		outErrors << to.second << endl;
	}
	outErrors.close();
	return false;
}

void firstPass()
{
// First pass of assembler which uses the functions declared above

	ifstream infile;
	infile.open(file_name);
	if(infile.fail()){
		cout << "Input file doesn't exist in the given directory!" << endl;

		exit_flag = true;
		return;
	}

	string s, curr;
	while(getline(infile, s))
    {
		int cln_sz = cleaned.size();
		curr = clean(s, cln_sz);
		cleaned.push_back(curr);
	}

	processLabel();					// Process labels in the code Ex: var1:
	if(errors.empty())
		implementSET();				// Implementing SET mnemonic
	data.resize((int) cleaned.size()); //Allocates memory for table
	programCounter.resize((int) cleaned.size());	//Allocates memory for programCounter array
	makeDataSegment();						//Separates the code in data segment
	tableForm();						// Makes the code in table form as states in the problem

	exit_flag = !noErrors();
	if(!exit_flag)
    {
		secondPass();
		writeToFile();
    }
    return;
}

string appZero(string s, int sz = 6)
{
// Utility function to append zero(s) at the end to make its size 24 bit
//Example: A changes to 00000A

	//int sz = 6;
	reverse(s.begin(), s.end());
	while((int) s.size() < sz)
		s += '0';
	reverse(s.begin(), s.end());
	return s;
}

void secondPass()
{
// Second pass of assembler
// Converts the code to machine code and also generates Listing file
	for(int i = 0; i < (int) data.size(); i++)
    {
		if(cleaned[i].empty())
			continue;

		string location = appZero(decToHex(programCounter[i]));
		if(data[i].mnemonic == "")
        {
			string curMacCode = "        ";
			machineCode.push_back({i, curMacCode});
			continue;
		}

		if(data[i].oprType == 0)
		{
			string curMacCode = "000000" + OPC[data[i].mnemonic].first;
			machineCode.push_back({i, curMacCode});
		}
		else if(data[i].oprType == 1)
		{
			int decForm;
			if(OPC[data[i].mnemonic].second == 2)
			{
				int val = labels[data[i].operand];
				decForm = val - (programCounter[i] + 1);
			}
			else
			{
				decForm = labels[data[i].operand];
			}
			string curMacCode = appZero(decToHex(decForm)) + OPC[data[i].mnemonic].first;
			machineCode.push_back({i, curMacCode});
		}
		else
		{
			int sz = 6, add = 24;
			if(data[i].mnemonic == "data")
				sz = 8, add = 32;
			int decForm = stoi(data[i].operand, 0, data[i].oprType);
			string curMacCode = appZero(decToHex(decForm, add), sz) + OPC[data[i].mnemonic].first;
			machineCode.push_back({i, curMacCode});
		}
	}
}

void writeToFile()
{
// This function writes machine code and to the file:
// 1. Listing Code: listCode.l
// 2. Machine code: machineCode.o
	ofstream outList("listCode.l");
	for(auto to: machineCode)
    {
		outList << appZero(decToHex(programCounter[to.first])) << " " << to.second << " " << cleaned[to.first] << endl;
	}
	outList.close();
	ofstream outMachineCode;
	outMachineCode.open("machineCode.o",ios::binary | ios::out);

	for(auto to : machineCode)
	{
		unsigned int x;
		if(to.second.empty() or to.second == "        ")
			continue;
    	stringstream ss;
    	ss << hex << to.second;
    	ss >> x; // output it as a signed type
     	static_cast<int>(x);
 		outMachineCode.write((const char*)&x, sizeof(unsigned int));
	}
	outMachineCode.close();
	cout << "Log code generated in: logFile.log" << endl;
	cout << "Machine code generated in: machineCode.o" << endl;
	cout << "Listing code generated in: listCode.l" << endl;
}

int main(int argc, char* argv[])
{
	// Calling required functions
	init();							// Initializing the mnemonics
	exit_flag=false;

	file_name = argv[1];

	firstPass();
	if(exit_flag)
        return 0;

	system("pause");
	return 0;
}

bool isOctal(string s)
{
// Funtion to check whether the string is Octal or not
    if((int) s.size() < 2)
    	return false;
    bool ok = true;
    for(int i = 1; i < (int)s.size(); ++i)
    {
        ok &= (s[i] >= '0' && s[i] <= '7');
    }
    return ok & (s[0] == '0');
}
bool isHexadecimal(string s)
{
// Funtion to check whether the string is in HexaDecimal form or not
    bool state = true ;
    if((int)s.size() < 3)
    	return false;
    state &= (s[0] == '0') & ((s[1] == 'x' or s[1] == 'X'));
    for(int i = 2; i < (int) s.size(); i++)
    {
        bool st = (s[i] >= '0' && s[i] <= '9');
        st |= ((s[i] >= 'a' && s[i] <= 'f') || ((s[i] >= 'A' && s[i] <= 'F')));
        state &= st;
    }
    return state;
}
bool isDecimal(string s)
{
// Funtion to check whether the string is in decimal form or not
	bool ok = true;
	for(int i = 0; i < (int)s.size(); ++i)
		ok &= (s[i] >= '0' and s[i] <= '9');
	return ok;
}

bool validName(string cur)
{
// Check whether the label  name is valid. Ex: 1var is invalid, val1 is valid
	for(auto to: cur)
    {
		if((to >= 'a' && to <= 'z') || (to >= 'A' && to <= 'Z') || (to >= '0' && to <= '9') || (to == '_'))
			continue;
		return false;
	}
	if((cur[0] >= 'a' && cur[0] <= 'z') || (cur[0] >= 'A' && cur[0] <= 'Z') || (cur[0] == '_'))
		return true;
	return false;
}
