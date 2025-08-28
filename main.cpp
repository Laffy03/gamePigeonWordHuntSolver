#include <iostream>
#include <unordered_set>
#include <fstream>
#include <chrono>
#include <QApplication>

#include "wordHuntTrie.h"
#include "wordHuntGUI.h"


using namespace std;


unordered_set<string> buildWordDict(const string& dictionary_filepath);


int main(int argc, char *argv[]) {
    auto trieStartTime = chrono::high_resolution_clock::now();
    unordered_set<string> wordDict = buildWordDict(R"(path)");
    wordHuntTrie trie;
    trie.insertDict(wordDict);
    auto trieEndTime = chrono::high_resolution_clock::now();
    chrono::duration<double> buildTrieElapsed = trieEndTime - trieStartTime;
    cout << "Time to import dict and build trie: " << buildTrieElapsed.count() << " seconds" << endl;

    QApplication app(argc, argv);
    WordHuntGUI window(trie);
    window.setWindowTitle("WordHunt Solver");
    window.show();
    return app.exec();
}


unordered_set<string> buildWordDict(const string& dictionary_filepath) {
    ifstream file(dictionary_filepath);
    if (!file.is_open()) {
        throw ios_base::failure("Error opening file: " + dictionary_filepath);
    }
    unordered_set<string> dict;
    string word;
    while (getline(file, word, '\n')) {
        if (word.length() >= 3 && word.length() <= 16) {
            dict.insert(word);
        }
    }
    return dict;
}

