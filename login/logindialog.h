#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

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
#include "settings.h"

#define MAX 80
#define PORT 8080
#define SA struct sockaddr



class LoginDialog : public QDialog
{

 Q_OBJECT

private:

 QLabel* labelUsername;

 QLabel* labelPassword;

 QLabel* labelRepeat;

 QLineEdit* editUsername;

 QLineEdit* editPassword;

 QLineEdit* editRepeat;

 QDialogButtonBox* buttons;

 QGridLayout* formGridLayout;

 QMessageBox messageBox;

 void setDialog();

 int sockfd;
 struct sockaddr_in servaddr, cli;

public:

LoginDialog();
void connection();
public slots:
 void opne();
 void checkwords();

};


#endif // LOGINDIALOG_H
