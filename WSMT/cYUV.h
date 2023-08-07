#ifndef __CYUV_H__
#define __CYUV_H__

#include "TypeDef.h"

template <class PixelType>
class cYUV{
public:
	Char m_sYUVFilename[MAX_TEXT_LENGTH];

	UInt m_uiWidth;
	UInt m_uiHeight;
	UInt m_uiWidthUV;
	UInt m_uiHeightUV;

	UInt m_uiOriginalWidth;
	UInt m_uiOriginalHeight;

	UInt m_uiImSize;
	UInt m_uiImSizeUV;

	UInt m_uiBitsPerSample;
	UInt m_uiBytesPerSample;
	UInt m_uiChromaSubsampling;

	Int m_iFilterType;
	Double m_uiHorizontalPrecision;
	Double m_uiVerticalPrecision;

	PixelType *m_atY;
	PixelType *m_atU;
	PixelType *m_atV;

	~cYUV();
	cYUV();
	cYUV(Int w, Int h, Int bps = 8, Int cs = 420);
	Void setOriginalSize();
	Void init(Int w, Int h, Int bps = 8, Int cs = 420);
	Void frameReader(String sFilename, Int iFrame);
	Void frameWriter(String sFilename, Bool bAppend);

	Void imResize(Int filterType, Double hPrec, Double vPrec);
	Void imResizeHorizontalY();
	Void imResizeVerticalY();
	Void imResizeHorizontalUV();
	Void imResizeVerticalUV();
};

template <class PixelType>
cYUV<PixelType>::~cYUV(){
	if (m_atY) { delete m_atY; m_atY = NULL; }
	if (m_atU) { delete m_atU; m_atU = NULL; }
	if (m_atV) { delete m_atV; m_atV = NULL; }
}

template <class PixelType>
cYUV<PixelType>::cYUV(){
	m_atY = NULL;
	m_atU = NULL;
	m_atV = NULL;
	return;
}

template <class PixelType>
cYUV<PixelType>::cYUV(Int w, Int h, Int bps, Int cs){
	m_atY = NULL;
	m_atU = NULL;
	m_atV = NULL;
	init(w, h, bps, cs);
	return;
}

template <class PixelType>
Void cYUV<PixelType>::setOriginalSize(){

	if (m_atY) { delete m_atY; m_atY = NULL; }
	if (m_atU) { delete m_atU; m_atU = NULL; }
	if (m_atV) { delete m_atV; m_atV = NULL; }

	m_uiWidth = m_uiOriginalWidth;
	m_uiHeight = m_uiOriginalHeight;
	m_uiImSize = m_uiWidth*m_uiHeight;

	m_atY = new PixelType[m_uiImSize];

	if (m_uiChromaSubsampling == 400){
		m_uiWidthUV = 0;
		m_uiHeightUV = 0;
		m_uiImSizeUV = 0;
		m_atU = NULL;
		m_atV = NULL;
	}
	if (m_uiChromaSubsampling == 420){
		m_uiWidthUV = m_uiWidth >> 1;
		m_uiHeightUV = m_uiHeight >> 1;
		m_uiImSizeUV = m_uiWidthUV*m_uiHeightUV;
		m_atU = new PixelType[m_uiImSizeUV];
		m_atV = new PixelType[m_uiImSizeUV];
	}
	if (m_uiChromaSubsampling == 444){
		m_uiWidthUV = m_uiWidth;
		m_uiHeightUV = m_uiHeight;
		m_uiImSizeUV = m_uiWidthUV*m_uiHeightUV;
		m_atU = new PixelType[m_uiImSizeUV];
		m_atV = new PixelType[m_uiImSizeUV];
	}

}

template <class PixelType>
Void cYUV<PixelType>::init(Int w, Int h, Int bps, Int cs){

	m_uiOriginalHeight = h;
	m_uiOriginalWidth = w;

	m_uiBitsPerSample = bps;
	m_uiBytesPerSample = m_uiBitsPerSample < 9 ? 1 : 2;
	m_uiChromaSubsampling = cs;

	m_uiWidth = m_uiOriginalWidth;
	m_uiHeight = m_uiOriginalHeight;
	m_uiImSize = m_uiWidth*m_uiHeight* m_uiBytesPerSample;

	setOriginalSize();
	
	return;
}


