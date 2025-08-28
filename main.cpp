#include <iostream>
#include <unordered_set>
#include <fstream>
#include <chrono>
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDir>

#include "wordHuntTrie.h"
#include "wordHuntGUI.h"


using namespace std;


unordered_set<string> buildWordDict(const QString& resourcePath);


int main(int argc, char *argv[]) {
    QDir dir(":/");
    QStringList files = dir.entryList();
    qDebug() << "Available resources:" << files;
    auto trieStartTime = chrono::high_resolution_clock::now();
    unordered_set<string> wordDict = buildWordDict(":/CollinsScrabbleWords2019.txt");
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


unordered_set<string> buildWordDict(const QString& resourcePath) {
    QFile file(resourcePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw ios_base::failure("Error opening resource: " + resourcePath.toStdString());
    }

    unordered_set<string> dict;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        string word = line.toStdString();
        if (word.length() >= 3 && word.length() <= 16) {
            dict.insert(word);
        }
    }
    return dict;
}

