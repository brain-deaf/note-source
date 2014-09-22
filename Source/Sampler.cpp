/*
  ==============================================================================

    Sampler.cpp
    Created: 28 Jun 2014 2:03:28pm
    Author:  patrick

  ==============================================================================
*/

#include "Sampler.h"
#include "Adsr.h"
#include "InstrumentMappingEditor.h"
#define _USE_MATH_DEFINES
#include <math.h>

static bool isNoteHeld(SelectedItemSet<std::pair<int, int> > s, int n){
    for (auto pair : s){
         
         if (pair.first == n){
             return true;
         }
    }
    return false;
}

SampleVoice::SampleVoice() : SamplerVoice(), /*filter1(), filter2(),*/ samplePosition(0.0f),
                             attackTime(10.0f), attackCurve(0.05f), releaseTime(50.0f), 
                             sampleStart(0.0), releaseCurve(0.01f), volume(1.0), ringMod(false),
                             noteEvent(nullptr), tf_volume(1.0), ringAmount(1.0), angleDelta(0.0),
                             currentAngle(0.0)
{
    //filter1.setCoefficients(IIRCoefficients::makeLowPass(44100.0, 300.0));
    //filter2.setCoefficients(IIRCoefficients::makeLowPass(44100.0, 300.0));
}

void SampleVoice::startNote(const int midiNoteNumber,
                            const float /*velocity*/,
                            SynthesiserSound* s,
                            const int /*pitchWheelPosition*/)
{   
    if (SampleSound* sound = (SampleSound*)s){
		releaseTriggered = false;
        const double a = pow(2.0, 1.0/12.0);
        double old_frequency = 440.0 * pow(a, (double)sound->getRootNote() - 69.0);
        double new_frequency = old_frequency * pow(a, ((float)midiNoteNumber + sound->getTuning()) - sound->getRootNote());
        pitchRatio = new_frequency/old_frequency;
        releaseStart = 0.0f;
        samplePosition = sound->getSampleStart();
        sampleStart = samplePosition;
        
        for (auto e : sound->getSampler()->getIncomingEvents()){
            if (e->getNoteNumber() == midiNoteNumber){
                noteEvent = e;
                sound->getSampler()->getIncomingEvents().removeFirstMatchingValue(e);
                sound->getSampler()->getEvents().add(noteEvent);
                break;
            }
        }

        Array<int> groups_for_note = sound->getGroups();
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
                if (fx->getFxType() == TransformChooser::FX::SINE){
                    SineTransform* ltf= (SineTransform*)fx->getContent();
                    if (ltf->getTargetBox()->getSelectedId() == TransformID::VELOCITY){
                        int gValue = ltf->getGraph()->getGValue();
                        if (gValue != -1)
                            tf_vel_multiplier *= ltf->getGraph()->getTValue();
                    }
                }
            }
        }
        bb = false;
		if (noteEvent != nullptr)
			noteEvent->setVelocity(noteEvent->getVelocity()*tf_vel_multiplier);
    }
}

//y = (y2-y1)/(e^wx2 - e^wx1) * (e^wx - e^wx1) + y1
static double getAttackMultiplier(float attackTime, float attackCurve, float x){
    float answer = (1.0f - 0.0f) / (pow(M_E, attackCurve*attackTime) - pow(M_E, attackCurve*0.0f)) * (pow(M_E, attackCurve*x) - pow(M_E, attackCurve*0.0f)) + 0.0f;
    return answer;
}

static double getReleaseMultiplier(float releaseTime, float releaseCurve, float x){
    float answer = (1.0f - 0.0f) / (pow(M_E, releaseCurve*releaseTime) - pow(M_E, releaseCurve*0.0f)) * (pow(M_E, releaseCurve*x) - pow(M_E, releaseCurve*0.0f)) + 0.0f;
    return 1.0 - answer;
}

static double plotAdsr(double x1, double time, double y1, double max_volume, double curve_width, double x){
	return(max_volume - y1) / (pow(M_E, curve_width*time) - pow(M_E, curve_width*x1)) * (pow(M_E, curve_width*x) - pow(M_E, curve_width*x1)) + y1;
}

