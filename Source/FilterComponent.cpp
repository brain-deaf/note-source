/*
  ==============================================================================

    FilterComponent.cpp
    Created: 19 Jul 2014 12:32:31pm
    Author:  patrick

  ==============================================================================
*/

#include "FilterComponent.h"
#include <cmath>


static int check_band(double d, Array<int> bands){
    for (int i=0; i<bands.size(); i++){
        if (d >= bands[i] && d < bands[i+1]){
            return i;
        }
    }
    return -1;
}

FilterComponent::FilterComponent() : Component(), filter(nullptr){}

void FilterComponent::paint(Graphics& g){
    g.fillAll(Colours::black);
    
    if (filter != nullptr){
        Array<double> impulse_response = filter->getImpulseResponse();
        Array<double> interpolated_impulse = filter->getInterpolatedImpulseResponse();
        Array<double> frequency_response = filter->getFrequencyResponse();
        double grid_width = getWidth() / ((double)(frequency_response.size()));
    
        g.setColour(Colours::blue);
        g.setOpacity(1.0f);
        Path myPath;
        myPath.startNewSubPath(0, getHeight()/5.0f * 4.0f);
        
        float sample_resolution = 20.0;
        
        /*for (int i=0; i<impulse_response.size(); i++){
            if (i < filter->getZeroHeadingSize()){
                myPath.lineTo(i*grid_width, (getHeight()/5.0f * 4.0f) - 500.0f * impulse_response[i]);
                interpolatedImpulseResponse.add(impulse_response[i]);
            }else{
                for (int j=0; j<sample_resolution; j++){
                    float sample_count = (float)i + ( ((float)j) * (1.0/sample_resolution));
                    double y = (getHeight()/5.0f * 4.0f) - 500.0f * polynomialInterpolation(sample_count, impulse_response);
                    myPath.lineTo(sample_count * grid_width, y);
                }
            }
        }*/
        
        /*std::cout<<"grid width: "<<grid_width<<" get width: "<<getWidth()<<" size: "<<interpolated_impulse.size()<<std::endl;
        for (int i=0; i<interpolated_impulse.size(); i++){
            //if (i < 500 /*|| interpolated_impulse[i] > 0.00001*///){
                /*myPath.lineTo(i*grid_width, (getHeight()/5.0f * 4.0f) - 500.0f * interpolated_impulse[i]);
                std::cout<<"x: "<<i*grid_width<<" y: "<<(getHeight()/5.0f * 4.0f) - 500.0f * interpolated_impulse[i]<<std::endl;
            }else{
                myPath.lineTo(i*grid_width, (getHeight()/5.0f * 4.0f));
                //std::cout<<"x: "<<i*grid_width<<" y: "<<getHeight()/5.0f*4.0f<<std::endl;
            }
        }*/
        
        double hz_per_sample = 44100.0/((double)(frequency_response.size()));
        double max_hz = 15000.0;
        
        int num_bands = 10;
        int start_band_offset = 40;
        
        double actual_width = getWidth() - 200.0; //why is getWidth() not working?
        double band_width = actual_width/num_bands;
        
        /*for (int i=0; i<num_bands; i++){
            myPath.startNewSubPath(band_width*i, 0);
            myPath.lineTo(band_width*i, getHeight());
        }*/
            
        myPath.startNewSubPath(0, getHeight()/5.0f * 4.0f);
        grid_width = actual_width/(max_hz/hz_per_sample);
        
        double C = actual_width / log2(max_hz/hz_per_sample - 1);
        int band;
        for (int i=0; i<max_hz/hz_per_sample; i++){
            double y =  (getHeight()/5.0f*4.5f) - 200.0* frequency_response[i];
            if (!std::isnan(y)){
                if (i !=0){
                    myPath.lineTo(C*log2(i), y);
                }else{
                    myPath.lineTo(0, y);
                }
            }else{
                myPath.lineTo(C*log2(i), (getHeight()/5.0f*4.5f));
            }
        }
        
        Array<double> bands;
        bands.add(40.0);
        bands.add(80.0);
        bands.add(160.0);
        bands.add(320.0);
        bands.add(640.0);
        bands.add(1280.0);
        bands.add(2560.0);
        bands.add(5120.0);
        bands.add(10240.0);
        
        for (int i=0; i<bands.size(); i++){
            double sample = bands[i] / hz_per_sample;
            myPath.startNewSubPath(C*log2(sample), 0);
            myPath.lineTo(C*log2(sample), getHeight());
        }
        
        g.strokePath (myPath, PathStrokeType (1.0f));
        
    }
}

