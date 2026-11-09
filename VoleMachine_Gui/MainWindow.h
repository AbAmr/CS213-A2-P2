#pragma once

#include <QMainWindow>
#include <memory>

class QFrame;
class QLabel;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;
class QStackedWidget;
class QTableWidget;
class VoleMachine;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow() override;

private slots:
    void loadProgram();
    void openFile();
    void insertSample();
    void stepOnce();
    void runAll();
    void resetMachine();
    void toggleMemory();

private:
    QWidget* buildSidebar();
    QWidget* buildCenter();
    QWidget* buildRight();

    void execute(bool runAll);
    void refresh();
    void log(const QString& text);
    void snapshotRegisters();

    std::unique_ptr<VoleMachine> machine;
    bool loaded = false;
    bool showMemory = false;
    unsigned startAddress = 0;
    unsigned programLength = 0;
    int previousRegs[16];
    bool havePrevious = false;

    QPlainTextEdit* source = nullptr;
    QLineEdit* startEdit = nullptr;
    QPushButton* stepButton = nullptr;
    QPushButton* runButton = nullptr;
    QPushButton* dumpButton = nullptr;
    QStackedWidget* views = nullptr;
    QTableWidget* listing = nullptr;
    QTableWidget* memoryTable = nullptr;
    QPlainTextEdit* console = nullptr;
    QLabel* pcLabel = nullptr;
    QLabel* irLabel = nullptr;
    QLabel* stateLabel = nullptr;
    QLabel* regLabels[16] = {};
};
