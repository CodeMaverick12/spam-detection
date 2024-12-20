#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <deque>
#include <cmath>
#include <cstring>
#include <string>
#include <utility>  
using namespace std;

class CleanData {
private:
    string message;

public:
    CleanData(string message = "") : message(message) {}

    string removeSpecialChar() {
        for (char &c : message) {
            if (!isalnum(c) && c != '-') {
                c = ' ';
            }
        }
        return message;
    }

    deque<string> tokenize() {
        deque<string> tokens;
        stringstream ss(removeSpecialChar()); 
        string token;

        while (ss >> token) {
            for (char &c : token) {
                c = tolower(c);
            }
            tokens.push_back(token);
        }
        return tokens;
    }

    deque<string> RemoveStopWords(deque<string>& tokens) {
        vector<string> stopWords = { 
            "i", "me", "my", "myself", "we", "our", "ours", "ourselves", "you", "you're", 
            "you've", "you'll", "you'd", "your", "yours", "yourself", "yourselves", 
            "he", "him", "his", "himself", "she", "she's", "her", "hers", "herself", 
            "it", "it's", "its", "itself", "they", "them", "their", "theirs", 
            "themselves", "what", "which", "who", "whom", "this", "that", "that'll", 
            "these", "those", "am", "is", "are", "was", "were", "be", "been", "being", 
            "have", "has", "had", "having", "do", "does", "did", "doing", "a", "an", 
            "the", "and", "but", "if", "or", "because", "as", "until", "while", 
            "of", "at", "by", "for", "with", "about", "against", "between", "into", 
            "through", "during", "before", "after", "above", "below", "to", "from", 
            "up", "down", "in", "out", "on", "off", "over", "under", "again", 
            "further", "then", "once", "here", "there", "when", "where", "why", 
            "how", "all", "any", "both", "each", "few", "more", "most", "other", 
            "some", "such", "no", "nor", "not", "only", "own", "same", "so", 
            "than", "too", "very", "s", "t", "can", "will", "just", "don", 
            "don't", "should", "should've", "now", "d", "ll", "m", "o", 
            "re", "ve", "y", "ain", "aren", "aren't", "couldn", "couldn't", 
            "didn", "didn't", "doesn", "doesn't", "hadn", "hadn't", "hasn", 
            "hasn't", "haven", "haven't", "isn", "isn't", "ma", "mightn", 
            "mightn't", "mustn", "mustn't", "needn", "needn't", "shan", 
            "shan't", "shouldn", "shouldn't", "wasn", "wasn't", "weren", 
            "weren't", "won", "won't", "wouldn", "wouldn't"
            }; 
        deque<string> result;
        for (string& token : tokens) {
            if (find(stopWords.begin(), stopWords.end(), token) == stopWords.end()) {
                result.push_back(token);
            }
        }
        return result;
    }

