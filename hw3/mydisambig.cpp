#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include "Ngram.h"

using namespace std;
typedef std::pair<vector<string>, double> pathPair;


class Dictionary{
public:
    void reset(){
        wordTree.clear();
        bestPath.clear();
        ProbTable.clear();
        TraceTable.clear();
    }

    void parseMap(const string& str){
        string line, token;
        ifstream ifs(str.c_str(), ios::in);

        while(getline(ifs, line))
        {
            string ZhuYin = line.substr(0, 2);             
            if (line.size() > 10){
                vector <string> Big5;
                size_t pos = 3;
                while (str2token(line, token, pos) != string::npos)
                {
                    assert(token.size() == 2);
                    Big5.push_back(token);
                    pos = str2token(line, token, pos);
                }
                mapping[ZhuYin] = Big5;
            }
            else{
                vector<string> single_word {ZhuYin};
                mapping[ZhuYin] = single_word;
            }
        }
        ifs.close();
    }

    void parseText(string& line){
        size_t pos = 0;
        string token;
        while(str2token(line, token, pos) != string::npos){
            assert(token.size() == 2);
            wordTree.push_back(mapping.find(token)->second);
            pos = str2token(line, token, pos);
        }
    }
    
    double getBigramProb(const string& s1, const string& s2, Vocab& voc, Ngram& lmm)
    {
        VocabIndex wid1 = voc.getIndex(s1.c_str());
        VocabIndex wid2 = voc.getIndex(s2.c_str());

        if(wid1 == Vocab_None)  //OOV
            wid1 = voc.getIndex(Vocab_Unknown);
        if(wid2 == Vocab_None)  //OOV
            wid2 = voc.getIndex(Vocab_Unknown);

        VocabIndex context[] = { wid1, Vocab_None };
        return lmm.wordProb( wid2, context);
    }

    void viterbiPath(Ngram& ngram, Vocab& vocab){
        //  initialize
        vector<double> tmp;
        for (int i = 0; i < wordTree[0].size(); ++i){
            double prob = getBigramProb("", wordTree[0][i], vocab, ngram);
            tmp.push_back(prob);
        }
        ProbTable.push_back(tmp);
        // cout << wordTree[0].size() << " ";

        // recursion
        for(int i = 1; i < wordTree.size(); ++i)
        {
            vector<double> tmp;
            vector<int> last_move;
            for (int j = 0; j <  wordTree[i].size(); ++j)
            {
                double bestprob = -10000000.0;
				int index = 0;
                for (int k = 0; k < wordTree[i-1].size(); ++k)
                {
                    double prob = (getBigramProb(wordTree[i-1][k], wordTree[i][j], vocab, ngram)
									+ ProbTable[i-1][k]);
                    if(prob >= bestprob){
                        bestprob = prob;
                        index = k;
                    }
                }
                tmp.push_back(bestprob);
                last_move.push_back(index);
            }
            ProbTable.push_back(tmp);
            TraceTable.push_back(last_move);
        }

        //backtrack
        int index = 0;
        int length = ProbTable.size()-1;
        double maxProb = -1000000.0;
        for(int i = 0 ; i < ProbTable[length].size(); ++i){
            if(ProbTable[length][i] > maxProb){
                index = i;
                maxProb = ProbTable[length][i];
            }
        }
        string word = wordTree[length][index];
        bestPath.push_back(word);

        // cout << wordTree.size() << " " << ProbTable.size() << " "<< TraceTable.size() <<endl;
        for(int i = length - 1; i >= 0; --i){
            index = TraceTable[i][index];
            word = wordTree[i][index];
            bestPath.push_back(word);
        }
    }

    void outputSentence(){
        cout << "<s>";
        for(int i = bestPath.size()-1; i >=0; --i){
            cout << " " << bestPath[i];
        }
        cout << " </s>" << endl;
    }

private:
    size_t str2token(const string &str, string &tok, size_t pos = 0)
    {
        size_t begin = str.find_first_not_of(' ', pos);
        if (begin == string::npos) { tok = ""; return begin; }
        size_t end = str.find_first_of(' ', begin);
        tok = str.substr(begin, end - begin);
        return end;
    }

    unordered_map<string, vector<string> > mapping;
    vector< vector<string> > wordTree;
    vector< vector<double> > ProbTable; // record log prob 
    vector< vector<int> >TraceTable; // record last move
    vector<string> bestPath;
};

int main(int argc, char** argv){
    string map, text, model, order;
    for (int i = 1; i <= 8; i += 2)
	{
		if(argv[i] == string("-map"))
            map = argv[i+1];
		else if(argv[i] ==  string("-text"))
            text = argv[i+1];
		else if(argv[i] ==  string("-lm")) 
            model = argv[i+1];
		else if(argv[i] ==  string("-order")) 
            order = argv[i+1];
		else 
			cerr << "[Error] unknown command" << argv[i] <<endl;
	}


    Vocab vocab;
    Ngram lm(vocab, 2);
    File frLm(model.c_str(), "r"); 
    lm.read(frLm);
    frLm.close();
    
    Dictionary* dict = new Dictionary();
    dict->parseMap(map);

    ifstream ifs;
    string line;
    ifs.open(text.c_str(), ios::in);
    while( getline(ifs, line)){
        dict->reset();
        dict->parseText(line);
        dict->viterbiPath(lm, vocab);
        dict->outputSentence();
    }

    int order_ = atoi(order.c_str());
    return 0;
}