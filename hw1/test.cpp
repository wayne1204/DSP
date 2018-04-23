#include "hmm.h"
#include <math.h>
#include <iostream>
#include <fstream>
using namespace std;

// global variable
int data[2500][50];
double Delta[5][6][49];
double Delta_max[5];
int answerIndex[2500];
double answerProb[2500];

// forward declaration
bool parsingSeq(const string &file);
void CalculateDelta(HMM &hmm_model, int model_num, int iteration);
void outputResult(ofstream& file);
void getAccuracy();


int main(int argc, char** argv){
    if(argc != 4){
        cout<<"[Error] wrong input number!\n";
        return -1;
    }
    HMM hmms[5];
    load_models(argv[1], hmms, 5);

    if (!parsingSeq(argv[2]))
    {
        cout << "error\n";
        return 0;
    }
    for(int i = 0; i < 2500; ++i)
    {
        int max_index = 0;
        double max_num = 0;
        for (int modelNum = 0; modelNum < 5; ++modelNum)
        {
            CalculateDelta(hmms[modelNum], modelNum, i);
            max_index = Delta_max[modelNum] > max_num ? modelNum : max_index;
            max_num = max(Delta_max[modelNum], max_num);
        }
        answerIndex[i] = max_index;
        answerProb[i] = max_num;
    }
    ofstream outfile;
    outfile.open(argv[3], ios::out);
    outputResult(outfile);
    // if(argv[2] == "testing_data1.txt")
        getAccuracy();
}

bool parsingSeq(const string &file)
{
	ifstream ifs;
	string line;
	ifs.open(file.c_str(), ios::in);
	if(!ifs.is_open()){
		cout << "[error] File \"" << file << "\" not found!\n";
		return false;
	}

	int row = 0;
	while(getline(ifs,line)){
		for(size_t i = 0, j = line.size(); i < j; ++i){
			int ia = line[i] - 'A';
			data[row][i] = ia;
		}
		++row;
	}
	return true;
}

void CalculateDelta(HMM &hmm_model, int model_num, int iteration)
{   
    //initialize
    int obsev = data[iteration][0];
    for(int state = 0; state < 6; ++state){
        Delta[model_num][state][0] = hmm_model.initial[state] * 
                                     hmm_model.observation[obsev][state];
    }

    for(int seq = 0; seq < 48; ++seq)
    {
        int obsev = data[iteration][seq+1];
        for(int state = 0; state < 6; ++state)
        {
            double max_num = 0;
            for(int prev_state = 0; prev_state < 6; ++prev_state)
            {
                double candidate = Delta[model_num][prev_state][seq] * 
                                   hmm_model.transition[prev_state][state];
                max_num = max(max_num, candidate);
            }
            Delta[model_num][state][seq+1] = max_num * hmm_model.observation[obsev][state];
        }
    }

    Delta_max[model_num] = 0;
    for(int state = 0; state < 6; ++state){
        Delta_max[model_num] = max(Delta_max[model_num], Delta[model_num][state][48]);
    }
    // cout<<"data#"<<iteration<<endl;
    // for (int d = 0; d < 5; ++d)
    // {
    //     for (int i = 0; i < 6; ++i)
    //     {
    //         for (int j = 40; j < 49; ++j)
    //         {
    //             cout << Delta[d][i][j] << " ";
    //         }
    //         cout<<endl;
    //     }
    //     cout<<endl;
    // }
}

void outputResult(ofstream& file){
    string str1 = "model_0";
    string str2 = ".txt ";
    for(int i =0; i<2500; ++i){
        file << str1 << to_string(answerIndex[i]+1) << str2 ;
        file.setf(ios::scientific);
        file << answerProb[i] << endl;
    }
    file.close();
}

void getAccuracy(){
    int count = 0;
    ifstream ifs;
    string line;
    const string file = "testing_answer.txt";
    ifs.open(file.c_str(), ios::in);
    
    if(ifs.is_open()){
        int row = 0;
        while (getline(ifs, line))
        {
            if ((line[7] - '1') == answerIndex[row])
                count++;
            row++;
        }
        double acc = (double)count / 2500.0;
        cout << "Accuracy: " << count << endl;
    }
}