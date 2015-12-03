/*
 * SPI testing utility (using spidev driver)
 *
 * Copyright (c) 2007  MontaVista Software, Inc.
 * Copyright (c) 2007  Anton Vorontsov <avorontsov@ru.mvista.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 * Cross-compile with cross-gcc -I/path/to/cross-kernel/include
 * gcc -Wall -o znp znp.c -lwiringPi
 */

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <wiringPi.h>

#define CMD_POLL {0x00, 0x00, 0x00}
#define CMD_POLL2 {0x00, 0x00, 0x00, 0x00}
#define ZB_READ_CONFIGURATION {0x01, 0x26, 0x04, 0x44}
#define GET_CONF_RESPONCE {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
//#define ZB_WRITE {0xFE, 0x00, 0x21, 0x02}
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#define CFG0 0
#define CFG1 1
#define RESET 1
#define SRDY 3
#define MRDY 4


static void transferPoll(int fd);
static void transferCmd(int fd, uint8_t tx[]);
static void transferSreq(uint8_t tx[], uint8_t rx[]);
static void transfer(int fd);
static void poll();

int fd;
uint8_t get_conf[] = GET_CONF;
uint8_t get_conf_responce[] = GET_CONF_RESPONCE;
static void pabort(const char *s)
{
	perror(s);
	abort();
}

static const char *device = "/dev/spidev0.0";
static uint8_t mode;
static uint8_t bits = 8;
static uint32_t speed = 3900000;
static uint16_t delaySpi;

static void poll(){
	while(digitalRead(SRDY)){
	}
	printf("POLL: SRDY is low seting MRDY low\n");
        digitalWrite (MRDY, LOW) ;
	delay(200);
	printf("POLL: MRDY (and ss) set to low to indicate we are sending poll\n");
	transferPoll(fd);
	printf("POLL: wait for SRDY to go high\n");
	int count = 0;
	while(!digitalRead(SRDY)){
		delay(1);
		count++;
		if(count % 10000 == 0){
			printf("POLL: waited to long for srdy sending new poll");
			transferPoll(fd);
		}
	}
	transfer(fd);
        digitalWrite (MRDY, HIGH) ;
	printf("POLL: SRDY is high Ready to start oporation\n");
	delay(10);
}
static void transferSreq(uint8_t tx[], uint8_t rx[]){
	delay(100);
        digitalWrite (MRDY, LOW);
	printf("SREQ: wait for SRDY to go low\n");
	int count = 0;
	while(digitalRead(SRDY)){
		delay(1);
		count++;
		if(count % 10000 == 0){
			printf("SREQ1: waited to long for srdy sending new poll");
			transferPoll(fd);
		}
	}
	printf("SREQ: SRDY is low\n");
	transferCmd(fd, tx);
	printf("transfered conf wait for SRDY to go HIGH\n");

	count = 0;
	while(!digitalRead(SRDY)){
		delay(1);
		count++;
		if(count % 10000 == 0){
			printf("SREQ2: waited to long for srdy sending new poll");
			transferPoll(fd);
		}
	}

	transferCmd(fd, rx);
	printf("done receiving set MRDY high\n");
        digitalWrite (MRDY, HIGH) ;
	while(!digitalRead(SRDY)){
		delay(1);
		count++;
		if(count % 10000 == 0){
			printf("SREQ3: waited to long for srdy sending new poll");
			transferPoll(fd);
		}
	}
	printf("SRDY is high\n");

}

static void transferCmd(int fd, uint8_t tx[])
{
	int ret;
	uint8_t rx[ARRAY_SIZE(tx)] = {0, };
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = ARRAY_SIZE(tx),
		.delay_usecs = delaySpi,
		.speed_hz = speed,
		.bits_per_word = bits,
	};

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		pabort("can't send spi message");

	for (ret = 0; ret < ARRAY_SIZE(tx); ret++) {
		if (!(ret % 6))
			puts("");
		printf("%.2X ", rx[ret]);
	}
	puts("");
}

/*
 * send a poll command
 */
static void transferPoll(int fd)
{
	int ret;
	uint8_t tx[] = CMD_POLL;
	uint8_t rx[ARRAY_SIZE(tx)] = {0, };
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = ARRAY_SIZE(tx),
		.delay_usecs = delaySpi,
		.speed_hz = speed,
		.bits_per_word = bits,
	};

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		pabort("can't send spi message");

	for (ret = 0; ret < ARRAY_SIZE(tx); ret++) {
		if (!(ret % 6))
			puts("");
		printf("%.2X ", rx[ret]);
	}
	puts("");
}

static void transfer(int fd)
{
	int ret;
	uint8_t tx[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00 ,0x00, 0x00, 0x00
	};
	uint8_t rx[ARRAY_SIZE(tx)] = {0, };
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = ARRAY_SIZE(tx),
		.delay_usecs = delaySpi,
		.speed_hz = speed,
		.bits_per_word = bits,
	};

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		pabort("can't send spi message");

	for (ret = 0; ret < ARRAY_SIZE(tx); ret++) {
		if (!(ret % 6))
			puts("");
		printf("%.2X ", rx[ret]);
	}
	puts("");
}

