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
    void realtimeDataSlot();    // slot to refresh plot

private:
    Ui::MainWindow *ui;
    AccIII acciii;     // USB transmission object

    // Plotting functions
    void setupPlot(QCustomPlot *customPlot);
    void getData();

    double data;    // USB Data


};

