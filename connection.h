#ifndef CONNECTION_H
#define CONNECTION_H

#include <QDialog>

namespace Ui {
class TConnection;
}

class TConnection : public QDialog
{
    Q_OBJECT

public:
    explicit TConnection(QWidget *parent = nullptr);
   // TConnection(const TConnection& tc);
    ~TConnection();

    QString GetAddr();
    unsigned int GetPort();

private:
    Ui::TConnection *ui;
};

#endif // CONNECTION_H
