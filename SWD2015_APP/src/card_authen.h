/*
 * card_authen.h
 *
 * Created: 2/2/2559 10:14:49
 *  Author: Pan
 */ 


#ifndef CARD_AUTHEN_H_
#define CARD_AUTHEN_H_


ascii * card_authenGetTrack3(int8_t index);
int8_t card_authenGetTrack3Tabid(void) ;
u8 card_authenLicenseID(ascii *str);
ascii * card_authenInit(void);
void card_authenClean(void);


#endif /* CARD_AUTHEN_H_ */