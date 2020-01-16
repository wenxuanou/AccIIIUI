#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

MainWindow::MainWindow(int argc, char** argv, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    acciii(argc,argv)       // initiate USB transmission
{
    ui->setupUi(this);
    setGeometry(400, 250, 542, 390);
    setupPlot(ui->customPlot);
    //ui->customPlot->replot();

}

MainWindow::~MainWindow()
{
    delete ui;
}

// For static ploting
void MainWindow::setupPlot(QCustomPlot *customPlot){
    // create graph and assign data to it:
    customPlot->addGraph();
    // give the axes some labels:
    customPlot->xAxis->setLabel("sample");
    customPlot->yAxis->setLabel("values");
    // set axes ranges, so we see all data:
    customPlot->xAxis->setRange(-1, 1);
    customPlot->yAxis->setRange(0, 1);
}

/*
// For real time plotting
void MainWindow::setupPlot(QCustomPlot *customPlot){

    QTimer *dataTimer = new QTimer(this);

    // ploting
    customPlot->addGraph(); // blue line
    customPlot->graph(0)->setPen(QPen(QColor(40, 110, 255)));

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    customPlot->xAxis->setTicker(timeTicker);
    customPlot->axisRect()->setupFullAxesBox();
    customPlot->yAxis->setRange(-1.2, 1.2);

    // make left and bottom axes transfer their ranges to right and top axes:
    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

    // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
    connect(dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));

    dataTimer->start(0); // Interval 0 means to refresh as fast as possible

}
*/

/*
// For real time plotting only
void MainWindow::realtimeDataSlot(){

    static QTime time(QTime::currentTime());
    // calculate two new data points:
    double key = time.elapsed()/1000.0; // time elapsed since start of demo, in seconds
    static double lastPointKey = 0;

    if (key-lastPointKey > 0.002) // at most add point every 2 ms
    {
        // obtain data from USB
        //getData();
        // add data to lines:
        ui->customPlot->graph(0)->addData(key,___);     // Get external data input

        // rescale value (vertical) axis to fit the current data:
        ui->customPlot->graph(0)->rescaleValueAxis();
        lastPointKey = key;
    }

    // make key axis range scroll with the data (at a constant range size of 8):
    ui->customPlot->xAxis->setRange(key, 8, Qt::AlignRight);
    ui->customPlot->replot();

    // calculate frames per second:
    static double lastFpsKey;
    static int frameCount;
    ++frameCount;
    if (key-lastFpsKey > 2) // average fps over 2 seconds
    {
        ui->statusBar->showMessage(
                    QString("%1 FPS, Total Data points: %2")
                    .arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
                    .arg(ui->customPlot->graph(0)->data()->size())
                    , 0);
        lastFpsKey = key;
        frameCount = 0;
    }
}
*/

void MainWindow::plotData(QCustomPlot *customPlot){

    std::vector<std::vector<float>> data_buffer = acciii.printData();       // Get data from USB
    int dataSetNum = acciii.printDataSetNum();
    QVector<double> value(dataSetNum), sampleNum(dataSetNum); // initialize vector for plotting

    /*
    // Plot for each channel(138 total, 138 = 3 * 46), plot dataSetNum number of data
    for (int countSensor = 0; countSensor < 138; countSensor++){
        for(int countDataNum = 0; countDataNum < dataSetNum; countDataNum++){
            value.push_back(data_buffer[countDataNum][countSensor]);
            sampleNum.push_back(countDataNum);
        }
        customPlot->graph(0)->addData(sampleNum,value);
        value.erase(value.begin(),value.end());
        sampleNum.erase(sampleNum.begin(),sampleNum.end());
    }
    */

    // Plot only first 3 channels, plot dataSetNum number of data
    for (int countSensor = 0; countSensor < 3; countSensor++){
        for(int countDataNum = 0; countDataNum < dataSetNum; countDataNum++){
            value.push_back(data_buffer[countDataNum][countSensor]);
            sampleNum.push_back(countDataNum);
        }
        customPlot->graph(0)->addData(sampleNum,value);
        value.erase(value.begin(),value.end());
        sampleNum.erase(sampleNum.begin(),sampleNum.end());
    }

    ui->customPlot->graph(0)->rescaleValueAxis();
    ui->customPlot->replot();

}


void MainWindow::on_StartButton_clicked()
{
    float input = ui->sampleTime->text().toFloat();

    if(input > 0){
        acciii.setSamplingTime(input);
    }

    acciii.transmitData();
}

void MainWindow::on_PlotButton_clicked()
{
    plotData(ui->customPlot);
}
