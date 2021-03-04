#include "logindialog.h"

LoginDialog::LoginDialog()
{
    connection();
    setDialog();
    setWindowTitle(tr("Login"));
    setModal(true);
}

void LoginDialog::setDialog()
{

    formGridLayout = new QGridLayout(this);

    editUsername = new QLineEdit(this);
    editUsername->setEchoMode(QLineEdit::Normal);

    editPassword = new QLineEdit(this);
    editPassword->setEchoMode(QLineEdit::Password);

    labelUsername = new QLabel(this);
    labelPassword = new QLabel(this);
    labelRepeat = new QLabel(this);
    labelUsername->setText(tr("Username"));
    labelUsername->setBuddy(editUsername);
    labelPassword->setText(tr("Password"));
    labelPassword->setBuddy(editPassword);

    buttons = new QDialogButtonBox(this);
    buttons->addButton(QDialogButtonBox::Ok);
    buttons->addButton(QDialogButtonBox::Cancel);
    buttons->button(QDialogButtonBox::Ok)->setText(tr("Register"));
    buttons->button(QDialogButtonBox::Cancel)->setText(tr("Quit"));

    connect(buttons->button(QDialogButtonBox::Cancel),
            SIGNAL(clicked()),
            this,
            SLOT(close()));

    connect(buttons->button(QDialogButtonBox::Ok),
            SIGNAL(clicked()),
            this,
            SLOT(checkwords()));

    formGridLayout->addWidget(labelUsername, 0, 0);
    formGridLayout->addWidget(editUsername, 0, 1);
    formGridLayout->addWidget(labelPassword, 1, 0);
    formGridLayout->addWidget(editPassword, 1, 1);
    formGridLayout->addWidget(buttons, 2, 0, 1, 2);

    setLayout(formGridLayout);
}

void LoginDialog::checkwords()
{
    char message[100];
    QString u = editUsername->text();
    QString p = editPassword->text();

    QByteArray latinuser = u.toLatin1();
    QByteArray latinpass = p.toLatin1();

    char *username = latinuser.data();
    char *password = latinpass.data();
    strcat(message, username);
    strcat(message, " ");
    strcat(message, password);
    write(sockfd, "Check Pass", sizeof("Check Pass"));
    char buffer[20];
    read(sockfd, buffer, sizeof(buffer));
    if (strncmp(buffer, "Ok", 2) == 0)
    {
        write(sockfd, message, sizeof(message));
    }
    else
    {
        messageBox.critical(0, "Error", "Server does not answere");
        messageBox.setFixedSize(300, 200);
    }
    bzero(buffer, sizeof(buffer));
    read(sockfd, buffer, sizeof(buffer));
    if (strncmp(buffer, "Right", 5) == 0)
    {
        Settings *s = new Settings(sockfd);
        s->show();
    }
    else
    {
        messageBox.critical(0, "Error", "False password or username!");
        messageBox.setFixedSize(300, 200);
    }
}

void LoginDialog::connection()
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        qDebug() << "socket creation failed...\n";
        exit(0);
    }
    else
        qDebug() << "Socket successfully created..\n";
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("ip adrres of raspi pi");
    servaddr.sin_port = htons(PORT);

    if (::connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0)
    {
        qDebug() << "connection with the server failed...\n";
        exit(0);
    }
    else
        qDebug() << "connected to the server..\n";
}

void LoginDialog::opne()
{
}
