/*
 * geofence.c
 *
 *  Created on: Jun 28, 2016
 *      Author: Panmaya
 */

#include "adl_global.h"
#include "geofence.h"

static void get_data_field(geofence_t *ptab, ascii *pToken, u8 field_id) {
	//ascii buffer[100] = {"---"};

	switch(field_id) {
	case 1:
		ptab->geo_id = wm_hexatoi ( pToken, strlen(pToken) );
		//wm_sprintf(buffer,"geo_id:%s = %02X\r\n",pToken,ptab->geo_id);
		break;
	case 2:
		ptab->idx_point	= wm_hexatoi ( pToken, 2 );
		//wm_sprintf(buffer,"idx_point:%s = %02X\r\n",pToken,ptab->idx_point);
		break;
	case 3:
		ptab->lat = atof(pToken);
		//wm_sprintf(buffer,"lat:%s = %f\r\n",pToken,ptab->lat);
		break;
	case 4:
		ptab->lon = atof(pToken);
		//wm_sprintf(buffer,"lon:%s = %f\r\n",pToken,ptab->lon);
		break;
	case 5:
		ptab->radius_meter = wm_hexatoi ( pToken, 2 );
		//wm_sprintf(buffer,"radius_meter:%s = %02X\r\n",pToken,ptab->radius_meter);
		break;
	case 6:
		ptab->geo_type = *pToken;
		//wm_sprintf(buffer,"geo_type:%s = %c\r\n",pToken,ptab->geo_type);
		break;
	case 7:
		ptab->alarm_type = wm_hexatoi ( pToken, 2 );
		//wm_sprintf(buffer,"alarm_type:%s = %02X\r\n",pToken,ptab->alarm_type);
		break;
	case 8:
		ptab->data_cond	= wm_hexatoi ( pToken, 2 );
		//wm_sprintf(buffer,"data_cond:%s = %02X\r\n",pToken,ptab->data_cond);
		break;
	case 9:
		ptab->data_value = wm_hexatoi ( pToken, 2 );
		//wm_sprintf(buffer,"data_valuee:%s = %02X\r\n",pToken,ptab->data_value);
		break;
	case 10:
		ptab->data_cond_ex = wm_hexatoi ( pToken, 2 );
		//wm_sprintf(buffer,"data_cond_ex:%s = %02X\r\n",pToken,ptab->data_cond_ex);
		break;
	case 11:
		ptab->data_value_ex	= wm_hexatoi ( pToken, 2 );
		//wm_sprintf(buffer,"data_value_ex:%s = %02X\r\n",pToken,ptab->data_value_ex);
		break;
	case 12:
		ptab->action_sound = wm_hexatoi ( pToken, 2 );
		//wm_sprintf(buffer,"action_sound:%s = %02X\r\n",pToken,ptab->action_sound);
		break;
	case 13:
		ptab->sound_cond = wm_hexatoi ( pToken, 2 );
		//wm_sprintf(buffer,"sound_cond:%s = %02X\r\n",pToken,ptab->sound_cond);
		break;
	case 14:
		ptab->speed_cond = wm_hexatoi ( pToken, 2 );
		//wm_sprintf(buffer,"speed_cond:%s = %02X\r\n",pToken,ptab->speed_cond);
		break;
	case 15:
		ptab->speed_value_before_warning = wm_hexatoi ( pToken, 2 );
		//wm_sprintf(buffer,"speed_value_before_warning:%s = %02X\r\n",pToken,ptab->speed_value_before_warning);
		break;
	case 16:
		ptab->speed_value_exceed_warninig = wm_hexatoi ( pToken, 2 );
		//wm_sprintf(buffer,"speed_value_exceed_warninig:%s = %02X\r\n",pToken,ptab->speed_value_exceed_warninig);
		break;
	case 17:
		ptab->ovs_alert_send_time = wm_hexatoi ( pToken, 2 );
		//wm_sprintf(buffer,"ovs_alert_send_time:%s = %02X\r\n",pToken,ptab->ovs_alert_send_time);
		break;
	default:
		break;
	}

	//adl_atSendResponse ( ADL_AT_RSP, buffer );
}

s8 geofence_decode_raw_config( const char *buffer, u16 size , geofence_t dst[GEOFENCE_MAX_TABLE_ID], u8 *version) {
	u8 tabid = 0;
	u8 field_id = 0;
	char strbuff[25];
	uint16_t idxbuf = 0;
	uint16_t memidx = 0;

	if ((buffer[2] ==0x0D) && (buffer[3] == 0x0A)) {
		memcpy(strbuff,buffer,2);
		strbuff[2] = 0;
		if(wm_ishexastring(strbuff)) {
			*version = wm_hexatoi ( strbuff, 2 );
			idxbuf = 4;
			memidx = 4;
			size -= 4;
		}else return ERROR -1;
	} else return ERROR -2;

	for(; size ; size-- ) {
		if((*(buffer+idxbuf) == ',')||(*(buffer+idxbuf) == 0x0D)) {//Found Token Character
			memcpy(strbuff,buffer+memidx,idxbuf-memidx);
			strbuff[idxbuf-memidx] = 0;
			field_id = field_id >= 17 ? 1 : field_id + 1;
			get_data_field(&dst[tabid], strbuff, field_id);
			memidx = idxbuf+1;//Point to next
		}
		else if(*(buffer+idxbuf) == 0x0A) {//End of Field
			if(size<=1)break;//Exit
			memidx = idxbuf+1;//Point to next
			if(tabid >= (GEOFENCE_MAX_TABLE_ID-1)) break;//Exit
			else tabid++;
		}
		idxbuf++;
	}

	return tabid;
}








