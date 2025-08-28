//
// Created by lucfi on 8/26/2025.
//
#include "WordHuntGUI.h"

#include <QVBoxLayout>
#include <QString>
#include <QtGlobal>
#include <QFont>
#include <unordered_set>


// Custom validator for single uppercase letters
class UppercaseValidator : public QValidator {
public:
    explicit UppercaseValidator(QObject *parent = nullptr) : QValidator(parent) {}
    State validate(QString &input, int &pos) const override {
        Q_UNUSED(pos);
        if (input.isEmpty()) return Acceptable;
        input = input.toUpper();
        return (input.length() == 1 && input[0].isLetter()) ? Acceptable : Invalid;
    }
};

WordHuntGUI::WordHuntGUI(wordHuntTrie& t, QWidget *parent)
        : QWidget(parent), trie(t)
{
    auto *mainLayout = new QHBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    // ----------------- Left container: board + reset button -----------------
    auto *leftContainer = new QWidget();
    auto *leftLayout = new QVBoxLayout(leftContainer);
    leftLayout->setSpacing(10);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    // Board container
    boardContainer = new QWidget();
    const int boardPixelSize = 550; // total board size
    boardContainer->setFixedSize(boardPixelSize, boardPixelSize);

    // Grid layout inside board container, add cells
    boardLayout = new QGridLayout(boardContainer);
    boardLayout->setHorizontalSpacing(7);
    boardLayout->setVerticalSpacing(7);
    boardLayout->setContentsMargins(5, 5, 5, 5);

    board.resize(4, std::vector<QLineEdit*>(4));

    int cellSize = (boardPixelSize - boardLayout->horizontalSpacing() * 3 - 10) / 4;
    QFont cellFont;
    cellFont.setBold(true);
    cellFont.setPointSize(cellSize / 3);

    auto *validator = new UppercaseValidator(this);

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            auto *cell = new QLineEdit();
            cell->setMaxLength(1);
            cell->setAlignment(Qt::AlignCenter);
            cell->setFixedSize(cellSize, cellSize);
            cell->setFont(cellFont);
            cell->setStyleSheet("border: 1px solid black;");
            cell->setValidator(validator);

            // autofocus and check if solve button should be enabled
            connect(cell, &QLineEdit::textChanged, [this, i, j](const QString &text) {
                if (text.length() == 1) {
                    int nextRow = i;
                    int nextCol = j + 1;
                    if (nextCol >= 4) {
                        nextCol = 0;
                        nextRow++;
                    }
                    if (nextRow < 4)
                        board[nextRow][nextCol]->setFocus();
                }
                updateSolveButtonState(); // enable/disable solve button
            });

            board[i][j] = cell;
            boardLayout->addWidget(cell, i, j);
        }
    }

    // Reset button centered under the board
    auto *resetButton = new QPushButton("Reset Board");
    resetButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    resetButton->setMinimumHeight(30);

    leftLayout->addStretch();
    leftLayout->addWidget(boardContainer, 0, Qt::AlignHCenter);
    leftLayout->addWidget(resetButton, 0, Qt::AlignHCenter);
    leftLayout->addStretch();

    mainLayout->addWidget(leftContainer, 3); // left panel takes 3/4 width

    // ----------------- Right container: word list + solve button -----------------
    auto *rightContainer = new QWidget();
    auto *rightLayout = new QVBoxLayout(rightContainer);
    rightLayout->setSpacing(10);

    wordListWidget = new QListWidget();
    wordListWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    solveButton = new QPushButton("Solve");
    solveButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    solveButton->setMinimumHeight(40);
    solveButton->setEnabled(false);

    rightLayout->addWidget(wordListWidget);
    rightLayout->addWidget(solveButton);

    mainLayout->addWidget(rightContainer, 1); // right panel takes 1/4 width

    // ----------------- Connect signals -----------------
    connect(solveButton, &QPushButton::clicked, this, &WordHuntGUI::solveBoard);
    connect(resetButton, &QPushButton::clicked, this, &WordHuntGUI::resetBoard);
}


void WordHuntGUI::updateSolveButtonState() {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (board[i][j]->text().isEmpty()) {
                solveButton->setEnabled(false);
                return;
            }
        }
    }
    solveButton->setEnabled(true);
}


void WordHuntGUI::resetBoard() {
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            board[i][j]->clear();

    wordListWidget->clear();
    updateSolveButtonState();
}


void WordHuntGUI::solveBoard() {
    auto solveStartTime = std::chrono::high_resolution_clock::now();

    char letters[4][4]; // Collect letters from GUI
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            letters[i][j] = board[i][j]->text().toUpper().toStdString()[0];

    std::unordered_set<std::string>solutions; // Run DFS starting at each letter
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            dfs(letters, i, j, trie.getRootptr(), solutions);
        }
    }
    std::vector<std::string>sortedWordsByLength; // Sort and print to GUI
    for (const auto& word:solutions) {
        sortedWordsByLength.emplace_back(word);
    }
    sort(sortedWordsByLength.begin(), sortedWordsByLength.end(), [](std::string& s1, std::string& s2) {return s1.size() > s2.size();});
    wordListWidget->clear();
    for (auto& word:sortedWordsByLength) {
        wordListWidget->addItem(QString::fromStdString(word));
    }

    auto solveEndTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> solveElapsed = solveEndTime - solveStartTime;
    std::cout << "Time to search trie: " << solveElapsed.count() << " seconds" << std::endl;
}


void WordHuntGUI::dfs(char board[4][4], int i, int j, TrieNode* node, std::unordered_set<std::string>& solutions) {
    if (i < 0 || i >= 4 || j < 0 || j >= 4 || board[i][j] == '#')
        return;
    char letter = board[i][j];
    if (!node->getChild(letter))
        return;
    node = node->getChild(letter);
    if (node->isTerminal()) {
        solutions.insert(node->getTerminal());
    }

    board[i][j] = '#';

    std::pair<int, int> directions[] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0},{1, 1}, {-1, -1}, {1, -1}, {-1, 1}};
    for (auto& dir : directions) {
        int new_i = i + dir.first;
        int new_j = j + dir.second;
        dfs(board, new_i, new_j, node, solutions);
    }

    board[i][j] = letter;
}