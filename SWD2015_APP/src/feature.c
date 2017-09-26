/*
 * feature_tbl.c
 *
 * Created: 2/11/2558 14:41:54
 *  Author: Pan
 */ 

#include "adl_global.h"
#include "feature.h"

typedef struct FEATURE_TAB_ATTR {
	ascii sFwName[4];
	uint64_t fw_option;
}feature_tab_t;

static uint64_t FEATURE_GlobalMask = 0;

static const feature_tab_t feature_tab[] = {
	/* 50X on Legacy */ {"000", FEATURE_STANDARD_50X},  
	/* 52X on Legacy */ {"001", FEATURE_MOVELESS_MACHINE_NO_IDLE_52X},
	/* 58X on Legacy */ {"002", FEATURE_PTO3_A2D_READ_DOOR_SIGNAL_58X},
	/* 24X on Legacy */ {"003", FEATURE_NEVER_USE_IG_FOR_CAL_DISTANCE_24X},
	/* 27X on Legacy */ {"004", FEATURE_USING_4_HEAT_SENSORS_27X},
	/*SCG*/			{"005", FEATURE_GEOFENCE_SOUND_4SCG},
	/* Bike */			{"006", FEAT_BIKE_TRACKER},
	
	{ "" , 0}
};


bool swd_feature_isEnable(uint64_t feature) {
	return (bool) (FEATURE_GlobalMask & feature);
}

void swd_feature_Init(ascii * fwname) {
	uint16_t i = 0;
	for(;feature_tab[i].sFwName[0];i++) {
		if(!strcmp(feature_tab[i].sFwName,fwname)) {
			FEATURE_GlobalMask = feature_tab[i].fw_option;
			break;
		}
	}
}