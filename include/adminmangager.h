#ifndef ADMINMANGAGER_H
#define ADMINMANGAGER_H

#include <QWidget>

namespace Ui {
class adminMangager;
}

class adminMangager : public QWidget
{
    Q_OBJECT

public:
    explicit adminMangager(QWidget *parent = nullptr);
    ~adminMangager();

private:
    Ui::adminMangager *ui;
};

#endif // ADMINMANGAGER_H
