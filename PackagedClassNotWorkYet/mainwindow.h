#include <QMainWindow>
#include <QTimer>
#include "qcustomplot.h"
#include "acciii.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(int argc, char** argv, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    //void realtimeDataSlot();    // slot to refresh plot

    void on_StartButton_clicked();      // Start sampling button clicked


    void on_PlotButton_clicked();       // Plot button clicked

private:
    Ui::MainWindow *ui;
    AccIII acciii;     // USB transmission object

    // Plotting functions
    void setupPlot(QCustomPlot *customPlot);
    void plotData(QCustomPlot *customPlot);

};

