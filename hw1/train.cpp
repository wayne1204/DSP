#include "hmm.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <cassert>
#include <math.h>
using namespace std;

/* global variable*/
int sequence[10000][50] ={};
double alpha[6][50] = {};
double beta[6][50] = {};
double gamma_matrix[6][50] = {}; // sum all sample
double gamma_obsev[6][6];
double epsilon[49][6][6] = {}; // sum all sample

/* forward declaraion*/
bool parsingSeq(const string &file);
void trainingModel(HMM &hmm_initial);
void initialParameter();
void ForwardAlgoritm(HMM &hmm_initial, int iteration);
void BackwardAlgoritm(HMM &hmm_initial, int iteration);
void CalculateGamma(HMM &hmm_initial, int iteration); // sum all sample
void CalculateEpsilon(HMM &hmm_initial, int iteration);
void UpdateParameter(HMM &hmm_initial);
void printmatrix(HMM &hmm_initial);
void checking(HMM &hmm_initial);



int main(int argc, char **argv)
{
	/*
	HMM hmms[5];
	load_models( "modellist.txt", hmms, 5);
	dump_models( hmms, 5);
*/
	if (argc != 5){
		cerr << "[error] wrong input number\n";
		return -1;
	}
	// int iterat = atoi(argv[1]);

	HMM hmm_initial;
	loadHMM(&hmm_initial, argv[2]);

	if (!parsingSeq(argv[3]))	
		return -1;
	
	for (int i = 0; i < atoi(argv[1]); ++i)
	{
		trainingModel(hmm_initial);
		if(i % 500 == 0 && i != 0)
			cout<<"iteration"<<i<<endl;
	}

	// printmatrix(hmm_initial);
	// checking(hmm_initial);
	const char* str = "w";
	FILE* fp = open_or_die(argv[4], str);
	dumpHMM(fp, &hmm_initial);
	return 0;
}

void trainingModel(HMM &hmm_initial)
{
	initialParameter();
	for (int i = 0; i < 10000; ++i)
	{
		ForwardAlgoritm(hmm_initial, i);
		BackwardAlgoritm(hmm_initial, i);
		CalculateGamma(hmm_initial, i);
		CalculateEpsilon(hmm_initial, i);
	}
	UpdateParameter(hmm_initial);
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
			int ia = line[i] - 'A';			assert(ia != 6);
			sequence[row][i] = ia;
		}
		++row;
	}
	return true;
}

void initialParameter()
{
	for (int i = 0; i < 6; ++i){
		for (int j = 0; j < 50; ++j){
			// alpha[i][j] = 0;
			// beta[i][j] = 0;
			gamma_matrix[i][j] = 0;
		}
	}
	for (int i = 0; i < 49; ++i){
		for (int j = 0; j < 6; ++j){
			for (int k = 0; k < 6; ++k)	{
				epsilon[i][j][k] = 0;
			}
		}
	}

	for (int i = 0; i < 6; i++)	{
		for (int j = 0; j < 6; ++j)	{
			gamma_obsev[i][j] = exp(-10);
		}
	}
}

void ForwardAlgoritm(HMM &hmm_initial, int iteration)
{
	//initilize
	for (int state = 0; state < 6; ++state)
	{
		int obsev = sequence[iteration][0];
		alpha[state][0] = hmm_initial.initial[state] * hmm_initial.observation[obsev][state];
	}
	// induction
	for(int seq = 0; seq < 49; ++seq)	{
		int obsev = sequence[iteration][seq+1];
		for(int state = 0; state < 6; ++state){
			double sum = 0;
			for (int pre_state = 0; pre_state < 6; ++pre_state){
				sum += alpha[pre_state][seq] * hmm_initial.transition[pre_state][state];
			}
			alpha[state][seq+1] = sum * hmm_initial.observation[obsev][state];
		}
	}
}

void BackwardAlgoritm(HMM &hmm_initial, int iteration)
{
	//initialize
	for (int state = 0; state < 6; ++state){
		beta[state][49] = 1.0;
	}
	// induction
	for (int seq = 48; seq >=  0; --seq){
		int obsev = sequence[iteration][seq+1];
		for(int state = 0; state < 6; ++state){
			double sum = 0;
			for(int next_state = 0; next_state < 6; ++next_state){
				sum += beta[next_state][seq+1] * 
					   hmm_initial.transition[state][next_state] * 
					   hmm_initial.observation[obsev][next_state];
			}
			beta[state][seq] = sum;
		}
	}
}

