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
    setStyleSheet("background-color: #74a36f;");
    auto *mainLayout = new QHBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    // ----------------- Left container: board + reset button -----------------
    auto *leftContainer = new QWidget();
    auto *leftLayout = new QVBoxLayout(leftContainer);
    leftLayout->setSpacing(10);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    // Board container with dark green background
    boardContainer = new QWidget();
    boardContainer->setObjectName("boardContainer"); // For CSS targeting
    const int boardPixelSize = 550;
    boardContainer->setFixedSize(boardPixelSize, boardPixelSize);

    // Set dark green background for the grid
    boardContainer->setStyleSheet("QWidget#boardContainer {"
                                  "background-color: #2a6e3f;"
                                  "border-radius: 15px;"
                                  "}");

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

            // Style the cell with wood background and rounded edges
            cell->setStyleSheet("QLineEdit {"
                                "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                                "stop: 0 #f6d7b0, stop: 1 #e9c69a);"
                                "border: 2px solid #000000;"
                                "border-radius: 10px;"
                                "color: #000000;"
                                "font-weight: bold;"
                                "}"
                                "QLineEdit:focus {"
                                "border: 2px solid #ffa500;"
                                "}");

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
                updateSolveButtonState();
            });

            board[i][j] = cell;
            boardLayout->addWidget(cell, i, j);
        }
    }

    // Reset button with matching style
    auto *resetButton = new QPushButton("Reset Board");
    resetButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    resetButton->setMinimumHeight(30);
    resetButton->setStyleSheet("QPushButton {"
                               "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                               "stop: 0 #6a994e, stop: 1 #386641);"
                               "color: white;"
                               "border: 2px solid #2d5016;"
                               "border-radius: 10px;"
                               "padding: 5px;"
                               "font-weight: bold;"
                               "}"
                               "QPushButton:hover {"
                               "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                               "stop: 0 #8cb369, stop: 1 #6a994e);"
                               "}"
                               "QPushButton:pressed {"
                               "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                               "stop: 0 #386641, stop: 1 #6a994e);"
                               "}");

    leftLayout->addStretch();
    leftLayout->addWidget(boardContainer, 0, Qt::AlignHCenter);
    leftLayout->addWidget(resetButton, 0, Qt::AlignHCenter);
    leftLayout->addStretch();

    mainLayout->addWidget(leftContainer, 3);

    // ----------------- Right container: word list + solve button -----------------
    auto *rightContainer = new QWidget();
    auto *rightLayout = new QVBoxLayout(rightContainer);
    rightLayout->setSpacing(10);

    wordListWidget = new QListWidget();
    wordListWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    wordListWidget->setStyleSheet("QListWidget {"
                                  "background: #f6d7b0;"
                                  "border: 2px solid #000000;"
                                  "border-radius: 10px;"
                                  "color: #3d2b1f;"
                                  "font-weight: bold;"
                                  "}"
                                  "QListWidget::item:selected {"
                                  "background: #e9c69a;"
                                  "color: #3d2b1f;"
                                  "}");

    solveButton = new QPushButton("Solve");
    solveButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    solveButton->setMinimumHeight(40);
    solveButton->setEnabled(false);
    solveButton->setStyleSheet("QPushButton {"
                               "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                               "stop: 0 #6a994e, stop: 1 #386641);"
                               "color: white;"
                               "border: 2px solid #2d5016;"
                               "border-radius: 10px;"
                               "padding: 5px;"
                               "font-weight: bold;"
                               "font-size: 16px;"
                               "}"
                               "QPushButton:hover {"
                               "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                               "stop: 0 #8cb369, stop: 1 #6a994e);"
                               "}"
                               "QPushButton:pressed {"
                               "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                               "stop: 0 #386641, stop: 1 #6a994e);"
                               "}"
                               "QPushButton:disabled {"
                               "background: #aabdac;"
                               "border: 2px solid #7a8c7d;"
                               "}");

    rightLayout->addWidget(wordListWidget);
    rightLayout->addWidget(solveButton);

    mainLayout->addWidget(rightContainer, 1);

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