void SampleVoice::renderNextBlock(AudioSampleBuffer& buffer, int startSample, int numSamples){
    SampleSound::Ptr s = (SampleSound::Ptr)getCurrentlyPlayingSound();

    if (s != nullptr && noteEvent != nullptr){
        SamplerProcessor* sampler = s->getSampler();
        
        double xfadeLength = s->getXfadeLength();
        bool looping = false;
        if (s->getLoopMode() && s->getLoopEnd() > s->getLoopStart())
            looping = true;
            
        if (!isNoteHeld(*(sampler->getNotesHeld()), noteEvent->getTriggerNote()))
            looping = false;
            
        Array<int> groups_for_note = s->getGroups();
        Array<int> groups_for_event = noteEvent->getGroups();
        bool return_flag = true;
        for (auto j : groups_for_event){
            if (sampler->getGroups()[j]->getSounds().contains(s)){ 
                return_flag = false;
            }
        }
        
        
        for (auto i : groups_for_note){
            FxGroup* fx_group = s->getFxSelector()->getGroups()[i];
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
        
        if ((int)noteEvent->getVelocity() < s->getVelocity().first || (int)noteEvent->getVelocity() > s->getVelocity().second){
            return_flag = true;
        }
        
        if (return_flag){ 
            s->getSampler()->getEvents().removeFirstMatchingValue(noteEvent);
            noteEvent = nullptr;
            stopNote(false);
            return;
        }
        
        double tf_vol_multiplier = 1.0;
        for (auto i : groups_for_note){
            TransformGroup* tf_group = s->getTransformSelector()->getGroups()[i];
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
                if (fx->getFxType() == TransformChooser::FX::SINE){
                    SineTransform* ltf= (SineTransform*)fx->getContent();
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
            
        
        double vol = noteEvent->getVolume();
        double vol_difference = vol - volume;
        double difference_per_sample = vol_difference/numSamples;
        
        double sample_length = s->getAudioData()->getNumSamples();
        int num_channels = s->getAudioData()->getNumChannels();
        if (getCurrentlyPlayingSound().get()){
            const float* const inL = s->getAudioData()->getReadPointer(0);
            const float* const inR = num_channels>1 ? s->getAudioData()->getReadPointer(1) : nullptr;
            float* outL = buffer.getWritePointer(0, startSample);
            float* outR = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1, startSample) : nullptr;
            
			//sometimes noteEvent->getTriggerNote() returns the wrong note...

            if (!isNoteHeld(*(s->getSampler()->getNotesHeld()), noteEvent->getTriggerNote())
                && noteEvent->getTriggerNote() != -1
                && releaseStart == 0.0f)
			{
				bool b = !isNoteHeld(*(s->getSampler()->getNotesHeld()), noteEvent->getTriggerNote());
				//samplePosition = s->getReleaseStart();
                releaseStart = samplePosition;
            }
            double release_sample_length = releaseTime/1000*s->getSampleRate();
            float release_x = 0.0f;
            
            int start = samplePosition;           
            
            for (int i= start; i<numSamples+start; i++){
                double x = (samplePosition - sampleStart)/s->getSampleRate()*1000;
                double attack_multiplier = x<attackTime ? getAttackMultiplier(attackTime, attackCurve, x) : 1.0;                      
                                           
                double samples_left = sample_length - samplePosition;
                
                if (samples_left < release_sample_length && releaseStart == 0.0){
                    releaseStart = samplePosition;
                }
                if (releaseStart != 0.0){
					release_x = releaseTriggered ? (samplePosition - s->getReleaseStart()) / s->getSampleRate() * 1000 : (samplePosition - releaseStart) / s->getSampleRate() * 1000;
                }
                
                double release_multiplier = releaseStart != 0.0 || samples_left < release_sample_length ? getReleaseMultiplier(releaseTime, releaseCurve, release_x) : 1.0;
            
                const int pos = (int) samplePosition;
                const double alpha = (double) (samplePosition - pos);
                const double invAlpha = 1.0f - alpha;
                //double num_samples = (double)numSamples;
                
                // just using a very simple linear interpolation here..
                float l = ((inL[pos]) * invAlpha +  (inL[pos+1]) * alpha);
                float r = inR != nullptr ? (inR[pos]) * invAlpha + (inR[pos+1] * alpha) : l;

				if (releaseStart != 0 && samplePosition - releaseStart > s->getReleaseTime() && !releaseTriggered){
					samplePosition = s->getReleaseStart() + s->getReleaseTime();
					releaseTriggered = true;
				}

				if (releaseStart != 0 && samplePosition - releaseStart < s->getReleaseTime()){
					double release_pos = s->getReleaseStart() + (samplePosition - releaseStart);
					double xfade_counter = release_pos - s->getReleaseStart();

					const int pos2 = (int)release_pos;
					const double alpha2 = (double)(release_pos - pos2);
					const double invAlpha2 = 1.0f - alpha2;
					float l2 = ((inL[pos2]) * invAlpha2 + (inL[pos2 + 1]) * alpha2);
					float r2 = inR != nullptr ? (inR[pos2]) * invAlpha2 + (inR[pos2 + 1] * alpha2) : l2;

					double resolution = 1000.0;

					double x1 = (s->getReleaseStart()) / resolution;
					double x2 = (s->getReleaseStart() + s->getReleaseTime()) / resolution;
					double y1 = resolution;
					double y2 = 0.0;
					double curve = s->getReleaseCurve();
					double _x = release_pos / resolution;

					double _y1 = plotAdsr(x1, x2, y1, y2, curve, _x) / resolution;

					l *= _y1;
					r *= _y1;

					l += (1.0 -_y1) * l2;
					r += (1.0 -_y1) * r2;
				}
                
                double start_pos;
                if (looping && s->getLoopEnd() - samplePosition <= xfadeLength){
                    start_pos = s->getLoopStart() + xfadeLength - (s->getLoopEnd() - samplePosition);
                    double xfade_counter = xfadeLength - (s->getLoopEnd() - samplePosition);
                    
                    const int pos2 = (int) start_pos;
                    const double alpha2 = (double) (start_pos - pos2);
                    const double invAlpha2 = 1.0f - alpha2;
                    float l2 = ((inL[pos2]) * invAlpha2 +  (inL[pos2+1]) * alpha2);
                    float r2 = inR != nullptr ? (inR[pos2]) * invAlpha2 + (inR[pos2+1] * alpha2) : l2;

					double resolution = 1000.0;

					double x1 = (s->getLoopEnd() - s->getXfadeLength()) / resolution;
					double x2 = s->getLoopEnd() / 1000.0;
					double y1 = resolution;
					double y2 = 0.0;
					double curve = s->getXfadeCurve();
					double _x = (s->getLoopEnd() - s->getXfadeLength() + xfade_counter) / resolution;

					double _y1 = plotAdsr(x1, x2, y1, y2, curve, _x) / resolution;

					x1 = s->getLoopStart() / resolution;
					x2 = (s->getLoopStart() + s->getXfadeLength()) / resolution;
					y1 = 0.0;
					y2 = resolution;
					curve = s->getXfadeCurve();
					_x = (s->getLoopStart() + xfade_counter) / resolution;

					double _y2 = plotAdsr(x1, x2, y1, y2, curve, _x) / resolution;
                    
                    l *= _y1;
                    r *= _y1;
                    
                    l += _y2 * l2;
                    r += _y2 * r2;
                }
                
                double ringMult = ringMod ? sin(currentAngle) : 1.0;
                
                *outL += l*attack_multiplier*release_multiplier*
                         (volume+difference_per_sample*(i-start))*
                         (tf_volume+tf_difference_per_sample*(i-start))*
                         sampler->getInstrumentVolume();
                         
                *outR += r*attack_multiplier*release_multiplier*
                         (volume+difference_per_sample*(i-start))*
                         (tf_volume+tf_difference_per_sample*(i-start))*
                         sampler->getInstrumentVolume();
                         
                if (ringMod){
                    float L = *outL;
                    float R = *outR;
                    
                    *outL *= 1.0 - ringAmount;
                    *outL += L*ringMult*ringAmount;
                    *outR *= 1.0 - ringAmount;
                    *outR += R*ringMult*ringAmount;
                }
                
                outL++;
                outR++;
                currentAngle += angleDelta;
                samplePosition += pitchRatio;
                
                if (!looping && samplePosition > sample_length || sample_length - samplePosition < numSamples || release_x >= releaseTime){
                    samplePosition = 0.0;
                    s->getSampler()->getEvents().removeFirstMatchingValue(noteEvent);
                    currentAngle = 0.0;
                    noteEvent = nullptr;
                    stopNote(false);
                    ringMod = false;
                    break;
                }
                
                if (looping && samplePosition >= s->getLoopEnd()+0){
                    samplePosition = s->getLoopStart() + xfadeLength;
                }
            }
            
            
            volume = vol;
            tf_volume = tf_vol_multiplier;
        }
    }
}