template <class PixelType>
Void cYUV<PixelType>::frameReader(String sFilename, Int frame){

	setOriginalSize(); //Owieczka - What it is needed??

	FILE* fileYUV = NULL;
	fopen_s(&fileYUV, sFilename.c_str(), "rb");
	
	Int iOffset;
	if (m_uiChromaSubsampling == 444) iOffset = 3 * m_uiImSize * m_uiBytesPerSample;
	if (m_uiChromaSubsampling == 420) iOffset = Int(m_uiImSize + (m_uiImSize >> 1)) * m_uiBytesPerSample;
	if (m_uiChromaSubsampling == 400) iOffset = m_uiImSize * m_uiBytesPerSample;

	_fseeki64(fileYUV, (Int64)iOffset*(Int64)frame, 0);

	fread(m_atY, m_uiBytesPerSample, m_uiImSize, fileYUV);

	if (m_uiChromaSubsampling == 420 || m_uiChromaSubsampling == 444){
		fread(m_atU, m_uiBytesPerSample, m_uiImSizeUV, fileYUV);
		fread(m_atV, m_uiBytesPerSample, m_uiImSizeUV, fileYUV);
	}

	fclose(fileYUV);

	return;
}

template <class PixelType>
Void cYUV<PixelType>::frameWriter(String sFilename, Bool bAppend){

	FILE* fileYUV;

	fopen_s(&fileYUV, sFilename.c_str(), (bAppend) ? "ab" : "wb");

	fwrite((Void*)m_atY, m_uiBytesPerSample, m_uiImSize, fileYUV);

	if (m_atU && m_atV){
		fwrite(m_atU, m_uiBytesPerSample, m_uiImSizeUV, fileYUV);
		fwrite(m_atV, m_uiBytesPerSample, m_uiImSizeUV, fileYUV);
	}

	fclose(fileYUV);

	return;
}

template <class PixelType>
Void cYUV<PixelType>::imResizeHorizontalY(){

	Int iLeftPix, iRightPix;
	Int* piPix = new Int[(Int)m_uiHorizontalPrecision];
	UInt uiNewWidth = UInt(m_uiWidth*m_uiHorizontalPrecision);
	UInt uiWidthMinus1 = m_uiWidth - 1;

	cYUV<PixelType> *temp = new cYUV<PixelType>(uiNewWidth, m_uiHeight);

	switch ((Int)m_uiHorizontalPrecision){
	case 2:
		for (UInt i = 0; i < this->m_uiWidth; i++){
			iLeftPix = CLIP(i, 0, uiWidthMinus1);
			iRightPix = CLIP(i + 1, 0, uiWidthMinus1);
			*piPix = i << 1;
			*(piPix + 1) = *piPix + 1;

			for (UInt j = 0; j < this->m_uiHeight; j++){
				temp->m_atY[j*uiNewWidth + *piPix] = this->m_atY[j*m_uiWidth + iLeftPix];
				temp->m_atY[j*uiNewWidth + *(piPix + 1)] = (this->m_atY[j*m_uiWidth + iLeftPix] + this->m_atY[j*m_uiWidth + iRightPix] + 1) >> 1;
			}
		}
		break;

	case 4:
		for (UInt i = 0; i < this->m_uiWidth; i++){
			iLeftPix = CLIP(i, 0, uiWidthMinus1);
			iRightPix = CLIP(i + 1, 0, uiWidthMinus1);
			*piPix = i << 2;
			*(piPix + 1) = *piPix + 1;
			*(piPix + 2) = *(piPix + 1) + 1;
			*(piPix + 3) = *(piPix + 2) + 1;

			for (UInt j = 0; j < this->m_uiHeight; j++){
				temp->m_atY[j*uiNewWidth + *piPix] = this->m_atY[j*m_uiWidth + iLeftPix];
				temp->m_atY[j*uiNewWidth + *(piPix + 1)] = (this->m_atY[j*m_uiWidth + iLeftPix] * 3 + this->m_atY[j*m_uiWidth + iRightPix] + 2) >> 2;
				temp->m_atY[j*uiNewWidth + *(piPix + 2)] = (this->m_atY[j*m_uiWidth + iLeftPix] + this->m_atY[j*m_uiWidth + iRightPix] + 1) >> 1;
				temp->m_atY[j*uiNewWidth + *(piPix + 3)] = (this->m_atY[j*m_uiWidth + iLeftPix] + this->m_atY[j*m_uiWidth + iRightPix] * 3 + 2) >> 2;
			}
		}
		break;
	}



	delete this->m_atY;
	this->m_atY = temp->m_atY;
	this->m_uiWidth = uiNewWidth;
	this->m_uiImSize = uiNewWidth*this->m_uiHeight*(m_uiBytesPerSample);
}