static void print_usage(const char *prog)
{
	printf("Usage: %s [-DsbdlHOLC3]\n", prog);
	puts("  -D --device   device to use (default /dev/spidev1.1)\n"
	     "  -s --speed    max speed (Hz)\n"
	     "  -d --delaySpi    delay (usec)\n"
	     "  -b --bpw      bits per word \n"
	     "  -l --loop     loopback\n"
	     "  -H --cpha     clock phase\n"
	     "  -O --cpol     clock polarity\n"
	     "  -L --lsb      least significant bit first\n"
	     "  -C --cs-high  chip select active high\n"
	     "  -3 --3wire    SI/SO signals shared\n");
	exit(1);
}

static void parse_opts(int argc, char *argv[])
{
	while (1) {
		static const struct option lopts[] = {
			{ "device",  1, 0, 'D' },
			{ "speed",   1, 0, 's' },
			{ "delaySpi",   1, 0, 'd' },
			{ "bpw",     1, 0, 'b' },
			{ "loop",    0, 0, 'l' },
			{ "cpha",    0, 0, 'H' },
			{ "cpol",    0, 0, 'O' },
			{ "lsb",     0, 0, 'L' },
			{ "cs-high", 0, 0, 'C' },
			{ "3wire",   0, 0, '3' },
			{ "no-cs",   0, 0, 'N' },
			{ "ready",   0, 0, 'R' },
			{ NULL, 0, 0, 0 },
		};
		int c;

		c = getopt_long(argc, argv, "D:s:d:b:lHOLC3NR", lopts, NULL);

		if (c == -1)
			break;

		switch (c) {
		case 'D':
			device = optarg;
			break;
		case 's':
			speed = atoi(optarg);
			break;
		case 'd':
			delaySpi = atoi(optarg);
			break;
		case 'b':
			bits = atoi(optarg);
			break;
		case 'l':
			mode |= SPI_LOOP;
			break;
		case 'H':
			mode |= SPI_CPHA;
			break;
		case 'O':
			mode |= SPI_CPOL;
			break;
		case 'L':
			mode |= SPI_LSB_FIRST;
			break;
		case 'C':
			mode |= SPI_CS_HIGH;
			break;
		case '3':
			mode |= SPI_3WIRE;
			break;
		case 'N':
			mode |= SPI_NO_CS;
			break;
		case 'R':
			mode |= SPI_READY;
			break;
		default:
			print_usage(argv[0]);
			break;
		}
	}
}

int main(int argc, char *argv[])
{
	int ret = 0;
        /*
         * initialice control pins
         */
        wiringPiSetup () ;
        pinMode (CFG0, OUTPUT) ; //cfg0 High to indicate slep crystal
        pinMode (CFG1, OUTPUT) ; //cfg1 High to indicate SPI main pin conf
        pinMode (RESET, OUTPUT) ; //reset
        pinMode (SRDY, INPUT)  ; //SRDY
        pinMode (MRDY, OUTPUT) ; //MRDY

        digitalWrite (CFG0, HIGH) ;
        digitalWrite (CFG1, HIGH) ;
        digitalWrite (MRDY, HIGH) ;
        digitalWrite (RESET, LOW) ; delay (500) ;

	parse_opts(argc, argv);

	fd = open(device, O_RDWR);
	if (fd < 0)
		pabort("can't open device");

	/*
	 * spi mode
	 */
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		pabort("can't set spi mode");

	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1)
		pabort("can't get spi mode");

	/*
	 * bits per word
	 */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't get bits per word");

	/*
	 * max speed hz
	 */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't get max speed hz");

	printf("spi mode: %d\n", mode);
	printf("bits per word: %d\n", bits);
	printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);

	//wait for CC2530 to set SRDY low which indicate it is ready to
	//send the SYS_RESET_IND message
        digitalWrite (RESET, HIGH) ; delay(100); 
	printf("CC2530 Reset released delay 2 sec\n");

	//wait 2 sec. Delays are needed to avoid errors.
	delay(2000);
  	while(digitalRead(SRDY)){
        }

        // send zb write configuration
        while (1){
		// as long as the CC2530 has SRDY low we will poll for messages
	    	//first time this loop is entered the SYS_RESET_IND message is reseived.
    	 	while(!digitalRead(SRDY)){
			printf("----------------polling\n");
			poll();
			printf("----------------DONE polling\n\n");
        	}


		//if we are here CC2530 has its SRDY set to high indicating it is ready to
		//receive commands. The following code sends a Syncronious request message and
		//read the reply. The message is defined in the define statments in the top of
		//this file.
		transferSreq(ZB_READ_CONFIGURATION, get_conf_responce);
		printf("------------------Done sending SREQ\n");
	}
	close(fd);
	
return ret;
}
