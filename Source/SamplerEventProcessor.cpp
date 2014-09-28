/*
  ==============================================================================

    SamplerEventProcessor.cpp
    Created: 30 Aug 2014 1:32:00pm
    Author:  patrick

  ==============================================================================
*/

#include "SamplerEventProcessor.h"
#include "Adsr.h"
#include "InstrumentMappingEditor.h"
#include "MainComponent.h"
#define _USE_MATH_DEFINES
#include <math.h>

//y = (y2-y1)/(e^wx2 - e^wx1) * (e^wx - e^wx1) + y1
static double getAttackMultiplier(float attackTime, float attackCurve, float x){
    float answer = (1.0f - 0.0f) / (pow(M_E, attackCurve*attackTime) - pow(M_E, attackCurve*0.0f)) * (pow(M_E, attackCurve*x) - pow(M_E, attackCurve*0.0f)) + 0.0f;
    return answer;
}

static double getReleaseMultiplier(float releaseTime, float releaseCurve, float x){
    float answer = (1.0f - 0.0f) / (pow(M_E, releaseCurve*releaseTime) - pow(M_E, releaseCurve*0.0f)) * (pow(M_E, releaseCurve*x) - pow(M_E, releaseCurve*0.0f)) + 0.0f;
    return 1.0 - answer;
}


SamplerEventProcessor::SamplerEventProcessor() : sampleCount(0), events(), samplePosition(0.0f),
attackTime(10.0f), attackCurve(0.05f), releaseTime(50.0f),
sampleStart(0.0f), releaseCurve(0.01f), volume(1.0), ringMod(false),
tf_volume(1.0), ringAmount(1.0), angleDelta(0.0),
currentAngle(0.0), sound(nullptr), maxSampleCount(0), data()
{

}

void SamplerEventProcessor::renderAllEvents(){
    data = Data();
    data.push_back(std::vector<float>());
    data.push_back(std::vector<float>());
    sampleCount = 0;
    maxSampleCount = 0;
    for (auto i : events){
        startSamplerEvent(i);
    }
    
	float* f_l = &(data[0][0]);
    for (int i=0; i<data[0].size(); i++){
        f_l[i] = data[0][i];
    }
    float* f_r = &(data[1][0]);
    for (int i=0; i<data[1].size(); i++){
        f_r[i] = data[1][i];
    }
    

    ScopedPointer<float*> f_d = new float*[2];
    f_d[0] = f_l;
    f_d[1] = f_r;
    const float* const* f_channels = static_cast<const float* const*>(f_d.get());
    
    ScopedPointer<WavAudioFormat> w = new WavAudioFormat();
    Array<int> bits = w->getPossibleBitDepths();
    File f = File(File::getCurrentWorkingDirectory().getFullPathName() + "/temp.wav");
    ScopedPointer<AudioFormatWriter> a = w->createWriterFor(new FileOutputStream(f), 44100.0, 2,
                                               bits[bits.size()-1], StringPairArray(), 0);
    
    a->writeFromFloatArrays(f_channels, 2, data[0].size());
    
	a = nullptr;
    w = nullptr;
}

void SamplerEventProcessor::startSamplerEvent(SamplerEvent s)
{   
   
	for (int j = 0; j<MainContentComponent::_static_sampler->getSynth()->getNumSounds(); j++){
		sound = static_cast<SampleSound*>(MainContentComponent::_static_sampler->getSynth()->getSound(j));
        if (sound == nullptr)
            continue;
        Array<int> groups_for_note = sound->getGroups();
        double tf_vel_multiplier = 1.0;
        
        for (auto i : groups_for_note){
             TransformGroup* tf_group = sound->getTransformSelector()->getGroups()[i];
             for (auto fx : tf_group->group_fx){
                if (fx->getFxType() == TransformChooser::FX::LINEAR){
                    LinearTransform* ltf= (LinearTransform*)fx->getContent();
                    if (ltf->getTargetBox()->getSelectedId() == TransformID::VELOCITY){
                        int gValue = ltf->getGraph()->getGValue();
                        if (gValue != -1)
                            tf_vel_multiplier *= ltf->getGraph()->getTValue();
                    }
                }
                if (fx->getFxType() == TransformChooser::FX::EXPONENTIAL){
                    ExponentialTransform* ltf= (ExponentialTransform*)fx->getContent();
                    if (ltf->getTargetBox()->getSelectedId() == TransformID::VELOCITY){
                        int gValue = ltf->getGraph()->getGValue();
                        if (gValue != -1)
                            tf_vel_multiplier *= ltf->getGraph()->getTValue();
                    }
                }
            }
        }
        s.setVelocity(s.getVelocity()*tf_vel_multiplier);
        
        if (sound->appliesToNote(s.getNoteNumber()) &&
            s.getVelocity() >= sound->getVelocity().first &&
            s.getVelocity() <= sound->getVelocity().second){
            const double a = pow(2.0, 1.0/12.0);
            double old_frequency = 440.0 * pow(a, (double)sound->getRootNote() - 69.0);
            double new_frequency = old_frequency * pow(a, (float)s.getNoteNumber() - sound->getRootNote());
            pitchRatio = new_frequency/old_frequency;
            releaseStart = 0.0f;
            samplePosition = sound->getSampleStart();
            sampleStart = samplePosition;

            for (auto i : groups_for_note){
                FxGroup* fx_group = sound->getFxSelector()->getGroups()[i];
                for (auto fx : fx_group->group_fx){
                    if (fx->getFxType() == FxChooser::FX::ADSR){
                        Adsr* adsr  = (Adsr*)fx->getContent();
                        attackTime  = adsr->getAttackTimeSlider()->getValue();
                        attackCurve = adsr->getAttackCurveSlider()->getValue();
                        decayTime   = adsr->getDecayTimeSlider()->getValue();
                        decayCurve  = adsr->getDecayCurveSlider()->getValue();
                        sustain     = adsr->getSustainSlider()->getValue();
                        releaseTime = adsr->getReleaseTimeSlider()->getValue();
                        releaseCurve= adsr->getReleaseCurveSlider()->getValue();
                    }
                }
            }
            renderSamplerEvent(s, data);
        }
    }
}

