#include "biblioteke.h"


int serial_port;
int sockfd, connfd, len; 
struct sockaddr_in servaddr, cli; 
pthread_t threadarduino , threadserver;
sqlite3 *db;
char *err_msg = 0;
int rc;
char sql[100];
User gazda;
int server_fd, new_socket, valread; 
struct sockaddr_in address; 
int opt = 1; 
int addrlen = sizeof(address); 
char *hello = "Received message"; 
bool alarmState;
char alarmReason;
bool updated;
pthread_mutex_t mutex1 ;



int main()
{

open_Arduino();
set_server();
set_database();
setdefaults();

pthread_t threadarduino ,threadserver ;
int tr2 ,tr1;

tr1 = pthread_create(&threadarduino, NULL, listenArduino , NULL);
    if(tr1)
    {
        fprintf(stderr,"Error - pthread_create() return code: %d\n",tr1);
        exit(EXIT_FAILURE);   
    }
tr2 = pthread_create(&threadserver, NULL, listenClient, NULL);
    if(tr2)
    {
        fprintf(stderr,"Error - pthread_create() return code: %d\n",tr2);
        exit(EXIT_FAILURE);
    }


pthread_join(threadarduino, NULL);
pthread_join(threadserver, NULL);


return 0;

}

void set_database()
{

rc = sqlite3_open("basa.db", &db);
    
    if (rc != SQLITE_OK) 
    {
        
        fprintf(stderr, "Canot open database: %s\n", sqlite3_errmsg(db));
        
        sqlite3_close(db);

         exit(1);
    }
 }


void set_server(){
  sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully created..\n"); 
	bzero(&servaddr, sizeof(servaddr)); 

	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port = htons(PORT); 

	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
		printf("socket bind failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully binded..\n"); 

	if ((listen(sockfd, 5)) != 0) { 
		printf("Listen failed...\n"); 
		exit(0); 
	} 
	else
		printf("Server listening..\n"); 
}


void open_Arduino()
{
serial_port = open("/dev/ttyACM0",  O_RDWR );

struct termios tty;

memset(&tty, 0, sizeof tty);

    if(tcgetattr(serial_port, &tty) != 0) 
    {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
    }

tty.c_cflag &= ~PARENB;
tty.c_cflag &= ~CSTOPB;
tty.c_cflag |= CS8; 
//tty.c_cflag &= ~CRTSCTS;
tty.c_cflag |= CREAD | CLOCAL;

tty.c_lflag &= ~ICANON;
tty.c_lflag &= ~ECHO; 
tty.c_lflag &= ~ECHOE; 
tty.c_lflag &= ~ECHONL;
tty.c_lflag &= ~ISIG; 
tty.c_iflag &= ~(IXON | IXOFF | IXANY); 
tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); 

tty.c_oflag &= ~OPOST; 
tty.c_oflag &= ~ONLCR; 


tty.c_cc[VTIME] =1;
tty.c_cc[VMIN] = 5;

cfsetispeed(&tty, B9600);
cfsetospeed(&tty, B9600);

    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) 
    {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
    }
char b[10];
    while(1)
    {
        bzero(b,sizeof(b));
        int a = read(serial_port, &b, sizeof(b));
        if(a<0)
            printf("Error reading: %s", strerror(errno));

        if(strcmp(b,"start")==0)
        {
        write(serial_port,"Connected",strlen("Connected"));
        break;
        }
    }
}


void * listenArduino()
{

char BuffArduino[10];
char password[20];
int num_bytes;
    while(1)
    {
        bzero(BuffArduino ,sizeof(BuffArduino));
        num_bytes = read(serial_port, &BuffArduino, sizeof(BuffArduino));

        if (num_bytes < 0) 
        {
            printf("Error reading: %s", strerror(errno));
        }

        if(strncmp(BuffArduino , "Lopov",5)==0)
        {
            alarmReason = 'L';
            printf("Lopov\n");
            pthread_t camerathread;
            int c = pthread_create(&camerathread, NULL, camera, NULL);
            if(c)
            {
                fprintf(stderr,"Error - pthread_create() return code: %d\n",c);
                exit(EXIT_FAILURE);
            }
            pthread_join(camerathread, NULL);
        }
        else if(strncmp(BuffArduino , "Vatra",5)==0)
        {
            alarmReason = 'V';
            printf("Vatra\n");
        }
        else if(strncmp(BuffArduino , "Times",5)==0)
        {
            alarmReason = 'T';
            printf("Times\n");
        }
        else if(strncmp(BuffArduino , "Check",5)==0)
        {
            bzero(password, sizeof(password));
            int i = 0;
            int rest = strlen(BuffArduino)-5;
            for(i = 0;i<rest;i++)
                password[i]= BuffArduino[5+i];
            bzero(BuffArduino, sizeof(BuffArduino));
            while(rest<10)
            {
                read(serial_port, &BuffArduino, sizeof(BuffArduino));
                int s = strlen(BuffArduino);
                for(int j = 0; j<s;j++)
                {
                    password[rest]= BuffArduino[j];
                    rest++;
                    if(rest == 10)
                    break;
                }
            }
            if(Checkentrance(password)==1)
            {
                write(serial_port,"Right",strlen("Right"));
                InsertLogin();
            }
            else
            {
                write(serial_port,"False",strlen("False"));
            }
        } 
    }
}


