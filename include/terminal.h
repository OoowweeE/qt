#ifndef TERMINAL_H
#define TERMINAL_H

#include <QMainWindow>
#include <QProcess>
#include <QKeyEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class Terminal; }
QT_END_NAMESPACE

class Terminal : public QMainWindow
{
    Q_OBJECT

public:
    Terminal(QWidget *parent = nullptr);
    ~Terminal();
    void AddButton();
    void moveToCenter();//将窗口居中显示
public slots:
    void slot_Pbt_Send();
    void slot_process_Output();
protected:
    void keyPressEvent(QKeyEvent *event) override;


private slots:
    void on_pbt_ctrl_C_clicked();

    void on_pbt_clc_clicked();







    void on_pbt_HistoryShow_clicked();

    void on_pbt_saveCMD_clicked();

private:
    Ui::Terminal *ui;
    QProcess *process;

    bool HistoryShowState = 0;
    int buttonCount; // 记录当前按钮的数量
};
#endif // TERMINAL_H
