#include <3ds.h>
#include <sf2d.h>
#include <sfil.h>
#include <sftd.h>
#include "OpenSans_ttf.h"

int main(int argc, char **argv)
{
	//Initialize the sf2d lib
	sf2d_init();
	romfsInit();
	nfcInit(NFC_OpType_NFCTag);
	sftd_init();
	sftd_font *font = sftd_load_font_mem(OpenSans_ttf, OpenSans_ttf_size);

	//Start scanning for NFC tags
	nfcStartScanning(0x0);

	NFC_TagInfo nfcTag;
	NFC_TagState nfcState;
	bool scanned = false;
	u8 UID[4] = {0,0,0,0};

	while (aptMainLoop()){
		hidScanInput();
		u32 kDown = hidKeysDown();

		if (kDown & KEY_START)
			break;

		nfcGetTagState(&nfcState);
		if (nfcState == NFC_TagState_InRange && scanned == false){
			nfcGetTagInfo(&nfcTag);
			UID[0]=nfcTag.id[0];
			UID[1]=nfcTag.id[1];
			UID[2]=nfcTag.id[2];
			UID[3]=nfcTag.id[3];
			scanned = true;
		}
		if (nfcState == NFC_TagState_OutOfRange){
			scanned = false;
			nfcStartScanning(0x0);
		}

		sf2d_start_frame(GFX_TOP, GFX_LEFT);

		sf2d_end_frame();

		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
			sftd_draw_text(font, 10, 0, RGBA8(255,255,255,255), 15, "NFC Reader");
			sftd_draw_textf(font, 10, 16, RGBA8(255,255,255,255), 15, "Last Tag UID: %02x:%02x:%02x:%02x",UID[0],UID[1],UID[2],UID[3]);	
			sftd_draw_textf(font, 10, 32, RGBA8(255,255,255,255), 15, "canScan: %i", (int)!scanned);
		sf2d_end_frame();
		//Swap the buffers
		sf2d_swapbuffers();
	}
	nfcStopScanning();
	nfcExit();
	romfsExit();
	sftd_fini();
	sf2d_fini();
	return 0;
}
