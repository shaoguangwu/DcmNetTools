#pragma once

#include <QWidget>
#include "ui_EchoStoreSCP.h"

class EchoStoreSCP : public QWidget
{
    Q_OBJECT

public:
    EchoStoreSCP(QWidget *parent = Q_NULLPTR);
    ~EchoStoreSCP();

private:
    Ui::EchoStoreSCP ui;
};
