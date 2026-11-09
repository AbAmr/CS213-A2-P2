#include "MainWindow.h"

#include <QColor>
#include <QFile>
#include <QFileDialog>
#include <QFontDatabase>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QStackedWidget>
#include <QTableWidget>
#include <QVBoxLayout>

#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// Qt on Windows can define an ERROR macro that would break ExecutionStatus::ERROR.
#ifdef ERROR
#undef ERROR
#endif

#include "VoleMachine.h"

namespace {

// ---------- helpers ----------

QString hx(unsigned value, int digits) {
    return QString::number(value, 16).toUpper().rightJustified(digits, '0');
}

// Captures everything the simulator prints with std::cout (the [SCREEN] lines).
struct Capture {
    std::ostringstream text;
    std::streambuf* old;
    Capture() : old(std::cout.rdbuf(text.rdbuf())) {}
    ~Capture() { std::cout.rdbuf(old); }
};

bool isHex(const std::string& s) {
    if (s.empty()) return false;
    for (char c : s)
        if (!std::isxdigit(static_cast<unsigned char>(c))) return false;
    return true;
}

// Same rules as the console version: hex tokens, optional 0x, comments with # ; //
bool parseProgram(const std::string& text, std::vector<uint8_t>& out, std::string& err) {
    std::istringstream in(text);
    std::string line, digits;
    int lineNo = 0;
    while (std::getline(in, line)) {
        ++lineNo;
        for (const char* marker : {"#", ";", "//"}) {
            const size_t pos = line.find(marker);
            if (pos != std::string::npos) line.erase(pos);
        }
        std::istringstream tokens(line);
        std::string token;
        while (tokens >> token) {
            if (token.size() > 2 && token[0] == '0' && (token[1] == 'x' || token[1] == 'X'))
                token = token.substr(2);
            if (!isHex(token)) {
                err = "invalid hex value '" + token + "' on line " + std::to_string(lineNo);
                return false;
            }
            digits += token;
        }
    }
    if (digits.empty()) { err = "the program is empty"; return false; }
    if (digits.size() % 2 != 0) {
        err = "odd number of hex digits (" + std::to_string(digits.size()) + "); every byte needs two";
        return false;
    }
    out.clear();
    for (size_t i = 0; i < digits.size(); i += 2)
        out.push_back(static_cast<uint8_t>(std::stoi(digits.substr(i, 2), nullptr, 16)));
    return true;
}

// Turns a 16-bit instruction into readable text, e.g. 0x3000 -> "STORE R0, [00]"
QString disassemble(unsigned w) {
    const unsigned op = w >> 12;
    const QString r = "R" + hx((w >> 8) & 0xF, 1);
    const QString x = "R" + hx((w >> 4) & 0xF, 1);
    const QString y = "R" + hx(w & 0xF, 1);
    const QString a = hx(w & 0xFF, 2);
    switch (op) {
        case 0x1: return "LOAD " + r + ", [" + a + "]";
        case 0x2: return "LOAD " + r + ", " + a;
        case 0x3: return "STORE " + r + ", [" + a + "]";
        case 0x4: return "MOVE " + x + " -> " + y;
        case 0x5: return "ADD " + r + " = " + x + " + " + y;
        case 0x6: return "ADDF " + r + " = " + x + " + " + y;
        case 0x7: return "OR " + r + " = " + x + " | " + y;
        case 0x8: return "AND " + r + " = " + x + " & " + y;
        case 0x9: return "XOR " + r + " = " + x + " ^ " + y;
        case 0xA: return "ROR " + r + ", " + QString::number(w & 0xF);
        case 0xB: return "JUMP " + a + " if " + r + " == R0";
        case 0xC: return "HALT";
        default:  return "???";
    }
}

const char* STYLE = R"(
QWidget { background: #0E1116; color: #E6EAF0; font-size: 14px; }
QLabel { background: transparent; }
#side, #right { background: #151A22; }
#side { border-right: 1px solid #262E3A; }
#right { border-left: 1px solid #262E3A; }
#title { font-size: 20px; font-weight: 700; }
#section { color: #9AA6B8; font-size: 11px; font-weight: 700; }
#caption { color: #9AA6B8; font-size: 12px; }
QPlainTextEdit, QLineEdit { background: #0E1116; border: 1px solid #38445A; border-radius: 6px; padding: 6px; }
QPushButton { background: #1E2633; border: 1px solid #38445A; border-radius: 8px; min-height: 38px; font-weight: 600; }
QPushButton:hover { background: #27324A; }
QPushButton:disabled { color: #667085; background: #171D28; }
QPushButton#primary { background: #3DDC84; border-color: #3DDC84; color: #08210F; }
QPushButton#primary:hover { background: #5BE89A; }
QTableWidget { background: #0E1116; border: none; gridline-color: #0E1116; }
QHeaderView::section { background: #0E1116; color: #8493A8; border: none; padding: 4px; }
#console { background: #080A0E; color: #8FE3B0; border: none; border-top: 1px solid #262E3A; border-radius: 0; padding: 12px 16px; }
)";

}  // namespace

// ---------- MainWindow ----------

MainWindow::MainWindow() : machine(new VoleMachine) {
    for (int& v : previousRegs) v = 0;

    setWindowTitle("Vole Machine Simulator");
    resize(1200, 720);

    QWidget* central = new QWidget;
    QHBoxLayout* layout = new QHBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(buildSidebar());
    layout->addWidget(buildCenter(), 1);
    layout->addWidget(buildRight());
    setCentralWidget(central);
    setStyleSheet(STYLE);

    log("Ready. Load a program to begin.");
    refresh();
}

MainWindow::~MainWindow() = default;

QWidget* MainWindow::buildSidebar() {
    QFrame* side = new QFrame;
    side->setObjectName("side");
    side->setFixedWidth(290);
    QVBoxLayout* v = new QVBoxLayout(side);
    v->setContentsMargins(20, 20, 20, 20);
    v->setSpacing(10);

    QLabel* title = new QLabel("Vole Machine");
    title->setObjectName("title");
    v->addWidget(title);

    QLabel* load = new QLabel("LOAD");
    load->setObjectName("section");
    v->addWidget(load);

    QLabel* cap = new QLabel("Program (hex)");
    cap->setObjectName("caption");
    v->addWidget(cap);

    source = new QPlainTextEdit;
    source->setPlaceholderText("2005 2101 3000 C000");
    source->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    source->setMinimumHeight(140);
    v->addWidget(source, 1);

    QPushButton* open = new QPushButton("Open .txt file...");
    connect(open, &QPushButton::clicked, this, &MainWindow::openFile);
    v->addWidget(open);

    QLabel* cap2 = new QLabel("Start address (hex)");
    cap2->setObjectName("caption");
    v->addWidget(cap2);

    startEdit = new QLineEdit("0A");
    startEdit->setMaxLength(2);
    startEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9A-Fa-f]{0,2}"), startEdit));
    v->addWidget(startEdit);

    QPushButton* loadBtn = new QPushButton("Load program");
    loadBtn->setObjectName("primary");
    connect(loadBtn, &QPushButton::clicked, this, &MainWindow::loadProgram);
    v->addWidget(loadBtn);

    QPushButton* sample = new QPushButton("Insert sample program");
    connect(sample, &QPushButton::clicked, this, &MainWindow::insertSample);
    v->addWidget(sample);

    QLabel* run = new QLabel("RUN");
    run->setObjectName("section");
    v->addWidget(run);

    stepButton = new QPushButton("Step");
    connect(stepButton, &QPushButton::clicked, this, &MainWindow::stepOnce);
    v->addWidget(stepButton);

    runButton = new QPushButton("Run to HALT");
    connect(runButton, &QPushButton::clicked, this, &MainWindow::runAll);
    v->addWidget(runButton);

    QPushButton* reset = new QPushButton("Reset machine");
    connect(reset, &QPushButton::clicked, this, &MainWindow::resetMachine);
    v->addWidget(reset);

    dumpButton = new QPushButton("Dump memory");
    connect(dumpButton, &QPushButton::clicked, this, &MainWindow::toggleMemory);
    v->addWidget(dumpButton);

    return side;
}

QWidget* MainWindow::buildCenter() {
    QWidget* mid = new QWidget;
    QVBoxLayout* v = new QVBoxLayout(mid);
    v->setContentsMargins(0, 0, 0, 0);
    v->setSpacing(0);

    const QFont mono = QFontDatabase::systemFont(QFontDatabase::FixedFont);

    listing = new QTableWidget(0, 3);
    listing->setHorizontalHeaderLabels({"Addr", "Hex", "Instruction"});
    listing->verticalHeader()->setVisible(false);
    listing->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    listing->setEditTriggers(QAbstractItemView::NoEditTriggers);
    listing->setSelectionMode(QAbstractItemView::NoSelection);
    listing->setShowGrid(false);
    listing->setFont(mono);

    memoryTable = new QTableWidget(16, 16);
    QStringList heads;
    for (int i = 0; i < 16; ++i) heads << hx(i, 1);
    memoryTable->setHorizontalHeaderLabels(heads);
    memoryTable->setVerticalHeaderLabels(heads);
    memoryTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    memoryTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    memoryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    memoryTable->setSelectionMode(QAbstractItemView::NoSelection);
    memoryTable->setShowGrid(false);
    memoryTable->setFont(mono);

    views = new QStackedWidget;
    views->addWidget(listing);
    views->addWidget(memoryTable);
    v->addWidget(views, 1);

    console = new QPlainTextEdit;
    console->setObjectName("console");
    console->setReadOnly(true);
    console->setFont(mono);
    console->setFixedHeight(200);
    v->addWidget(console);

    return mid;
}

QWidget* MainWindow::buildRight() {
    QFrame* right = new QFrame;
    right->setObjectName("right");
    right->setFixedWidth(280);
    QVBoxLayout* v = new QVBoxLayout(right);
    v->setContentsMargins(20, 20, 20, 20);
    v->setSpacing(8);

    const QFont mono = QFontDatabase::systemFont(QFontDatabase::FixedFont);

    QLabel* status = new QLabel("STATUS");
    status->setObjectName("section");
    v->addWidget(status);

    auto addRow = [&](const QString& name, QLabel*& value) {
        QHBoxLayout* row = new QHBoxLayout;
        QLabel* n = new QLabel(name);
        n->setObjectName("caption");
        value = new QLabel("-");
        value->setFont(mono);
        value->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        row->addWidget(n);
        row->addWidget(value, 1);
        v->addLayout(row);
    };
    addRow("PC", pcLabel);
    addRow("IR", irLabel);
    addRow("State", stateLabel);

    QLabel* regs = new QLabel("REGISTERS");
    regs->setObjectName("section");
    v->addWidget(regs);

    QGridLayout* grid = new QGridLayout;
    grid->setVerticalSpacing(4);
    for (int i = 0; i < 16; ++i) {
        regLabels[i] = new QLabel;
        regLabels[i]->setFont(mono);
        grid->addWidget(regLabels[i], i % 8, i / 8);
    }
    v->addLayout(grid);
    v->addStretch(1);

    return right;
}

// ---------- actions ----------

void MainWindow::log(const QString& text) {
    console->appendPlainText(text);
}

void MainWindow::snapshotRegisters() {
    for (int i = 0; i < 16; ++i) previousRegs[i] = machine->getRegister(static_cast<uint8_t>(i));
    havePrevious = true;
}

void MainWindow::insertSample() {
    source->setPlainText("# Prints \"Hi!\" to the screen\n2048\n3000\n2069\n3000\n2021\n3000\nC000\n");
}

void MainWindow::openFile() {
    const QString path = QFileDialog::getOpenFileName(this, "Open program", QString(), "Text files (*.txt);;All files (*)");
    if (path.isEmpty()) return;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        log("> ERROR: cannot open file '" + path + "'");
        return;
    }
    source->setPlainText(QString::fromUtf8(file.readAll()));
}

void MainWindow::loadProgram() {
    showMemory = false;
    unsigned start = 0x0A;
    const QString startText = startEdit->text().trimmed();
    if (!startText.isEmpty()) start = startText.toUInt(nullptr, 16);

    std::vector<uint8_t> program;
    std::string error;
    if (!parseProgram(source->toPlainText().toStdString(), program, error)) {
        log("> ERROR: Load failed: " + QString::fromStdString(error));
        return;
    }

    bool ok;
    std::string output;
    {
        Capture capture;
        ok = machine->loadProgram(program, static_cast<uint8_t>(start));
        output = capture.text.str();
    }
    if (!ok) {
        log("> ERROR: Load failed: program does not fit in memory at 0x" + hx(start, 2));
        refresh();
        return;
    }
    loaded = true;
    startAddress = start;
    programLength = static_cast<unsigned>(program.size());
    havePrevious = false;
    log(QString("> Loaded %1 byte(s) at 0x%2").arg(programLength).arg(hx(start, 2)));
    refresh();
}

void MainWindow::execute(bool all) {
    if (!loaded) {
        log("> Load a program first.");
        return;
    }
    snapshotRegisters();

    std::string output;
    {
        Capture capture;
        if (all) machine->run(); else machine->step();
        output = capture.text.str();
    }
    QString text = QString::fromStdString(output).trimmed();
    if (!text.isEmpty()) log(text);

    const ExecutionStatus status = machine->getStatus();
    if (status == ExecutionStatus::ERROR)
        log("> ERROR: " + QString::fromStdString(machine->getErrorText()));
    else if (status == ExecutionStatus::HALTED)
        log("> Program halted normally (HALT reached).");
    refresh();
}

void MainWindow::stepOnce() { execute(false); }
void MainWindow::runAll() { execute(true); }

void MainWindow::resetMachine() {
    machine->reset();
    loaded = false;
    showMemory = false;
    programLength = 0;
    havePrevious = false;
    log("> Machine reset: registers, PC and memory cleared.");
    refresh();
}

void MainWindow::toggleMemory() {
    showMemory = !showMemory;
    refresh();
}

// ---------- drawing the current machine state ----------

void MainWindow::refresh() {
    const ExecutionStatus status = machine->getStatus();
    const unsigned pc = machine->getPC();

    pcLabel->setText("0x" + hx(pc, 2));
    irLabel->setText("0x" + hx(machine->getIR(), 4));
    stateLabel->setText(loaded || status != ExecutionStatus::CONTINUE ? statusName(status) : "-");
    stateLabel->setStyleSheet(status == ExecutionStatus::HALTED ? "color:#3DDC84;font-weight:700;"
                              : status == ExecutionStatus::ERROR ? "color:#FF6B6B;font-weight:700;" : "");

    for (int i = 0; i < 16; ++i) {
        const int value = machine->getRegister(static_cast<uint8_t>(i));
        regLabels[i]->setText("R" + hx(i, 1) + "   " + hx(value, 2));
        const bool changed = havePrevious && previousRegs[i] != value;
        regLabels[i]->setStyleSheet(changed ? "color:#FDE68A;font-weight:700;" : "");
    }

    // program listing
    listing->setRowCount(0);
    if (loaded) {
        const unsigned end = startAddress + programLength;
        for (unsigned a = startAddress; a < end; a += 2) {
            const int row = listing->rowCount();
            listing->insertRow(row);
            const bool single = a + 1 >= end;
            const unsigned word = (machine->readMemory(static_cast<uint8_t>(a)) << 8) |
                                  (single ? 0 : machine->readMemory(static_cast<uint8_t>(a + 1)));
            listing->setItem(row, 0, new QTableWidgetItem(hx(a, 2)));
            listing->setItem(row, 1, new QTableWidgetItem(single ? hx(word >> 8, 2) : hx(word, 4)));
            listing->setItem(row, 2, new QTableWidgetItem(single ? ".byte" : disassemble(word)));
            for (int c = 0; c < 3; ++c) {
                QTableWidgetItem* item = listing->item(row, c);
                if (c < 2) item->setForeground(QColor("#8493A8"));
                if (a == pc) item->setBackground(QColor("#1F3A2B"));
            }
        }
    }

    // memory grid
    for (int r = 0; r < 16; ++r) {
        for (int c = 0; c < 16; ++c) {
            const unsigned a = static_cast<unsigned>(r * 16 + c);
            const unsigned v = machine->readMemory(static_cast<uint8_t>(a));
            QTableWidgetItem* item = new QTableWidgetItem(hx(v, 2));
            item->setTextAlignment(Qt::AlignCenter);
            const bool inProgram = loaded && a >= startAddress && a < startAddress + programLength;
            item->setForeground(QColor(inProgram ? "#7CB7FF" : v ? "#E6EAF0" : "#5C697C"));
            memoryTable->setItem(r, c, item);
        }
    }

    views->setCurrentIndex(showMemory ? 1 : 0);
    dumpButton->setText(showMemory ? "Show program" : "Dump memory");
    const bool canRun = loaded && status == ExecutionStatus::CONTINUE;
    stepButton->setEnabled(canRun);
    runButton->setEnabled(canRun);
}
