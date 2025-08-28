#include <QWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QGridLayout>
#include <QPushButton>
#include <vector>
#include "wordHuntTrie.h"

#ifndef GAMEPIGEONWORDHUNTSOLVER_WORDHUNTGUI_H
#define GAMEPIGEONWORDHUNTSOLVER_WORDHUNTGUI_H

class QLineEdit;

class WordHuntGUI : public QWidget {
    Q_OBJECT
public:
    explicit WordHuntGUI(wordHuntTrie& trie, QWidget *parent = nullptr);

private slots:
    void resetBoard();
    void solveBoard();

private:
    std::vector<std::vector<QLineEdit*>> board;
    QPushButton* solveButton;
    QListWidget *wordListWidget;
    QWidget *boardContainer;
    QGridLayout *boardLayout;
    wordHuntTrie& trie;
    void updateSolveButtonState();
    void dfs(char board[4][4], int i, int j, TrieNode* node, std::unordered_set<std::string>& solutions);
};

#endif //GAMEPIGEONWORDHUNTSOLVER_WORDHUNTGUI_H