void CalculateGamma(HMM &hmm_initial, int iteration)
{
	for(int seq = 0; seq < 50; ++seq){
		int obsev = sequence[iteration][seq];
		double sum = 0;
		for(int state = 0; state < 6; ++state){
			sum += alpha[state][seq] * beta[state][seq];
		}
		for(int state = 0; state < 6; ++state){
			gamma_matrix[state][seq] += (alpha[state][seq] * beta[state][seq]) / sum;
			gamma_obsev[obsev][state] += (alpha[state][seq] * beta[state][seq]) / sum;
		}
	}
}

void CalculateEpsilon(HMM &hmm_initial, int iteration)
{
	for(int seq = 0; seq < 49; ++seq){
		int obsev = sequence[iteration][seq+1];
		double sum = 0;
		for(int state = 0; state < 6; ++state){
			sum += alpha[state][seq] * beta[state][seq];
		}
		// cout<<"seq#"<<seq<<" "<<sum<<" "<<sum2<<endl;
		
		for (int pre_state = 0; pre_state < 6; ++pre_state){
			for (int next_state = 0; next_state < 6; ++next_state){
				double current = alpha[pre_state][seq] *
								 beta[next_state][seq + 1] *
								 hmm_initial.transition[pre_state][next_state] *
								 hmm_initial.observation[obsev][next_state];
				epsilon[seq][pre_state][next_state] += (current/sum);							
			}
		}
	}
}

void UpdateParameter(HMM &hmm_initial)
{
	for(int i = 0; i < 6; ++i){
		hmm_initial.initial[i] = gamma_matrix[i][0]/10000;
	}

	double gamma_sum[6] = {};
	for(int state = 0; state < 6; ++state){
		for(int seq = 0; seq < 49; ++seq){
			gamma_sum[state] += gamma_matrix[state][seq];
		}
	}

	for(int state = 0; state < 6; ++state){
		for(int next_state = 0; next_state < 6; ++next_state){
			double sum = 0;
			for(int seq = 0; seq < 49; ++seq){
				sum += epsilon[seq][state][next_state];
			}
			hmm_initial.transition[state][next_state] = sum / gamma_sum[state];
		}
	}
	for(int state = 0; state < 6; ++state)
		gamma_sum[state] += gamma_matrix[state][49];
	

	for(int obsev = 0; obsev < 6; ++obsev){
		for(int state = 0; state < 6; ++state){
			hmm_initial.observation[obsev][state] = gamma_obsev[obsev][state] / gamma_sum[state];
		}
	}
}


void checking(HMM &hmm_initial)
{
	cout << "===== initial matrix =====\n";
	double sum = 0;
	for (int i = 0; i < 6; ++i)
	{
		sum += hmm_initial.initial[i];
	}
	cout << "value:" << sum << endl;
	cout << "===== transition matrix =====\n";
	double s[6] = {};
	for (int i = 0; i < 6; ++i)
	{
		for (int j = 0; j < 6; ++j)
		{
			s[i] += hmm_initial.transition[i][j];
		}
		cout << "row#" << i << " value:" << s[i] << endl;
	}

	cout << "===== obsev matrix =====\n";
	double t[6] = {};
	for (int i = 0; i < 6; ++i)
	{
		for (int j = 0; j < 6; ++j)
		{
			t[i] += hmm_initial.observation[j][i];
		}
		cout << "col#" << i << " value:" << t[i] << endl;
	}
}

void printmatrix(HMM &hmm_initial)
{
	cout << "======[Alpha]======\n";
	for (int i = 0; i < 6; ++i)
	{
		cout << "state" << i << " ";
		for (int j = 0; j < 10; ++j)
		{
			cout << alpha[i][j] << " ";
		}
		cout << endl;
	}

	cout << "======[Beta]======\n";
	for (int i = 0; i < 6; ++i)
	{
		cout << "state" << i << " ";
		for (int j = 0; j < 10; ++j)
		{
			cout << beta[i][j] << " ";
		}
		cout << endl;
	}

	cout << "=======[Gamma]======\n";
	for (int i = 0; i < 6; ++i)
	{
		cout << "state" << i << " ";
		for (int j = 0; j < 10; ++j)
		{
			cout << gamma_matrix[i][j] << " ";
		}
		cout << endl;
	}
}