void * listenClient() 
{ 
bool stop= false;

    while(1)
    {
        char bufferClient[MAX]; 

        len = sizeof(cli); 

        connfd = accept(sockfd, (SA*)&cli, &len); 

        if (connfd < 0) 
        { 
            printf("server acccept failed...\n"); 
            exit(0); 
        } 
        else
        {
            while(1)
            {
                bzero(bufferClient, MAX); 

                read(connfd, bufferClient, sizeof(bufferClient)); 

                if (strncmp("exit", bufferClient, 4) == 0) 
                { 
                    stop = true; 
                    break;
                } 

                if (strncmp("Check Pass", bufferClient, 10 ) == 0) 
                { 
                
                write(connfd , "Ok" , sizeof("Ok"));
                bzero(bufferClient, MAX); 
                read(connfd, bufferClient, sizeof(bufferClient)); 

                int n= 0;
                char p[20];
                char u[20];

                while(bufferClient[n]!=' ')
                {
                    u[n] = bufferClient[n];
                    n++;       
                }

                n++;
                int j= 0;

                while((bufferClient[n]!='\0')&&(bufferClient[n]!='\n'))
                {
                    u[j] = bufferClient[n];
                    n++;       
                    j++;
                }
                if(Checklogin(u,p))
                {
                updateClient();
                break;
                }
                else
                {
                continue;
                }
                }           
            }
            while(1) 
            { 
                bzero(bufferClient, MAX); 

                read(connfd, bufferClient, sizeof(bufferClient)); 

                printf("From client: %s\n", bufferClient); 

                if(stop==true)
                {
                    stop= false;
                    break;
                }

                if (strncmp("exit", bufferClient, 4) == 0) 
                { 
                    break; 
                }  
                else if (strncmp("AlarmOn", bufferClient,7) == 0)
                {
                    write(serial_port , "Alarm",strlen("Alarm"));
                    alarmState= true;
                }
                else if(strncmp("AlarmOff", bufferClient,8) == 0)
                {
                    write(serial_port , "AlarmOff" , strlen("AlarmOff"));
                    alarmState = false;
                }
                else if (strncmp("TurnSoundOff", bufferClient,12) == 0)
                {
                    write(serial_port , "SoundOff",strlen("SoundOff"));
                    alarmState= true;
                }
            }
        }
    }
}

int Checklogin(char * username , char * pass )
{
updated = false;
sprintf(sql, "SELECT * FROM Users WHERE Username = '%s'",username);
rc = sqlite3_exec(db, sql, assigning, 0, &err_msg);

    if (rc != SQLITE_OK ) 
    {
        fprintf(stderr, "SQL error: %s\n", err_msg);    
     
        sqlite3_free(err_msg);        
    } 
    else
    {
    fprintf(stdout , "Soucessfully Seelcted \n");
    }

    if(updated)
    {
        if(strcmp(pass, gazda.Password))
            return 1;
        else
            return 0;
    }
    else 
        return 0;

}



int Checkentrance(char * pass)
{
updated= false;
sprintf(sql, "SELECT * FROM Users WHERE Entrance = '%s'",pass);
rc = sqlite3_exec(db, sql, assigning, 0, &err_msg);

    if (rc != SQLITE_OK ) 
    {
        fprintf(stderr, "SQL error: %s\n", err_msg);    
     
        sqlite3_free(err_msg);        
    } 
    else
    {
            fprintf(stdout , "Soucessfully Seelcted \n");
    }

    if(updated)
        return 1;
    else 
        return 0;

}

int assigning(void *NotUsed, int argc, char **argv, char **azColName) 
{
pthread_mutex_lock(&mutex1);

updated = true;

strcpy(gazda.ID,argv[0]);
strcpy(gazda.Name,argv[1]);
strcpy(gazda.Password,argv[2]);
strcpy(gazda.Entrance,argv[3]);

pthread_mutex_unlock(&mutex1);

return 0;

}

void InsertLogin()
{

time_t t = time(NULL);
struct tm *tm = localtime(&t);
char s[64];
assert(strftime(s, sizeof(s), "%c", tm));
    
sprintf(sql, "INSERT INTO EntLogin(Username, Time) VALUES( '%s' , '%s');",gazda.Name , s);

rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK ) 
    {
        
        fprintf(stderr, "SQL error: %s\n", err_msg);
        
        sqlite3_free(err_msg);        
       
    } 
    else
    {
        fprintf(stdout , "Soucessfully inserted into login\n");
    }
}

void setdefaults()
{
alarmReason = 'S';

alarmState = false;
}

void * camera() 
{
char sys[200];

    for (int j = 0; j < 60 ;j++)
    {
        sprintf(sys, "raspistill -o /home/pi/Desktop/Project/camera/image%d.jpg -t 10 -n -ex auto -awb auto -w 800 -h 600", j);
        system(sys);
        sleep(500);
    }
}

void updateClient()
{
char message[80];
    if(alarmState== true)
    {
        strcat(message, "AlarmOn ");
    }
    else
    {
        strcat(message, "AlarmOff ");
    }
size_t len = strlen(message);
snprintf(message + len, sizeof message - len, "%c", alarmReason);
write(connfd, message, sizeof(message));
}