template <class PixelType>
Void cYUV<PixelType>::imResizeVerticalY(){

	Int iTopPix, iBottomPix;
	Int* piPix = new Int[(Int)m_uiVerticalPrecision];
	UInt uiNewHeight = UInt(this->m_uiHeight*m_uiVerticalPrecision);
	UInt uiHeightMinus1 = this->m_uiHeight - 1;

	cYUV<PixelType> *temp = new cYUV<PixelType>(this->m_uiWidth, uiNewHeight);

	switch ((Int)m_uiVerticalPrecision){
	case 2:
		for (UInt j = 0; j < this->m_uiHeight; j++){
			iTopPix = CLIP(j, 0, uiHeightMinus1);
			iBottomPix = CLIP(j + 1, 0, uiHeightMinus1);
			*piPix = j << 1;
			*(piPix + 1) = *piPix + 1;
			for (UInt i = 0; i < this->m_uiWidth; i++){
				temp->m_atY[*piPix * this->m_uiWidth + i] = this->m_atY[iTopPix * this->m_uiWidth + i];
				temp->m_atY[*(piPix + 1) * this->m_uiWidth + i] = (this->m_atY[iTopPix * this->m_uiWidth + i] + this->m_atY[iBottomPix * this->m_uiWidth + i] + 1) >> 1;
			}
		}
		break;

	case 4:
		for (UInt j = 0; j < this->m_uiHeight; j++){
			iTopPix = CLIP(j, 0, uiHeightMinus1);
			iBottomPix = CLIP(j + 1, 0, uiHeightMinus1);
			*piPix = j << 2;
			*(piPix + 1) = *piPix + 1;
			*(piPix + 2) = *(piPix + 1) + 1;
			*(piPix + 3) = *(piPix + 2) + 1;
			for (UInt i = 0; i < this->m_uiWidth; i++){
				temp->m_atY[*piPix * this->m_uiWidth + i] = this->m_atY[iTopPix * this->m_uiWidth + i];
				temp->m_atY[*(piPix + 1) * this->m_uiWidth + i] = (this->m_atY[iTopPix * this->m_uiWidth + i] * 3 + this->m_atY[iBottomPix * this->m_uiWidth + i] + 2) >> 2;
				temp->m_atY[*(piPix + 2) * this->m_uiWidth + i] = (this->m_atY[iTopPix * this->m_uiWidth + i] + this->m_atY[iBottomPix * this->m_uiWidth + i] + 1) >> 1;
				temp->m_atY[*(piPix + 3) * this->m_uiWidth + i] = (this->m_atY[iTopPix * this->m_uiWidth + i] + this->m_atY[iBottomPix * this->m_uiWidth + i] * 3 + 2) >> 2;
			}
		}
		break;
	}
	delete this->m_atY;
	this->m_atY = temp->m_atY;
	this->m_uiHeight = uiNewHeight;
	this->m_uiImSize = this->m_uiWidth*uiNewHeight*(m_uiBytesPerSample);
}

