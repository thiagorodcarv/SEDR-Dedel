#include "core.h"

/*
 * Core that apply a 3x3 (Configurable) 2d Convolution, Erode, Dilate on grayscale images
 *
*/

void doImgProc(hls::stream<uint_8_side_channel> &inStream, hls::stream<int_8_side_channel> &outStream, char kernel[KERNEL_DIM*KERNEL_DIM],int operation)
{


#pragma HLS INTERFACE axis port=inStream
#pragma HLS INTERFACE axis port=outStream
#pragma HLS INTERFACE s_axilite port=return bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=operation bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=kernel bundle=KERNEL_BUS


    hls::LineBuffer<KERNEL_DIM,IMG_WIDTH_OR_COLS,unsigned char> lineBuff;
    hls::Window<KERNEL_DIM,KERNEL_DIM,short> window;
    int idxRow=0;int idxCol = 0;
    int pixProcessed=0;
    int_8_side_channel dataOutSideChannel;
    uint_8_side_channel currPixelSideChannel;
    unsigned char pixelIn;

    for(int idxPixel = 0; idxPixel < (IMG_WIDTH_OR_COLS*IMG_HEIGHT_OR_ROWS); idxPixel++){
#pragma HLS PIPELINE
        currPixelSideChannel = inStream.read();
        pixelIn = currPixelSideChannel.data;
        lineBuff.shift_up(idxCol);
        lineBuff.insert_top(pixelIn,idxCol);
        for(int idxWinRow = 0; idxWinRow<KERNEL_DIM; idxWinRow++){
            for(int idxWinCol = 0; idxWinCol<KERNEL_DIM; idxWinCol++){
                short val = (short) lineBuff.getval(idxWinRow,idxWinCol+pixProcessed);
                val = (short) kernel[(idxWinRow*KERNEL_DIM)+idxWinCol]*val;
                window.insert(val,idxWinRow,idxWinCol);
            }
        }
        short valOutput =0;
        if((idxRow>=KERNEL_DIM-1)&&(idxCol>=KERNEL_DIM-1)){
            switch(operation){
            case 0:
            {
                valOutput = sumWindow(&window);
                valOutput = valOutput/8;
                if(valOutput<0)
                    valOutput=0;
                break;
            }
            case 1:
            {
                valOutput=minWindow(&window);
                break;
            }
            case 2:
            {
                valOutput = maxWindow(&window);
                break;
            }
            }
            pixProcessed++;
        }
        if(idxCol < IMG_WIDTH_OR_COLS-1){
            idxCol++;
        }
        else{
            idxCol=0;
            idxRow++;
            pixProcessed=0;
        }
        dataOutSideChannel.data=valOutput;
        dataOutSideChannel.keep=currPixelSideChannel.keep;
        dataOutSideChannel.strb=currPixelSideChannel.strb;
        dataOutSideChannel.user=currPixelSideChannel.user;
        dataOutSideChannel.last=currPixelSideChannel.last;
        dataOutSideChannel.dest=currPixelSideChannel.dest;

        outStream.write(dataOutSideChannel);
    }
}

short minWindow(hls::Window<KERNEL_DIM,KERNEL_DIM,short> *window) {
    unsigned char minVal=255;
    // loop for the small values on the array
    for(int idxRow=0;idxRow<KERNEL_DIM;idxRow++) {
        for(int idxCol=0;idxCol<KERNEL_DIM;idxCol++) {
            unsigned char valInWindow;
            valInWindow = (unsigned char ) window->getval(idxRow,idxCol);
            if(valInWindow<minVal)
                minVal=valInWindow;
        }
    }
    return minVal;
}

short maxWindow(hls::Window<KERNEL_DIM,KERNEL_DIM,short> *window) {
    unsigned char maxVal=255;
    // loop for the small values on the array
    for(int idxRow=0;idxRow<KERNEL_DIM;idxRow++) {
        for(int idxCol=0;idxCol<KERNEL_DIM;idxCol++) {
            unsigned char valInWindow;
            valInWindow = (unsigned char) window->getval(idxRow,idxCol);
            if(valInWindow>maxVal)
                maxVal=valInWindow;
        }
    }
    return maxVal;
}

short sumWindow(hls::Window<KERNEL_DIM,KERNEL_DIM,short> *window) {
    short accumulator=0;
    for(int idxRow=0;idxRow<KERNEL_DIM;idxRow++) {
        for(int idxCol=0;idxCol<KERNEL_DIM;idxCol++) {
            accumulator=accumulator+(short) window->getval(idxRow,idxCol);
        }
    }
    return accumulator;
}
