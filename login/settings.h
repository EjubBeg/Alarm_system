#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QComboBox>
#include <QGridLayout>
#include <QStringList>
#include <QDebug>
#include <QMessageBox>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <netdb.h>
#include <unistd.h>
#include <QObject>

class Settings : public QDialog
{
    Q_OBJECT

public:
    Settings(int s);

private:
QLabel * Alarm;

QLabel * Alarmstatus;

QLabel * Set;

QPushButton * AlarmOnOff;

QLabel * state;

QPushButton * Soundsystem;

int socketServer;

char buffer[200];

QMessageBox messageBox;

QGridLayout * formGridLayout;

private slots:

void AON();
void TurnOff();
void AOFF();

};

#endif // SETTINGS_H
