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

void MainWindow::plotData(std::vector<std::vector<double>> data_buffer, int dataSetNum){

    QVector<double> value(dataSetNum), sampleNum(dataSetNum); // initialize vector for plotting

    ui -> customPlot -> addGraph();

    // Plot only first 3 channels, plot dataSetNum number of data
    for (int countSensor = 0; countSensor < 3; countSensor++){
        for(int countDataNum = 0; countDataNum < dataSetNum; countDataNum++){
            value[countDataNum] = data_buffer[countDataNum][countSensor];
            sampleNum[countDataNum] = (double)countDataNum/1000.0;
        }

        ui->customPlot->graph(0)->addData(sampleNum,value);
    }
/*
    for(int i = 0; i < dataSetNum; i++)
    {
        value[i] = (double)i/50.0 -1;
        sampleNum[i] = value[i]*value[i];
    }
    ui->customPlot->graph(0)->addData(value,sampleNum);
    ui->customPlot->xAxis->setRange(-1,1);
    ui->customPlot->yAxis->setRange(0,1);
*/

    ui->customPlot->graph(0)->rescaleValueAxis();
    ui->customPlot->replot();

}
