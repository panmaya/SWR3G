/*
 * secbox_cmd.h
 *
 * Created: 2016-11-14 10:08:19 AM
 *  Author: Panmaya
 */ 


#ifndef SECBOX_CMD_H_
#define SECBOX_CMD_H_


void secbox_cmd_Card(ascii * cmdstr, const uint16_t len);
void secbox_cmd_Comm(ascii * cmdstr, const uint16_t len);
void secbox_cmd_Temp(ascii * cmdstr, const uint16_t len);
void secbox_cmd_Rfid(ascii * cmdstr, const uint16_t len);
void secbox_cmd_Sec(ascii * cmdstr, const uint16_t len);


#endif /* SECBOX_CMD_H_ */