#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent, Acciii *acciii_)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), sampleTime(1.0)
{
    ui->setupUi(this);
    //acciii = new Acciii;
    acciii = acciii_;

    dispSensorInd = {2, 32, 63, 92, 134};
}

MainWindow::~MainWindow()
{
    delete ui;
    delete acciii;
}

void MainWindow::plotData(){

    QVector<double> value(dataSetNum), sampleNum(dataSetNum); // initialize vector for plotting

    // For x sensors, 3x channels, plot dataSetNum number of data
    for (int countSensor = 0; countSensor < dispSensorInd.size(); countSensor++){
        // Plot only first 3 channels,
            ui -> customPlot -> addGraph();
            for(int countDataNum = 0; countDataNum < dataSetNum; countDataNum++){
                value[countDataNum] = data_buffer[countDataNum][dispSensorInd[countSensor]] + 16 * countSensor;

                sampleNum[countDataNum] = (double)countDataNum/fs;
            }

            ui->customPlot->graph(countSensor)->setData(sampleNum,value,true);
        }

    ui->customPlot->xAxis->setRange(0,sampleNum[sampleNum.size()-1]); // Specify the time range
    ui->customPlot->yAxis->setRange(-16, 16*dispSensorInd.size()); // Specify the measurement range +/- 16 g (gravity)

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
