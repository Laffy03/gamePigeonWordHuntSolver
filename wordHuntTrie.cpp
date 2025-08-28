//
// Created by lucfi on 10/27/2024.
//

#include "wordHuntTrie.h"

TrieNode::TrieNode(char val) {
    letter = val;
}

void TrieNode::insert(TrieNode *child) {
    children[child->letter] = child;
}

TrieNode* TrieNode::getChild(char val) {
    if (children.find(val) != children.end()) {
        return children[val];
    }
    return nullptr;
}

bool TrieNode::isTerminal() const {
    return !terminal.empty();
}

std::string TrieNode::getTerminal() const {
    return terminal;
}

void TrieNode::setTerminal(std::string& word) {
    terminal = word;
}

wordHuntTrie::wordHuntTrie() {
    rootptr = new TrieNode(' ');
}

void wordHuntTrie::insertDict(const std::unordered_set<std::string>&dict) {
    for (std::string word : dict) {
        TrieNode* curr = rootptr;
        for (char c:word) {
            TrieNode* next = curr->getChild(c);
            if (!next) {
                next = new TrieNode(c);
                curr->insert(next);
            }
            curr = next;
        }
        curr->setTerminal(word);
    }
}

TrieNode* wordHuntTrie::getRootptr() {
    return rootptr;
}