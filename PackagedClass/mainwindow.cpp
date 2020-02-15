#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent, Acciii *acciii_)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), sampleTime(1.0)
{
    ui->setupUi(this);
    //acciii = new Acciii;
    acciii = acciii_;

}

MainWindow::~MainWindow()
{
    delete ui;
    delete acciii;
}

void MainWindow::plotData(){


    QVector<double> value(dataSetNum), sampleNum(dataSetNum); // initialize vector for plotting

    // For 10 sensors, 30 channels, plot dataSetNum number of data
    for (int countSensor = 0; countSensor < 30; countSensor++){
        // Plot only first 3 channels,
            ui -> customPlot -> addGraph();
            for(int countDataNum = 0; countDataNum < dataSetNum; countDataNum++){
                value[countDataNum] = data_buffer[countDataNum][countSensor] + 16 * countSensor;

                sampleNum[countDataNum] = (double)countDataNum/fs;
            }

            ui->customPlot->graph(countSensor)->setData(sampleNum,value,true);
        }


    /*
    double value, sampleNum;

    // For 10 sensors, 30 channels, plot dataSetNum number of data
    for (int countSensor = 0; countSensor < 30; countSensor++){
        // Plot only first 3 channels,
            ui -> customPlot -> addGraph();
            for(int countDataNum = 0; countDataNum < dataSetNum; countDataNum++){
                value = data_buffer[countDataNum][countSensor] + 16 * countSensor;
                sampleNum = (double)countDataNum/fs;

                ui->customPlot->graph(countSensor)->addData(sampleNum,value);   // add data one by one
            }
    }
    */

    ui->customPlot->xAxis->setRange(0,sampleNum[sampleNum.size()-1]); // Specify the time range
    ui->customPlot->yAxis->setRange(-16, 500); // Specify the measurement range +/- 16 g (gravity)

    ui->customPlot->replot();

    return;
}

void MainWindow::on_startButton_clicked()
{
    sampleTime = ui->sampleTime->text().toFloat();

    acciii->setSampleTime(sampleTime);
    acciii->sampleData();

    data_buffer = acciii->decodeData();
    dataSetNum = acciii->returnDataSetNum();
    fs = acciii->returnIdDataRate();

    //plotData(acciii->decodeData(), acciii->returnDataSetNum(), acciii->returnIdDataRate());
    plotData();

}

//float MainWindow::returnSampleTime(){
//    return sampleTime;
//}