void SamplerEventProcessor::renderSamplerEvent(SamplerEvent noteEvent, Data& output){

        std::cout<<"rendering event"<<std::endl;
        long long numSamples = (static_cast<SamplerSound*>(sound))->getAudioData()->getNumSamples();
        //std::cout<<"num samples: "<<numSamples<<std::endl;
        
		long long sampleCount = MainContentComponent::_static_sampler->getWavSampleCount();

        double xfadeLength = sound->getXfadeLength();
        bool looping = false;
        if (sound->getLoopMode() && sound->getLoopEnd() > sound->getLoopStart())
            looping = true;
            
        //if (!isNoteHeld(*(sampler->getNotesHeld()), noteEvent->getTriggerNote()))
            //looping = false;
            
        Array<int> groups_for_note = sound->getGroups();
        Array<int> groups_for_event = noteEvent.getGroups();
        bool return_flag = true;
        if (groups_for_event.size() > 0){
            for (auto i : groups_for_note){
                for (auto j : groups_for_event){
                    if (j == i){ 
                        return_flag = false;
                    }
                }
            }
        }
        
        for (auto i : groups_for_note){
            FxGroup* fx_group = sound->getFxSelector()->getGroups()[i];
            for (auto fx : fx_group->group_fx){
                if (fx->getFxType() == FxChooser::FX::RINGMOD){
                    RingModulator* ringModulator = static_cast<RingModulator*>(fx->getContent());
                    ringMod = true;
                    double cyclesPerSecond = ringModulator->getFrequencySlider()->getValue();
                    double cyclesPerSample = cyclesPerSecond/44100.0;
                    angleDelta = cyclesPerSample*2.0*M_PI*double_Pi;
                    ringAmount = ringModulator->getAmplitudeSlider()->getValue();
                }
            }
        }
        
        if ((int)noteEvent.getVelocity() < sound->getVelocity().first || (int)noteEvent.getVelocity() > sound->getVelocity().second){
            return_flag = true;
        }
        
        if (return_flag){ 
            return;
        }
        
        double tf_vol_multiplier = 1.0;
        for (auto i : groups_for_note){
            TransformGroup* tf_group = sound->getTransformSelector()->getGroups()[i];
            for (auto fx : tf_group->group_fx){
                if (fx->getFxType() == TransformChooser::FX::LINEAR){
                    LinearTransform* ltf= (LinearTransform*)fx->getContent();
                    //ltf->getLFO()->elapseTime(numSamples);
                    if (ltf->getTargetBox()->getSelectedId() == TransformID::VOLUME){
                        int gValue = ltf->getGraph()->getGValue();
                        if (gValue != -1)
                            tf_vol_multiplier *= ltf->getGraph()->getTValue();
                    }
                }
                if (fx->getFxType() == TransformChooser::FX::EXPONENTIAL){
                    ExponentialTransform* ltf= (ExponentialTransform*)fx->getContent();
                    if (ltf->getTargetBox()->getSelectedId() == TransformID::VOLUME){
                        int gValue = ltf->getGraph()->getGValue();
                        if (gValue != -1)
                            tf_vol_multiplier *= ltf->getGraph()->getTValue();
                    }
                }
            }
        }
        
        double tf_vol_difference = tf_vol_multiplier - tf_volume;
        double tf_difference_per_sample = tf_vol_difference/numSamples;
            
        
        double vol = noteEvent.getVolume();
        double vol_difference = vol - volume;
        double difference_per_sample = vol_difference/numSamples;
        
        double sample_length = sound->getAudioData()->getNumSamples();
        int num_channels = sound->getAudioData()->getNumChannels();
        
        
        if (sound != nullptr){
            //int startSample = 0;
            const float* const inL = sound->getAudioData()->getReadPointer(0);
            const float* const inR = num_channels>1 ? sound->getAudioData()->getReadPointer(1) : nullptr;
            //float* outL = sound->getAudioData()->getWritePointer(0, startSample);
            //float* outR = sound->getAudioData()->getNumChannels() > 1 ? sound->getAudioData()->getWritePointer(1, startSample) : nullptr;
            float outLeft(0.0);
            float outRight(0.0);
            
            
            double release_sample_length = releaseTime/1000*sound->getSampleRate();
            float release_x = 0.0f;
            
            int start = samplePosition;
            long long event_start = noteEvent.getStart();
            sampleCount = 0;
            for (int i=0; i<event_start; i++){
                if (output[0].size() > sampleCount){
                    output[0][i] += 0.0;
                    output[1][i] += 0.0;
                }else{
                    output[0].push_back(0.0);
                    output[1].push_back(0.0);
                }
                sampleCount++;
            }

            for (int i= start; i<numSamples+start; i++){
                double x = (samplePosition - sampleStart)/sound->getSampleRate()*1000;
                double attack_multiplier = x<attackTime ? getAttackMultiplier(attackTime, attackCurve, x) : 1.0;                      
                                           
                double samples_left = sample_length - samplePosition;
                
                if (samples_left < release_sample_length && releaseStart == 0.0){
                    releaseStart = samplePosition;
                }
                if (releaseStart != 0.0){
                    release_x = (samplePosition - releaseStart)/sound->getSampleRate()*1000;
                }
                
                double release_multiplier = releaseStart != 0.0 || samples_left < release_sample_length ? getReleaseMultiplier(releaseTime, releaseCurve, release_x) : 1.0;
            
                const int pos = (int) samplePosition;
                const double alpha = (double) (samplePosition - pos);
                const double invAlpha = 1.0f - alpha;
                //double num_samples = (double)numSamples;
                
                // just using a very simple linear interpolation here..
                float l = ((inL[pos]) * invAlpha +  (inL[pos+1]) * alpha);
                float r = inR != nullptr ? (inR[pos]) * invAlpha + (inR[pos+1] * alpha) : l;
                
                double start_pos;
                if (looping && sound->getLoopEnd() - samplePosition <= xfadeLength){
                    start_pos = sound->getLoopStart() + xfadeLength - (sound->getLoopEnd() - samplePosition);
                    double xfade_counter = xfadeLength - (sound->getLoopEnd() - samplePosition);
                    
                    const int pos2 = (int) start_pos;
                    const double alpha2 = (double) (start_pos - pos2);
                    const double invAlpha2 = 1.0f - alpha2;
                    float l2 = ((inL[pos2]) * invAlpha2 +  (inL[pos2+1]) * alpha2);
                    float r2 = inR != nullptr ? (inR[pos2]) * invAlpha2 + (inR[pos2+1] * alpha2) : l2;
                    
                    l *= (xfadeLength - xfade_counter) / xfadeLength;
                    r *= (xfadeLength - xfade_counter) / xfadeLength;
                    
                    l += xfade_counter / xfadeLength * l2;
                    r += xfade_counter / xfadeLength * r2;
                }
                
                
                double ringMult = ringMod ? sin(currentAngle) : 1.0;
                
                /**outL += l*attack_multiplier*release_multiplier*
                         (volume+difference_per_sample*(i-start))*
                         (tf_volume+tf_difference_per_sample*(i-start));
                         
                *outR += r*attack_multiplier*release_multiplier*
                         (volume+difference_per_sample*(i-start))*
                         (tf_volume+tf_difference_per_sample*(i-start));*/
                         
                outLeft = l*attack_multiplier*release_multiplier*
                         (volume+difference_per_sample*(i-start))*
                         (tf_volume+tf_difference_per_sample*(i-start));
                         
                outRight = r*attack_multiplier*release_multiplier*
                         (volume+difference_per_sample*(i-start))*
                         (tf_volume+tf_difference_per_sample*(i-start));
                         
                if (ringMod){
                    float L = outLeft;
                    float R = outRight;
                    
                    outLeft *= 1.0 - ringAmount;
                    outLeft += L*ringMult*ringAmount;
                    outRight *= 1.0 - ringAmount;
                    outRight += R*ringMult*ringAmount;
                }
                
                if (output[0].size() > sampleCount){
                    output[0][sampleCount] += outLeft;
                    output[1][sampleCount] += outRight;
                }else{
                    output[0].push_back(outLeft);
                    output[1].push_back(outRight);
                }
                
                currentAngle += angleDelta;
                samplePosition += pitchRatio;
                sampleCount++;

                if (sampleCount > maxSampleCount){
                    maxSampleCount = sampleCount;
                }
                
                if (!looping && samplePosition > sample_length || /*sample_length - samplePosition < numSamples ||*/ release_x >= releaseTime){
                    //std::cout<<"stop rendering: "<<sample_length<<" "<<samplePosition<<" "<<numSamples<<std::endl;
                    samplePosition = 0.0;
                    currentAngle = 0.0;
                    ringMod = false;
                    break;
                }
                
                if (looping && samplePosition >= sound->getLoopEnd()+0){
                    samplePosition = sound->getLoopStart() + xfadeLength;
                }
            }
            volume = vol;
            tf_volume = tf_vol_multiplier;
        }
        std::cout<<"done rendering event"<<std::endl;
}