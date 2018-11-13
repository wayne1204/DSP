#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
using namespace std;

class DictionaryManager{
public:
    DictionaryManager(){}

    size_t getKey(const string& str, string& token, size_t start){
        token = str.substr(start, 2);
        start = str.find_first_of('/', start + 1); 
        return start;
    }
    
    void updateDict(string& key, string& word){
        if (key2Index.find(key) == key2Index.end())
        {
            vector<string> mapping{key};
            dictionary.push_back(mapping);
            pair<string, int> ZhuYin(key, index++);
            key2Index.insert(ZhuYin);
        }
        dictionary[key2Index.find(key)->second].push_back(word);
    }

    void processing(const string& str){
        string key, key_old;
        string word = str.substr(0, 2);
        size_t start = 3;

        int i = 0
        while(start != string::npos){
            start = getKey(str, key, start);
            if (key_old != key) { // handling '破音字'
                cout << key << "  " <<endl;
                updateDict(key, word);
                if
            }
        }
        cout <<endl;
        single_word.push_back(word);
    }

    void outputFile(const string& filename){
        ofstream ofs(filename.c_str(), ios::out);
        for(int i = 0; i < dictionary.size(); ++i)
        {
            ofs << dictionary[i][0]<<"   ";
            for(int j = 1; j < dictionary[i].size(); ++j){
                ofs<<' ' << dictionary[i][j];
            }
            ofs<<endl;
            
        }
        
        for (int i = 0; i < single_word.size(); ++i)
            ofs << single_word[i] << "    " << single_word[i] << endl;
    }

private:
    int index;
    unordered_map<string, int> key2Index;
    vector<vector<string> > dictionary;
    vector<string> single_word;
};

int main(int argc, char** argv){
    ifstream ifs;
    string line, key, word;
    string fileName = argv[1];
    DictionaryManager* mgr = new DictionaryManager;
    ifs.open(fileName.c_str(), ios::in);
    while(getline(ifs, line)){
        mgr->processing(line);
    }
    mgr->outputFile(argv[2]);
}