    deque<string> processMessage() {
        deque<string> tokens = tokenize();
        return RemoveStopWords(tokens);
    }
};
class NaiveBayes {

private:
    unordered_map<string, int> hamCount;    //frequency of unique words in ham messages
    unordered_map<string, int> spamCount;   //frequency of unique words in spam messages
    double totalHam = 0;
    double totalSpam = 0;
    double totalMessages = 0;
    // int totalHam = 0;
    // int totalSpam = 0;
    // int totalMessages = 0;

public:
    void readData(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening file: " << filename << endl;
            return;
        }
        string line, label, message;
        getline(file, line); 
        while (getline(file, line)) {
            stringstream ss(line);
            getline(ss, label, ',');
            getline(ss, message);
            CleanData cleaner(message);
            deque<string> tokens = cleaner.processMessage();
            totalMessages++;
            if (label == "ham") {
                totalHam++;
                
                for (string& token : tokens) 
                    hamCount[token]++;
            } 
            else {
                totalSpam++;
                for (string& token : tokens) 
                    spamCount[token]++;
            }
        }
    }

    string predict(const string& message) {
        CleanData cleaner(message);
        storeUserMessages(message,"UserMessages.csv");
        deque<string> tokens = cleaner.processMessage();
        double spamProb = log(totalSpam / (double)totalMessages);
        double hamProb = log(totalHam / (double)totalMessages);
        size_t totalVocabSize = 0;
        unordered_set<string> vocabSet;

        // Combine the tokens from both spam and ham counts
        for (const auto& entry : spamCount) {
            vocabSet.insert(entry.first);
        }
        for (const auto& entry : hamCount) {
            vocabSet.insert(entry.first);
        }
        totalVocabSize = vocabSet.size();
        for (string& token : tokens) {
            // spamProb += log((spamCount[token] + 1) / (double)(totalSpam + hamCount.size()));
            spamProb += log((spamCount[token] + 1) / (double)(totalSpam + totalVocabSize));
            hamProb += log((hamCount[token] + 1) / (double)(totalHam + totalVocabSize));
        }
        return spamProb > hamProb ? "spam" : "ham";
    }

    // for training and testing
    void predictFromFile(string& Inputfilename, string& Outputfilename) {
        ifstream Inputfile(Inputfilename);
        ofstream Outputfile(Outputfilename);
        string line, message;

        if (!Inputfile.is_open()) {
            cerr << "Could not open input file: " << Inputfilename << endl;
            return;
        }
        
        if (!Outputfile.is_open()) {
            cerr << "Could not open output file: " << Outputfilename << endl;
            return;
        }

        while (getline(Inputfile, line)) {
            message = line;
            Outputfile << message << "," << (predict(message)=="spam"? "spam" : "ham") << endl;
        }
        Inputfile.close();
        Outputfile.close();
    }

    //store results
    void storeResults(const string& Model){
        ofstream outputModel(Model);
        if (!outputModel){
            cerr << "Could not open input file: " << Model << endl;
            return;
        }
        //storing spam words count individual
        for (auto pair : spamCount) {
            outputModel << pair.first << ":" << pair.second << " ";
        }
        outputModel << endl;
        //storing ham words count individual
        for (auto pair : hamCount) {
            outputModel << pair.first << ":" << pair.second << " ";
        }
        outputModel << endl;
        //Spam words  Frequency
        outputModel  << totalSpam << endl;
        //Ham words Frequency
        outputModel << totalHam << endl;
        //Total messges
        outputModel <<  totalMessages << endl;
        // Spam Probability
        outputModel << (totalSpam / totalMessages) << endl;
         //Ham Probability
        outputModel << totalHam / totalMessages << endl;

        outputModel.close();
    }

    void loadModelResults(const string& Model) {
        ifstream inputModel(Model);
        if (!inputModel) {
            cerr << "Could not open input file: " << Model << endl;
            return;
        }
        
        string line;
        string word, count;

        // reading first line (Spam word counts)
        if (getline(inputModel, line)) {
            stringstream ss(line);
            while (getline(ss, word, ' ')) {
                size_t pos = word.find(':');
                string token = word.substr(0, pos);  
                string countStr = word.substr(pos + 1);  
                spamCount[token] = stoi(countStr); 
            }
        }

        // Reading second line (Ham word counts)
        if (getline(inputModel, line)) {
            stringstream ss(line);
            while (getline(ss, word, ' ')) {
                size_t pos = word.find(':'); 
                string token = word.substr(0, pos);
                string countStr = word.substr(pos + 1);
                hamCount[token] = stoi(countStr); 
            }
        }

        // Reading the total counts of spam , ham and messages which we get while training
        if (getline(inputModel, line)) totalSpam = stoi(line);
        if (getline(inputModel, line)) totalHam = stoi(line);
        if (getline(inputModel, line)) totalMessages = stoi(line);

        cout << "Total Spam: " << totalSpam << endl;
        cout << "Total Ham: " << totalHam << endl;
        cout << "Total Messages: " << totalMessages << endl;

        inputModel.close();
    }

    void storeUserMessages(string message, const string& ResponseFile){
        ofstream outputResponse(ResponseFile, ios::app);
        if (!outputResponse){
            cerr << "Could not open input file: " << ResponseFile << endl;
            return;
        }
        outputResponse << message << endl;
    }
    // process when user provides negative feedback
    void updateModel(string label, string message){
        CleanData cleaner(message);
        deque<string> tokens = cleaner.processMessage();
        if(label == "spam"){
            for (string& token : tokens) {
                spamCount[token]++;
                hamCount[token] = max(hamCount[token]-1, 0);                
            }
        }
        else{
            for (string& token : tokens) {
                hamCount[token]++;
                spamCount[token] = max(spamCount[token]-1, 0);                
            }
        }
        storeResults("modelResults.txt");
    }
    void checking(){
        int count = 0;
        for (auto it = hamCount.begin(); it != hamCount.end() && count < 5; it++, count++) {
            cout << "Ham Token: " << it->first << ", Count: " << it->second << endl;
        }

        // Looping through first 5 tokens of spamCount
        count = 0;
        for (auto it = spamCount.begin(); it != spamCount.end() && count < 5; it++, count++) {
            cout << "Spam Token: " << it->first << ", Count: " << it->second << endl;
        }
    }

};