template <class PixelType>
Void cYUV<PixelType>::imResizeHorizontalUV(){

	Int iLeftPix, iRightPix;
	Int* piPix = new Int[(Int)m_uiHorizontalPrecision];
	UInt uiNewWidth = UInt(m_uiWidthUV*m_uiHorizontalPrecision);
	UInt uiWidthMinus1 = this->m_uiWidthUV - 1;

	cYUV<PixelType> *tempU = new cYUV<PixelType>(uiNewWidth, this->m_uiHeightUV);
	cYUV<PixelType> *tempV = new cYUV<PixelType>(uiNewWidth, this->m_uiHeightUV);

	switch ((Int)m_uiHorizontalPrecision){
	case 2:
		for (UInt i = 0; i < this->m_uiWidthUV; i++){
			iLeftPix = CLIP(i, 0, uiWidthMinus1);
			iRightPix = CLIP(i + 1, 0, uiWidthMinus1);
			*piPix = i << 1;
			*(piPix + 1) = *piPix + 1;
			for (UInt j = 0; j < this->m_uiHeightUV; j++){
				tempU->m_atY[j*uiNewWidth + *piPix] = this->m_atU[j*m_uiWidthUV + iLeftPix];
				tempU->m_atY[j*uiNewWidth + *(piPix + 1)] = (this->m_atU[j*m_uiWidthUV + iLeftPix] + this->m_atU[j*m_uiWidthUV + iRightPix] + 1) >> 1;

				tempV->m_atY[j*uiNewWidth + *piPix] = this->m_atV[j*m_uiWidthUV + iLeftPix];
				tempV->m_atY[j*uiNewWidth + *(piPix + 1)] = (this->m_atV[j*m_uiWidthUV + iLeftPix] + this->m_atV[j*m_uiWidthUV + iRightPix] + 1) >> 1;
			}
		}
		break;
	case 4:
		for (UInt i = 0; i < this->m_uiWidthUV; i++){
			iLeftPix = CLIP(i, 0, uiWidthMinus1);
			iRightPix = CLIP(i + 1, 0, uiWidthMinus1);
			*piPix = i << 2;
			*(piPix + 1) = *piPix + 1;
			*(piPix + 2) = *(piPix + 1) + 1;
			*(piPix + 3) = *(piPix + 2) + 1;
			for (UInt j = 0; j < this->m_uiHeightUV; j++){
				tempU->m_atY[j*uiNewWidth + *piPix] = this->m_atU[j*m_uiWidthUV + iLeftPix];
				tempU->m_atY[j*uiNewWidth + *(piPix + 1)] = (this->m_atU[j*m_uiWidthUV + iLeftPix] * 3 + this->m_atU[j*m_uiWidthUV + iRightPix] + 2) >> 2;
				tempU->m_atY[j*uiNewWidth + *(piPix + 2)] = (this->m_atU[j*m_uiWidthUV + iLeftPix] + this->m_atU[j*m_uiWidthUV + iRightPix] + 1) >> 1;
				tempU->m_atY[j*uiNewWidth + *(piPix + 3)] = (this->m_atU[j*m_uiWidthUV + iLeftPix] + this->m_atU[j*m_uiWidthUV + iRightPix] * 3 + 2) >> 2;

				tempV->m_atY[j*uiNewWidth + *piPix] = this->m_atV[j*m_uiWidthUV + iLeftPix];
				tempV->m_atY[j*uiNewWidth + *(piPix + 1)] = (this->m_atV[j*m_uiWidthUV + iLeftPix] * 3 + this->m_atV[j*m_uiWidthUV + iRightPix] + 2) >> 2;
				tempV->m_atY[j*uiNewWidth + *(piPix + 2)] = (this->m_atV[j*m_uiWidthUV + iLeftPix] + this->m_atV[j*m_uiWidthUV + iRightPix] + 1) >> 1;
				tempV->m_atY[j*uiNewWidth + *(piPix + 3)] = (this->m_atV[j*m_uiWidthUV + iLeftPix] + this->m_atV[j*m_uiWidthUV + iRightPix] * 3 + 2) >> 2;
			}
		}
		break;
	}
	delete this->m_atU;
	delete this->m_atV;
	this->m_atU = tempU->m_atY;
	this->m_atV = tempV->m_atY;
	this->m_uiWidthUV = uiNewWidth;
	this->m_uiImSizeUV = uiNewWidth*this->m_uiHeightUV*(m_uiBytesPerSample);
}

