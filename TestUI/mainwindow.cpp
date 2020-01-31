#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::plotData(std::vector<std::vector<double>> data_buffer, int dataSetNum, float fs){

    QVector<double> value(dataSetNum), sampleNum(dataSetNum); // initialize vector for plotting

    ui->customPlot->addGraph();

    // Plot only first 3 channels, plot dataSetNum number of data
    for (int countSensor = 0; countSensor < 3; countSensor++){
        for(int countDataNum = 0; countDataNum < dataSetNum; countDataNum++){
            value[countDataNum] = data_buffer[countDataNum][countSensor];
            sampleNum[countDataNum] = (double)countDataNum/fs;
        }

        ui->customPlot->graph(0)->addData(sampleNum,value);
    }

    ui->customPlot->xAxis->setRange(0,sampleNum[sampleNum.size()-1]); // Specify the time range
    ui->customPlot->yAxis->setRange(-16,16); // Specify the measurement range +/- 16 g (gravity)

    //ui->customPlot->graph(0)->rescaleValueAxis();

    ui->customPlot->replot();

    return;
}
