// DS1302 RTC IC
// http://datasheets.maximintegrated.com/en/ds/DS1302.pdf
//

#include "ds1302.h"

#define MAGIC_HI  0x5A
#define MAGIC_LO  0xA5

void ds_ram_config_init(uint8_t * config) {
	uint8_t i;
	// check magic bytes to see if ram has been written before
	if ( (ds_readbyte( DS_CMD_RAM >> 1 | 0x00) != MAGIC_LO || ds_readbyte( DS_CMD_RAM >> 1 | 0x01) != MAGIC_HI) ) {
		// if not, must init ram config to defaults
		ds_writebyte( DS_CMD_RAM >> 1 | 0x00, MAGIC_LO);
		ds_writebyte( DS_CMD_RAM >> 1 | 0x01, MAGIC_HI);

		for (i=0; i<sizeof(struct ram_config); i++)
			ds_writebyte( DS_CMD_RAM >> 1 | (i+2), 0x00);
	}

	// read ram config
	for (i=0; i<sizeof(struct ram_config); i++)
		config[i] = ds_readbyte(DS_CMD_RAM >> 1 | (i+2));
}

void ds_ram_config_write(uint8_t const * config) {
	uint8_t i;
	for (i=0; i<sizeof(struct ram_config); i++)
		ds_writebyte( DS_CMD_RAM >> 1 | (i+2), config[i]);
}

static void ds_sendByte(uint8_t b) {
	uint8_t i;

	for (i=0; i < 8; i++) {
		_nop_; _nop_;
		DS_IO = b & 0x01;
		DS_SCLK = 1;
		_nop_; _nop_;
		DS_SCLK = 0;

		b >>= 1;
	}
}

static void ds_sendBegin(uint8_t cmd) {
	DS_CE = 0;
	DS_SCLK = 0;
	DS_CE = 1;
	ds_sendByte(cmd);
}

#define ds_sendEnd() { DS_CE = 0; }

static uint8_t ds_recvByte(void) {
	uint8_t i, b = 0, p = 1;
	for (i=0; i < 8; i++) {
		_nop_; _nop_;
		if(DS_IO) b |= p;
		DS_SCLK = 1;
		_nop_; _nop_;
		DS_SCLK = 0;

		p <<= 1;
	}
	return b;
}

uint8_t ds_readbyte(uint8_t addr) {
	uint8_t b;
	ds_sendBegin(DS_CMD | DS_CMD_CLOCK | addr << 1 | DS_CMD_READ);
	b = ds_recvByte();
	ds_sendEnd();
	return b;
}

void ds_readburst(uint8_t time[8]) {
	uint8_t i;
	ds_sendBegin(DS_CMD | DS_CMD_CLOCK | DS_BURST_MODE << 1 | DS_CMD_READ);
	for (i=0; i < 8; i++) {
		time[i] = ds_recvByte();;
	}
	ds_sendEnd();
}

void ds_writebyte(uint8_t addr, uint8_t data) {
	// ds1302 single-byte write
	ds_sendBegin(DS_CMD | DS_CMD_CLOCK | addr << 1 | DS_CMD_WRITE);
	ds_sendByte(data);
	ds_sendEnd();
}

void ds_writeburst(uint8_t const time[8]) {
	// ds1302 burst-write 8 bytes from struct
	uint8_t i;
	ds_sendBegin(DS_CMD | DS_CMD_CLOCK | DS_BURST_MODE << 1 | DS_CMD_WRITE);
	for (i=0; i < 8; i++) {
		ds_sendByte(time[i]);
	}
	ds_sendEnd();
}

void ds_init() {
	uint8_t b = ds_readbyte(DS_ADDR_SECONDS);
	ds_writebyte(DS_ADDR_WP, 0); // clear WP
	b &= ~(0b10000000);
	ds_writebyte(DS_ADDR_SECONDS, b); // clear CH
}

#if CFG_SET_DATE_TIME == 1

#define ds_split2int(tens, ones) ((tens) * 10 + (ones))

// reset date, time
void ds_reset_clock() {
	ds_writebyte(DS_ADDR_MINUTES, 0x00);
	ds_writebyte(DS_ADDR_HOUR, 0x87);
	ds_writebyte(DS_ADDR_MONTH, 0x01);
	ds_writebyte(DS_ADDR_DAY, 0x01);
}
    
// increment hours
void ds_hours_incr(uint8_t hours) {
	if (hours < 23)
		hours++;
	else
		hours = 0;
	ds_writebyte(DS_ADDR_HOUR, ds_int2bcd(hours));
}

// increment minutes
void ds_minutes_incr(uint8_t minutes) {
	if (minutes < 59)
		minutes++;
	else
		minutes = 1;
	ds_writebyte(DS_ADDR_MINUTES, ds_int2bcd(minutes));
}

void ds_seconds_reset() {
	ds_writebyte(DS_ADDR_SECONDS, 0);
}

// increment month
void ds_month_incr(struct ds1302_rtc* rtc) {
	uint8_t month = ds_split2int(rtc->tenmonth, rtc->month);
	if (month < 12)
		month++;
	else
		month = 1;
	ds_writebyte(DS_ADDR_MONTH, ds_int2bcd(month));
}

// increment day
void ds_day_incr(struct ds1302_rtc* rtc) {
	uint8_t day = ds_split2int(rtc->tenday, rtc->day);
	if (day < 31)
		day++;
	else
		day = 1;
	ds_writebyte(DS_ADDR_DAY, ds_int2bcd(day));
}

void ds_weekday_incr(struct ds1302_rtc* rtc) {
	if (rtc->weekday < 7)
		rtc->weekday++;
	else
		rtc->weekday = 1;
	ds_writebyte(DS_ADDR_WEEKDAY, rtc->weekday);
}

// return bcd byte from integer
uint8_t ds_int2bcd(uint8_t integer) {
	return integer / 10 << 4 | integer % 10;
}

#endif // CFG_SET_DATE_TIME == 1

uint8_t ds_int2bcd_tens(uint8_t integer) {
	return integer / 10 % 10;
}

uint8_t ds_int2bcd_ones(uint8_t integer) {
	return integer % 10;
}

