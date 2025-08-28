//
// Created by lucfi on 10/27/2024.
//
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <string>

#ifndef GAMEPIGEONWORDHUNTSOLVER_WORDHUNTTRIE_H
#define GAMEPIGEONWORDHUNTSOLVER_WORDHUNTTRIE_H


class TrieNode {
    public:
        explicit TrieNode(char val);
        void insert(TrieNode* child);
        TrieNode* getChild(char val);
        bool isTerminal() const;
        void setTerminal(std::string& word);
        std::string getTerminal() const;

private:
        char letter;
        std::unordered_map<char, TrieNode*>children;
        std::string terminal;
};


class wordHuntTrie {
    public:
        wordHuntTrie();
        void insertDict(const std::unordered_set<std::string>&dict);
        TrieNode* getRootptr();

    private:
        TrieNode* rootptr;
};


#endif //GAMEPIGEONWORDHUNTSOLVER_WORDHUNTTRIE_H
