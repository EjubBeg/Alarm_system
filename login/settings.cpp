#include "settings.h"

Settings::Settings(int s)
{
    socketServer= s;
    char alarmstatus[10] = "AlarmOn";
    char housestatus= 'S';

    int i = read(socketServer, buffer, sizeof(buffer));
if(i<0)
{
    messageBox.critical(0,"Error","Problems with server!");
    messageBox.setFixedSize(300,200);
}

int n= 0;
while(buffer[n]!= ' ')
{
    alarmstatus[n]= buffer[n];
    n++;
}
housestatus= buffer[++n];

formGridLayout = new QGridLayout( this );

Alarm = new QLabel(this);
Alarm->setText( tr( "Alarm : " ) );
Alarmstatus= new QLabel(this);
if(strncmp(alarmstatus,"AlarmOn",7)==0)
{
Alarmstatus->setText(tr( "Alarm Works" ));
AlarmOnOff = new QPushButton("&OFF", this);
connect(AlarmOnOff, SIGNAL(clicked(bool)),this,SLOT(AON()));
}
else if(strncmp(alarmstatus,"AlarmOff",8)==0)
{
Alarmstatus->setText(tr( "Alarm isnt Working" ));
AlarmOnOff = new QPushButton("&ON", this);
connect(AlarmOnOff, SIGNAL(clicked(bool)),this,SLOT(AOFF()));
}

Set= new QLabel(this);
Set->setText(tr("Set alarm :"));

formGridLayout->addWidget(Alarm, 0,0);
formGridLayout->addWidget(Alarmstatus,0,1);
formGridLayout->addWidget(Set , 1,0);
formGridLayout->addWidget(AlarmOnOff);

state = new QLabel(this);

if(housestatus== 'S')
{
    state->setText(tr("Safe"));
    formGridLayout->addWidget(state, 2,0);

}

if(housestatus== 'V')
{
    state->setText(tr("Fire"));
    formGridLayout->addWidget(state, 2,0);

    Soundsystem= new QPushButton("&SoundOff",this);
    connect(Soundsystem, SIGNAL(clicked(bool)),this,SLOT(TurnOff()));
    formGridLayout->addWidget(Soundsystem,2,1);

}

if(housestatus== 'T')
{
    state->setText(tr("To much wrong entries"));
    formGridLayout->addWidget(state, 2,0);

    Soundsystem= new QPushButton("&SoundOff",this);
    connect(Soundsystem, SIGNAL(clicked(bool)),this,SLOT(TurnOff()));
    formGridLayout->addWidget(Soundsystem,2,1);

}

if(housestatus== 'L')
{
    state->setText(tr("Burgler"));
    formGridLayout->addWidget(state, 2,0);
    Soundsystem= new QPushButton("&SoundOff",this);
    connect(Soundsystem, SIGNAL(clicked(bool)),this,SLOT(TurnOff()));
    formGridLayout->addWidget(Soundsystem,2,1);
    setLayout( formGridLayout );

}



}



void Settings::AON()
{
 write(socketServer, "AlarmOn",sizeof("AlarmOn"));
}

void Settings::TurnOff()
{
write(socketServer, "TurnSoundOff",sizeof("TurnSoundOff"));
}

void Settings::AOFF()
{
write(socketServer, "AlarmOff",sizeof("AlarmOff"));
}

















