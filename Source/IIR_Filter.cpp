/*
  ==============================================================================

    IIR_Filter.cpp
    Created: 19 Jul 2014 11:47:59am
    Author:  patrick

  ==============================================================================
*/

#include "IIR_Filter.h"

static double polynomialInterpolation(double x, Array<double> impulse_response){
    int n = impulse_response.size();
    double answer = 0.0;
    
    for (int i=0; i<= n; i++){
        double numerator = 1.0;
        for (int j=0; j<=impulse_response.size(); j++){
            if (j != i) numerator *= (x - j);
        }
    
        double denominator = 1.0;
        for (int j=0; j<=impulse_response.size(); j++){
            if (j != i) denominator *= (i - j);
        }
        answer += (numerator/denominator)*impulse_response[i];
    }
    return answer;
}

void IIR_Filter::setCoefficients(const IIRCoefficients& newCoefficients){
    IIRFilter::setCoefficients(newCoefficients);
    
    int impulse_size = 100;
    Array<double> impulse;

    zero_heading_size = 99;
    for (int i=0; i<zero_heading_size; i++){impulse.add(0.0);}
    
    impulse.add(1.0);
    for (int i=0; i<impulse_size; i++){
        impulse.add(0.0);
    }
    
    for (int i=0; i<impulse.size(); i++){
        impulseResponse.add(processSingleSampleRaw(impulse[i]));
    }
        
    float sample_resolution = 20.0;
       
    for (int i=0; i<impulseResponse.size(); i++){
        if (i < zero_heading_size){
            interpolatedImpulseResponse.add(impulseResponse[i]);
        }else{
            for (int j=0; j<sample_resolution; j++){
                float sample_count = (float)i + ( ((float)j) * (1.0/sample_resolution));
                interpolatedImpulseResponse.add(polynomialInterpolation(sample_count, impulseResponse));
            }
        }
    }
    
    /*Array<double> sinx;
    sinx.clear();
    double samplerate = 44100.0;
    double frequency = 1470.0;
    for (int j=0; j<samplerate/frequency; j++){
        sinx.add(sin((2.0*M_PI*(double)j*frequency)/samplerate));
    }
    std::cout<<"N: "<<sinx.size()<<std::endl;*/
    //for (auto i : sinx){std::cout<<i<<std::endl;}
    
    int NFFTW = impulseResponse.size();
    double samplerate = 44100.0;
    
    fftw_complex *outFFTW;
    double* inFFTW;
    fftw_plan pFFTW;
    
    inFFTW  = (double*) fftw_malloc(sizeof(double)*NFFTW);
    outFFTW = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*NFFTW);
    
    pFFTW = fftw_plan_dft_r2c_1d(NFFTW, inFFTW, outFFTW, FFTW_ESTIMATE);
    
    for (int h=0; h<NFFTW; h++){
        inFFTW[h] = impulseResponse[h];
    }
    
    fftw_execute(pFFTW);
    double hz_per_sample = samplerate/NFFTW;
    /*for (int k=0; k<NFFTW; k++){
        std::cout<<"hz: "<<hz_per_sample*k<<" out: "<<fabs(outFFTW[k][0] + outFFTW[k][1])<<" in: "<<inFFTW[k]<<std::endl;
    }*/
    
    for (int i=0; i<NFFTW; i++){
        frequencyResponse.add(fabs(outFFTW[i][0] + outFFTW[i][1]));
    }
    
    fftw_destroy_plan(pFFTW);
    fftw_free(inFFTW); fftw_free(outFFTW);
}