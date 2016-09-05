#include <cstring>

void spr_decompress(char *SrcBuf, int SrcSize, char *DestBuf, int DestSize) {
	char *TmpBuf = new char [0x1000];
	memset(TmpBuf, 0, 0x1000);
	int TmpPosIn = 0x0FEE, TmpPosOut;
	unsigned char flag;
	int flag_count = 0;
	int SrcPos = 0;
	int DestPos = 0;
	int LoopCount;

	while (1) {
		if (SrcPos >= SrcSize)
			break;
		if (DestPos >= DestSize)
			break;
		if (flag_count == 0) {
			flag = SrcBuf[SrcPos++];
			flag_count = 8;
		}
		if (flag & 0x01)
		{
			// 1 ??????
			flag >>= 1;
			flag_count--;
			DestBuf[DestPos] = SrcBuf[SrcPos++];
			TmpBuf[TmpPosIn] = DestBuf[DestPos++];
			TmpPosIn = (TmpPosIn + 1) % 0x1000;
		}
		else {
			// 0 ?????
			flag >>= 1;
			flag_count--;
			LoopCount = (SrcBuf[SrcPos+1] & 0x0F) + 3;
			TmpPosOut = SrcBuf[SrcPos+1];
			TmpPosOut = ((TmpPosOut << 4) & 0x00000F00) | (SrcBuf[SrcPos] & 0xFF);
			SrcPos += 2;
			while (LoopCount > 0) {
				DestBuf[DestPos] = TmpBuf[TmpPosOut];
				TmpBuf[TmpPosIn] = DestBuf[DestPos++];
				TmpPosIn = (TmpPosIn + 1) % 0x1000;
				TmpPosOut = (TmpPosOut + 1) % 0x1000;
				LoopCount--;
				if (DestPos >= DestSize)
					break;
			}
		}
	}

	delete TmpBuf;
}