template <class PixelType>
Void cYUV<PixelType>::imResizeVerticalUV(){

	Int iTopPix, iBottomPix;
	Int* piPix = new Int[(Int)m_uiVerticalPrecision];
	UInt uiNewHeight = UInt(this->m_uiHeightUV*m_uiVerticalPrecision);
	UInt uiHeightMinus1 = this->m_uiHeightUV - 1;

	cYUV<PixelType> *tempU = new cYUV<PixelType>(this->m_uiWidthUV, uiNewHeight); //FIX resize Owieczka
	cYUV<PixelType> *tempV = new cYUV<PixelType>(this->m_uiWidthUV, uiNewHeight);

	switch ((Int)m_uiVerticalPrecision){
	case 2:
		for (UInt j = 0; j < this->m_uiHeightUV; j++){
			iTopPix = CLIP(j, 0, uiHeightMinus1);
			iBottomPix = CLIP(j + 1, 0, uiHeightMinus1);
			*piPix = j << 1;
			*(piPix + 1) = *piPix + 1;
			*(piPix + 2) = *(piPix + 1) + 1;
			*(piPix + 3) = *(piPix + 2) + 1;
			for (UInt i = 0; i < this->m_uiWidthUV; i++){
				tempU->m_atY[*piPix * this->m_uiWidthUV + i] = this->m_atU[iTopPix * this->m_uiWidthUV + i];
				tempU->m_atY[*(piPix + 1) * this->m_uiWidthUV + i] = (this->m_atU[iTopPix * this->m_uiWidthUV + i] + this->m_atU[iBottomPix * this->m_uiWidthUV + i] + 1) >> 1;
				tempV->m_atY[*piPix * this->m_uiWidthUV + i] = this->m_atV[iTopPix * this->m_uiWidthUV + i];
				tempV->m_atY[*(piPix + 1) * this->m_uiWidthUV + i] = (this->m_atV[iTopPix * this->m_uiWidthUV + i] + this->m_atV[iBottomPix * this->m_uiWidthUV + i] + 1) >> 1;
			}
		}
		break;
	case 4:
		for (UInt j = 0; j < this->m_uiHeightUV; j++){
			iTopPix = CLIP(j, 0, uiHeightMinus1);
			iBottomPix = CLIP(j + 1, 0, uiHeightMinus1);
			*piPix = j << 2;
			*(piPix + 1) = *piPix + 1;
			*(piPix + 2) = *(piPix + 1) + 1;
			*(piPix + 3) = *(piPix + 2) + 1;
			for (UInt i = 0; i < this->m_uiWidthUV; i++){
				tempU->m_atY[*piPix * this->m_uiWidthUV + i] = this->m_atU[iTopPix * this->m_uiWidthUV + i];
				tempU->m_atY[*(piPix + 1) * this->m_uiWidthUV + i] = (this->m_atU[iTopPix * this->m_uiWidthUV + i] * 3 + this->m_atU[iBottomPix * this->m_uiWidthUV + i] + 2) >> 2;
				tempU->m_atY[*(piPix + 2) * this->m_uiWidthUV + i] = (this->m_atU[iTopPix * this->m_uiWidthUV + i] + this->m_atU[iBottomPix * this->m_uiWidthUV + i] + 1) >> 1;
				tempU->m_atY[*(piPix + 3) * this->m_uiWidthUV + i] = (this->m_atU[iTopPix * this->m_uiWidthUV + i] + this->m_atU[iBottomPix * this->m_uiWidthUV + i] * 3 + 2) >> 2;
				tempV->m_atY[*piPix * this->m_uiWidthUV + i] = this->m_atV[iTopPix * this->m_uiWidthUV + i];
				tempV->m_atY[*(piPix + 1) * this->m_uiWidthUV + i] = (this->m_atV[iTopPix * this->m_uiWidthUV + i] * 3 + this->m_atV[iBottomPix * this->m_uiWidthUV + i] + 2) >> 2;
				tempV->m_atY[*(piPix + 2) * this->m_uiWidthUV + i] = (this->m_atV[iTopPix * this->m_uiWidthUV + i] + this->m_atV[iBottomPix * this->m_uiWidthUV + i] + 1) >> 1;
				tempV->m_atY[*(piPix + 3) * this->m_uiWidthUV + i] = (this->m_atV[iTopPix * this->m_uiWidthUV + i] + this->m_atV[iBottomPix * this->m_uiWidthUV + i] * 3 + 2) >> 2;
			}
		}
		break;
	}

	delete this->m_atU;
	delete this->m_atV;
	this->m_atU = tempU->m_atY;
	this->m_atV = tempV->m_atY;
	this->m_uiHeightUV = uiNewHeight;
	this->m_uiImSizeUV = this->m_uiWidthUV*uiNewHeight*(m_uiBytesPerSample);
}

template <class PixelType>
Void cYUV<PixelType>::imResize(Int filterType, Double hPrec, Double vPrec){

	m_iFilterType = filterType;
	m_uiHorizontalPrecision = hPrec;
	m_uiVerticalPrecision = vPrec;

	if (m_uiHorizontalPrecision != 1){
		imResizeHorizontalY();
		imResizeHorizontalUV();
	}
	if (m_uiVerticalPrecision != 1){
		imResizeVerticalY();
		imResizeVerticalUV();
	}

	return;
}

#endif