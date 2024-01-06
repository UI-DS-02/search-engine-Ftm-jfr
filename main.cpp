#include <iostream>
#include <map>
#include <fstream>
#include <filesystem>
#include <string>
#include <set>
#include <regex>

using namespace std;

string Path =R"(D:\UNI\SEMASTER3\Ramezani\codes\searchengine\EnglishData)";

string removeNonAlphanumeric(const string& input) {
    regex pattern("[^a-zA-Z0-9 ]");
    return regex_replace(input, pattern, " ");
}

vector<string> splitBySpace(const string& input) {
    istringstream iss(input);
    vector<string> words;
    string word;

    while (iss >> word) {
        words.push_back(word);
    }

    return words;
}
void readFiles(const string &filename, map<string, set<string>> &m){

    ifstream f(Path+"\\"+filename);
    string line;

    while(getline(f,line)){

        string processedString = removeNonAlphanumeric(line);
        vector<string> separatedStrings = splitBySpace(processedString);

        while (!separatedStrings.empty()) {
            string word = separatedStrings.back();
            transform(word.begin(), word.end(), word.begin(), ::toupper);
            separatedStrings.pop_back();
            if( m.find(word) != m.end()){

                set<string> s;
                m.insert(make_pair(word,s));
                m[word].insert(filename);
            }
            else {

                if(!word.empty())
                    m[word].insert(filename);
            }
        }
    }

}
set<string> And(vector<string>& v, const map<string, set<string>>& m, set<string> filenames) {
    if (v.empty())
        return filenames;

    int n = v.size();
    set<string> result;

    for (const string& filename : filenames) {
        if (m.count(v[n - 1]) > 0 && m.at(v[n - 1]).count(filename) == 1) {
            result.insert(filename);
        }
    }

    v.pop_back();

    return And(v, m, result);
}
set<string> Or(const vector<string>& v, const map<string, set<string>>& m,set<string> filenames) {

    set<string> result;
    for (const string& word : v) {
        auto it = m.find(word);
        if (it != m.end()) {
            const set<string>& filesWithWord = it->second;
            result.insert(filesWithWord.begin(), filesWithWord.end());
        }
    }
    for(const auto& file:filenames){
        if(result.count(file) < 1){
            filenames.erase(file);
        }
    }
    return filenames;
}

set<string> Not(const vector<string>& v, const map<string, set<string>>& m, set<string> filenames) {
    for (const string& word : v) {
        auto it = m.find(word);
        if (m.count(word) > 0) {
            const set<string>& filesWithWord = it->second;
            for (const string& filename : filesWithWord) {
                filenames.erase(filename);
            }
        }
    }

    return filenames;
}

void search(const map<string, set<string>>& m, const set<string>& filenames) {
    string input;
    getline(cin, input);
    string word;
    set<string> result ;

    vector<string> and_input;
    vector<string> or_input;
    vector<string> not_input;

    stringstream ss(input);
    while (getline(ss, word, ' ')) {
        transform(word.begin(), word.end(), word.begin(), ::toupper);

            if (word[0] == '+') {

                word = word.substr(1);
                or_input.push_back(word);

            } else if (word[0] == '-') {

                word = word.substr(1);
                not_input.push_back(word);

            } else {

                and_input.push_back(word);

            }

    }
    result = filenames;
    if(!not_input.empty()){
        result = Not(not_input,m,result);
    }
    if(!and_input.empty()){
        result = And(and_input,m,result);
    }
    if(!or_input.empty()){
        result = Or(or_input,m,result);
    }

    if (result.empty()) {
        cout << "No matches found." << endl;
    } else {
        cout << result.size() << endl;
        for (const string& filename : result) {
            cout << filename << endl;
        }
    }
}

int main() {
    map<string,set<string>> m;
    set<string> filenames;

    for(const auto & entry : filesystem::directory_iterator(Path)) {
        string s = entry.path().string();
        reverse(s.begin(),s.end());
        int pos = s.find("\\");
        string sub = s.substr(0, pos);
        reverse(sub.begin(),sub.end());
        readFiles(sub, m);
        filenames.insert(sub);
    }

    search(m,filenames);

    return 0;
}
