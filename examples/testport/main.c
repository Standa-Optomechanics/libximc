#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <sys/ioctl.h>

int
command (int fd, const char* cmd, int resplen)
{
	int n,i,j,k=0;
	int amount;
	char buf[48], data[48];

// blocking
	printf( "sending command... ");
	for (i = 0; i < k; ++i)
		printf( "0x%x(%c) ", data[i], data[i] );
	printf("\n");

	n = write(fd, cmd, 4);
	if (n != 4)
		printf("write() failed with %d!\n", n);

	printf( "reading answer...\n");

	for (k = 0; k < resplen;)
	{
		n = read(fd, data+k, resplen-k );
		if (n < 0)
			break;
		if (n == 0 && k < resplen)
		{
			printf(".");
		//	usleep( 20*1000 );
		//	continue;
			break;
		}

		k += n;
		printf( "read %d bytes... ", n );
	}

	for (i = 0; i < k; ++i)
		printf( "0x%x(%c) ", data[i], data[i] );
	printf("\n");
}

int
main(int argc, char* argv[])
{
	int fd;
	struct termios options;
	int read_amount;
	char* cmd_string;

	//if (argc != 3)
	//	return 1;

	cmd_string = argv[1];
	sscanf( argv[2], "%d", &read_amount );

	fd = open("/dev/tty.usbserial-001", O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (fd == -1)
	{
		perror("open_port: Unable to open device");
		return 1;
	}
	printf( "opened\n" );

	fcntl(fd, F_SETFL, 0);

	tcgetattr(fd, &options);

	cfsetispeed(&options, B115200);
	cfsetospeed(&options, B115200);

	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

	options.c_cflag |= (CLOCAL | CREAD);
 	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;
	options.c_cflag &= ~(PARENB | PARODD);
	options.c_cflag |= CSTOPB;
	options.c_cflag &= ~CRTSCTS;

	options.c_iflag &= ~(IXON | IXOFF | IXANY);
	options.c_iflag &= ~(INPCK | PARMRK | ISTRIP | IGNPAR);
	options.c_iflag &= ~(IGNBRK | BRKINT | INLCR | IGNCR | ICRNL | IMAXBEL);

	options.c_oflag &= ~OPOST;

	options.c_cc[VMIN] = 0;
	options.c_cc[VTIME] = 4; // 400 msec

	printf( "control %x\n", options.c_cflag );
	printf( "local %x\n", options.c_lflag );
	printf( "input %x\n", options.c_iflag );
	printf( "output %x\n", options.c_oflag );

	tcsetattr(fd, TCSAFLUSH, &options);

	tcflush(fd, TCIOFLUSH);

	// command(fd, cmd_string, read_amount);
	// usleep( 2*1000*1000 );

	command(fd, "init", 4); usleep( 5*1000*1000 );
	command(fd, "gets", 24); usleep( 5*1000*1000 );
	command(fd, "gets", 24); usleep( 5*1000*1000 );
	command(fd, "left", 4); usleep( 5*1000*1000 );
	command(fd, "stop", 4); usleep( 5*1000*1000 );
	command(fd, "init", 4); usleep( 5*1000*1000 );

	close(fd);
}

