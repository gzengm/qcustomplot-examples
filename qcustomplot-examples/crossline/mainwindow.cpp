#include "mainwindow.h"
#include "../common/customplot.h"
#include "../common/crossline.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QComboBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    CustomPlot *customPlot = new CustomPlot;

    customPlot->addGraph(customPlot->xAxis, customPlot->yAxis);
    customPlot->graph()->setPen(QPen(Qt::blue));
    customPlot->graph()->setBrush(QBrush(QColor(0, 0, 255, 20)));

    QVector<double> x(251), y0(251);
    for (int i = 0; i < 251; ++i) {
        x[i] = i;
        y0[i] = qExp(-i / 150.0) * qCos(i / 10.0);
    }
    customPlot->graph()->setData(x, y0);
    customPlot->graph()->rescaleAxes();
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectItems);

    CrossLine *crossLine = new CrossLine(customPlot, customPlot->graph());

    QComboBox *comboBox = new QComboBox;
    comboBox->addItems(QStringList() << "Free" << "Follow Cursor" << "Tracing");

    QPushButton *btnAddHLine = new QPushButton("Add HLine");
    connect(btnAddHLine, &QPushButton::clicked, [crossLine](){
        crossLine->addHLine();
    });

    QPushButton *btnAddVLine = new QPushButton("Add VLine");
    connect(btnAddVLine, &QPushButton::clicked, [crossLine](){
        crossLine->addVLine();
    });

    QPushButton *btnAddTracer = new QPushButton("Add Tracer");
    connect(btnAddTracer, &QPushButton::clicked, [crossLine](){
        crossLine->addTracer();
    });

    QPushButton *btnClearHLines = new QPushButton("Clear HLines");
    connect(btnClearHLines, &QPushButton::clicked, [crossLine](){
        crossLine->clearHLines();
    });

    QPushButton *btnClearVLines = new QPushButton("Clear VLines");
    connect(btnClearVLines, &QPushButton::clicked, [crossLine](){
        crossLine->clearVLines();
    });

    QPushButton *btnClearTracers = new QPushButton("Clear Tracers");
    connect(btnClearTracers, &QPushButton::clicked, [crossLine](){
        crossLine->clearTracers();
    });

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(comboBox);
    hLayout->addWidget(btnAddHLine);
    hLayout->addWidget(btnAddVLine);
    hLayout->addWidget(btnAddTracer);
    hLayout->addWidget(btnClearHLines);
    hLayout->addWidget(btnClearVLines);
    hLayout->addWidget(btnClearTracers);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(hLayout);
    layout->addWidget(customPlot);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);
    setCentralWidget(widget);

    connect(comboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),  [crossLine](int index) {
        crossLine->setLineMode(CrossLine::LineMode(index));
    });

    // we don't want to drag when item is selected.
    connect(customPlot, &QCustomPlot::mousePress, [customPlot](QMouseEvent *event) {
        if (customPlot->itemAt(event->localPos()) && !customPlot->selectedItems().isEmpty())
            customPlot->setInteractions(QCP::iRangeZoom | QCP::iSelectItems);
    });
    connect(customPlot, &QCustomPlot::mouseRelease, [customPlot](){
        customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectItems);
    });

    showMaximized();
}

MainWindow::~MainWindow()
{
}
