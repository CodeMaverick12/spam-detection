#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <deque>
#include <cmath>
#include <cstring>
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
                c = ' '; // Replace special characters with space
            }
        }
        return message;
    }

    deque<string> tokenize() {
        deque<string> tokens;
        stringstream ss(removeSpecialChar()); // Use cleaned message directly
        string token;

        while (ss >> token) {
            for (char &c : token) {
                c = tolower(c);
            }
            tokens.push_back(token);
        }
        return tokens;
    }

    deque<string> RemoveStopWords(const deque<string>& tokens) {
        vector<string> stopWords = { "i", "me", "my", "myself", "we", "our", "ours", "ourselves", "you", "you're", 
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
             };  // Add your list of stop words here
        deque<string> result;
        for (const string& token : tokens) {
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
    unordered_map<string, int> hamCount;
    unordered_map<string, int> spamCount;
    int totalHam = 0;
    int totalSpam = 0;
    int totalMessages = 0;

public:
    void readData(const string& filename) {
        ifstream file(filename);
        string line, label, message;
        getline(file, line);  // Skip header
        while (getline(file, line)) {
            stringstream ss(line);
            getline(ss, label, ',');
            getline(ss, message);
            CleanData cleaner(message);
            deque<string> tokens = cleaner.processMessage();
            totalMessages++;
            if (label == "ham") {
                totalHam++;
                for (const string& token : tokens) hamCount[token]++;
            } else {
                totalSpam++;
                for (const string& token : tokens) spamCount[token]++;
            }
        }
    }

    pair<string,double> predict(const string& message) {
        CleanData cleaner(message);
        storeUserMessages(message,"UserMessages.csv");
        deque<string> tokens = cleaner.processMessage();
        double spamProb = log(totalSpam / (double)totalMessages);
        double hamProb = log(totalHam / (double)totalMessages);

        for (const string& token : tokens) {
            spamProb += log((spamCount[token] + 1) / (double)(totalSpam + hamCount.size()));
            hamProb += log((hamCount[token] + 1) / (double)(totalHam + hamCount.size()));
        }
        // return spamProb > hamProb ? "spam" : "ham";

        //calculating percentage
        double spamExp = exp(spamProb);
        double hamExp = exp(hamProb);

        double totalProb = spamExp + hamExp;
        double spamPercentage = (spamExp / totalProb) * 100;
        double hamPercentage = (hamExp / totalProb) * 100;
        return spamPercentage>hamPercentage? make_pair("spam", spamPercentage):make_pair("ham", hamPercentage);
    }
    //  void predictFromFile(const string& Inputfilename, const string& Outputfilename) {
    //     ifstream Inputfile(Inputfilename);
    //     ofstream Outputfile(Outputfilename);
    //     string line, message;

    //     if (!Inputfile.is_open()) {
    //         cerr << "Could not open input file: " << Inputfilename << endl;
    //         return;
    //     }
        
    //     if (!Outputfile.is_open()) {
    //         cerr << "Could not open output file: " << Outputfilename << endl;
    //         return;
    //     }
    
    //     while (getline(Inputfile, line)) {
    //         message = line;
    //         // cout << "The message is: " << prediction << endl;
    //         //if the message is spam add spam in next cell otherwise ham
    //         Outputfile << message << "," << (predict(message)=="spam"? "spam" : "ham") << endl;
    //         }
    //     Inputfile.close();
    //     Outputfile.close();
    // }
     void predictFromFile(const string& Inputfilename, const string& Outputfilename) {
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
            // cout << "The message is: " << prediction << endl;
            //if the message is spam add spam in next cell otherwise ham
            pair<string,double> res = predict(message);
            Outputfile << message << "," << (res.first=="spam"? "spam" : "ham") << endl;
            }
        Inputfile.close();
        Outputfile.close();
    }
    
    void storeUserMessages(string message, const string& ResponseFile){
        ofstream outputResponse(ResponseFile, ios::app);
        if (!outputResponse){
            cerr << "Could not open input file: " << ResponseFile << endl;
            return;
        }
        outputResponse << message << endl;
    }

};