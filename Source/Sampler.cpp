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

static bool isNoteHeld(SelectedItemSet<std::pair<int, int> > s, int n){
    for (auto pair : s){
         
         if (pair.first == n){
             return true;
         }
    }
    return false;
}

Sampler::Sampler(SelectedItemSet<std::pair<int, int> >* s) 
    : AudioSource(), synth(), idCount(0), notesHeld(s), formatManager(), 
      events(), incomingEvents(), wavFormat(nullptr), wavWriter(nullptr),/*wavOutput(new FileOutputStream(File(File::getCurrentWorkingDirectory()).getFullPathName() + "/temp.wav")),*/
      filter1(), filter2(), fx_selector(nullptr), transform_selector(nullptr),
      wavSampleCount(0.0), wavOutput(nullptr)
{
    //Array<int> bits = wavFormat.getPossibleBitDepths();
    //wavWriter = wavFormat.createWriterFor(wavOutput.get(), 44100.0, 2,
      //                                    bits[bits.size()-1], StringPairArray(), 0);
    
    for (int i=0; i<256; i++){
        synth.addVoice(new SampleVoice());
    }
    formatManager.registerBasicFormats();
    filter1.setCoefficients(IIRCoefficients::makeLowPass(44100.0, 6000.0));
    filter2.setCoefficients(IIRCoefficients::makeLowPass(44100.0, 10000.0));
    /*filter1.setCoefficients(IIRCoefficients::makeLowPass(44100.0, 7000.0));
    filter1.setCoefficients(IIRCoefficients::makeLowPass(44100.0, 7000.0));
    filter1.setCoefficients(IIRCoefficients::makeLowPass(44100.0, 7000.0));
    filter1.setCoefficients(IIRCoefficients::makeLowPass(44100.0, 7000.0));
    filter1.setCoefficients(IIRCoefficients::makeLowPass(44100.0, 7000.0));*/
    
    //filter1.setCoefficients(IIRCoefficients());
    //filter2.setCoefficients(IIRCoefficients());
}

void Sampler::setupRendering(){
    File f = File(File::getCurrentWorkingDirectory().getFullPathName() + "/temp.wav");
    if (f.exists())
        f.deleteFile();                                 
}
                                          
void Sampler::addSample(String path, int root_note, int note_low, int note_high, Array<int>& groups, PlaySettings* p, std::pair<int, int> v){
    ScopedPointer<AudioFormatReader> audioReader(formatManager.createReaderFor(File(path)));
        
    BigInteger allNotes;
    for (int i=note_low; i<note_high; i++){
        allNotes.setBit(i);
    }
    SampleSound* ss;
    synth.addSound(ss = new SampleSound("demo sound", *audioReader,
                                    allNotes, root_note,
                                    0.0, 0.0, 10.0, groups, fx_selector, tf_selector, this, v));
    ss->setSampleStart(p->getSampleStart());
    ss->setLoopMode(p->getLoopMode());
    ss->setLoopStart(p->getLoopStart());
    ss->setLoopEnd(p->getLoopEnd());
    ss->setXfadeLength(p->getXfadeLength());
    
    //std::cout<<v.first<<" "<<v.second<<std::endl;
}
    
void Sampler::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    midiCollector.reset(sampleRate);
    synth.setCurrentPlaybackSampleRate(sampleRate);
}
    
void Sampler::releaseResources() {}
    
void Sampler::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) {
    bufferToFill.clearActiveBufferRegion();
    MidiBuffer incomingMidi;
    midiCollector.removeNextBlockOfMessages(incomingMidi, bufferToFill.numSamples);

    synth.renderNextBlock(*bufferToFill.buffer, incomingMidi, 0, bufferToFill.numSamples);
    
    
    //add filter processing to the output channels 1 and 2
    //filter1.processSamples(bufferToFill.buffer->getWritePointer(0), bufferToFill.buffer->getNumSamples());
    //filter2.processSamples(bufferToFill.buffer->getWritePointer(1), bufferToFill.buffer->getNumSamples());
}

SampleVoice::SampleVoice() : SamplerVoice(), /*filter1(), filter2(),*/ samplePosition(0.0f),
                             attackTime(10.0), attackCurve(0.05), releaseTime(50.0), 
                             sampleStart(0.0), releaseCurve(0.01), volume(1.0), ringMod(false),
                             noteEvent(nullptr), tf_volume(1.0), ringAmount(1.0), angleDelta(0.0),
                             currentAngle(0.0)
{
    //filter1.setCoefficients(IIRCoefficients::makeLowPass(44100.0, 300.0));
    //filter2.setCoefficients(IIRCoefficients::makeLowPass(44100.0, 300.0));
}

void SampleVoice::startNote(const int midiNoteNumber,
                            const float velocity,
                            SynthesiserSound* s,
                            const int pitchWheelPosition)
{   
    if (SampleSound* sound = (SampleSound*)s){
        const double a = pow(2.0, 1.0/12.0);
        double old_frequency = 440.0 * pow(a, (double)sound->getRootNote() - 69.0);
        double new_frequency = old_frequency * pow(a, (float)midiNoteNumber - sound->getRootNote());
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
            }
        }
        bb = false;
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

void SampleVoice::renderNextBlock(AudioSampleBuffer& buffer, int startSample, int numSamples){
    SampleSound::Ptr s = (SampleSound::Ptr)getCurrentlyPlayingSound();

    if (s != nullptr){
        Sampler* sampler = s->getSampler();
        
        double xfadeLength = s->getXfadeLength();
        bool looping = false;
        if (s->getLoopMode() && s->getLoopEnd() > s->getLoopStart())
            looping = true;
            
        if (!isNoteHeld(*(sampler->getNotesHeld()), noteEvent->getTriggerNote()))
            looping = false;
            
        Array<int> groups_for_note = s->getGroups();
        Array<int> groups_for_event = noteEvent->getGroups();
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
            }
        }
        
        double tf_vol_difference = tf_vol_multiplier - tf_volume;
        double tf_difference_per_sample = tf_vol_difference/numSamples;
            
        
        double vol = noteEvent->getVolume();
        double vol_difference = vol - volume;
        double difference_per_sample = vol_difference/numSamples;
        //example of how to not process a sound for a particular Group
        //if (groups_for_note[0] == 0){return;}
        
        double sample_length = s->getAudioData()->getNumSamples();
        int num_channels = s->getAudioData()->getNumChannels();
        if (getCurrentlyPlayingSound().get()){
            const float* const inL = s->getAudioData()->getReadPointer(0);
            const float* const inR = num_channels>1 ? s->getAudioData()->getReadPointer(1) : nullptr;
            float* outL = buffer.getWritePointer(0, startSample);
            float* outR = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1, startSample) : nullptr;
            
            if (!isNoteHeld(*(s->getSampler()->getNotesHeld()), noteEvent->getTriggerNote())
                && noteEvent->getTriggerNote() != -1
                && releaseStart == 0.0f){
                releaseStart = samplePosition;
            }
            double release_sample_length = releaseTime/1000*s->getSampleRate();
            float release_x = 0.0f;
            
            int start = samplePosition;
    
            /*double* inFL  = (double*) fftw_malloc(sizeof(double)*((int)(pitchRatio*numSamples)));
            double* inFR  = (double*) fftw_malloc(sizeof(double)*((int)(pitchRatio*numSamples)));
            fftw_complex* outFFTW = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*((int)(pitchRatio*numSamples)));
            fftw_complex* outFFTWR = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*((int)(pitchRatio*numSamples)));
            double* outFL = (double*) fftw_malloc(sizeof(double)*((int)(pitchRatio*numSamples)));
            double* outFR = (double*) fftw_malloc(sizeof(double)*((int)(pitchRatio*numSamples)));
            
            fftw_plan pFFTW;
            fftw_plan pFFTWR;
            fftw_plan p2FFTW;
            fftw_plan p2FFTWR;
    
            pFFTW  = fftw_plan_dft_r2c_1d((int)(pitchRatio*numSamples), inFL, outFFTW, FFTW_ESTIMATE);
            pFFTWR = fftw_plan_dft_r2c_1d((int)(pitchRatio*numSamples), inFR, outFFTWR, FFTW_ESTIMATE);
            p2FFTW = fftw_plan_dft_c2r_1d((int)(pitchRatio*numSamples), outFFTW, outFL, FFTW_ESTIMATE);
            p2FFTWR= fftw_plan_dft_c2r_1d((int)(pitchRatio*numSamples), outFFTWR, outFR, FFTW_ESTIMATE);
            
            //std::cout<<numSamples*pitchRatio<<std::endl;
            for (int i=start; i<start+((int)(pitchRatio*numSamples)); i++){
                inFL[i-start] = inL[i];
                if (inR != nullptr){
                    inFR[i-start] = inR[i];
                }
            }
            
            fftw_execute(pFFTW);
            fftw_execute(pFFTWR);
            fftw_execute(p2FFTW);
            fftw_execute(p2FFTWR);*/
            
            //std::cout<<std::endl<<outFL[50]/((int)(pitchRatio*numSamples))<<std::endl;
            //std::cout<<inL[start+50]<<std::endl;
            
            //int pxn = (int)(pitchRatio*numSamples);
            
            //std::cout<<"start"<<std::endl;
            
            
            for (int i= start; i<numSamples+start; i++){
                double x = (samplePosition - sampleStart)/s->getSampleRate()*1000;
                double attack_multiplier = x<attackTime ? getAttackMultiplier(attackTime, attackCurve, x) : 1.0;                      
                                           
                double samples_left = sample_length - samplePosition;
                
                if (samples_left < release_sample_length && releaseStart == 0.0){
                    releaseStart = samplePosition;
                }
                if (releaseStart != 0.0){
                    release_x = (samplePosition - releaseStart)/s->getSampleRate()*1000;
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
                if (looping && s->getLoopEnd() - samplePosition <= xfadeLength){
                    start_pos = s->getLoopStart() + xfadeLength - (s->getLoopEnd() - samplePosition);
                    double xfade_counter = xfadeLength - (s->getLoopEnd() - samplePosition);
                    
                    const int pos2 = (int) start_pos;
                    const double alpha2 = (double) (start_pos - pos2);
                    const double invAlpha2 = 1.0f - alpha2;
                    float l2 = ((inL[pos2]) * invAlpha2 +  (inL[pos2+1]) * alpha2);
                    float r2 = inR != nullptr ? (inR[pos2]) * invAlpha2 + (inR[pos2+1] * alpha2) : l2;
                    
                    l *= (xfadeLength - xfade_counter) / xfadeLength;
                    r *= (xfadeLength - xfade_counter) / xfadeLength;
                    
                    l += xfade_counter / xfadeLength * l2;
                    r += xfade_counter / xfadeLength * r2;
                    
                    //if (xfade_counter > loop_xfade_length/10*9)
                        //std::cout<<xfade_counter<<std::endl;
                    //std::cout<<xfade_counter<<" "<<(loop_xfade_length - xfade_counter) / loop_xfade_length<<std::endl;
                    
                }
                
                
                //process fx after interpolation...
                //l = filter1.processSingleSampleRaw(l);
                //r = filter2.processSingleSampleRaw(r);*/

                //std::cout<<noteEvent->getVolume()<<std::endl;
                
                double ringMult = ringMod ? sin(currentAngle) : 1.0;
                
                *outL += l*attack_multiplier*release_multiplier*
                         (volume+difference_per_sample*(i-start))*
                         (tf_volume+tf_difference_per_sample*(i-start));
                         
                *outR += r*attack_multiplier*release_multiplier*
                         (volume+difference_per_sample*(i-start))*
                         (tf_volume+tf_difference_per_sample*(i-start));
                         
                if (ringMod){
                    float L = *outL;
                    float R = *outR;
                    
                    *outL *= 1.0 - ringAmount;
                    *outL += L*ringMult*ringAmount;
                    *outR *= 1.0 - ringAmount;
                    *outR += R*ringMult*ringAmount;
                }
                
                //std::cout<<*outL<<std::endl;
                if (looping && samplePosition >= s->getLoopEnd()-1)
                //std::cout<<std::endl<<*outL<<std::endl;

                if ((int)samplePosition == s->getLoopStart() + xfadeLength){
                    //std::cout<<std::endl<<"start after processing: "<<*outL<<std::endl;
                }else{
                    //std::cout<<"f";
                }
                if (bb){
                    //std::cout<<"start output L: "<<*outL<<" R: "<<*outR<<std::endl;
                    //std::cout<<samplePosition<<std::endl;
                    bb = false;
                }
                if (looping && samplePosition >= s->getLoopEnd()-1){
                    //std::cout<<"end output L: "<<*outL<<" R: "<<*outR<<std::endl;
                }
                //std::cout<<*outL;
                
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
                    bb = true;
                    //std::cout<<std::endl<<"sample position: "<<samplePosition<<std::endl;
                    //std::cout<<std::endl<<"start over: "<<inL[(int)samplePosition]<<" "<<inL[(int)samplePosition - 1]<<" R: "<<inR[(int)samplePosition]<<" "<<inR[(int)samplePosition - 1]<<std::endl;
                    //return;
                }
            }
            
            
            volume = vol;
            tf_volume = tf_vol_multiplier;
            /*fftw_destroy_plan(pFFTW);
            fftw_destroy_plan(pFFTWR);
            fftw_destroy_plan(p2FFTW);
            fftw_destroy_plan(p2FFTWR);
            fftw_free(outFL);
            fftw_free(outFR);
            fftw_free(outFFTW);
            fftw_free(outFFTWR);
            fftw_free(inFL);
            fftw_free(inFR);*/
            /*if (groups_for_note[0] == 0){
                filter1.processSamples(outL, buffer.getNumSamples());
                std::cout<<"process left"<<std::endl;
                if (outR != nullptr){filter2.processSamples(outR, buffer.getNumSamples());}
                std::cout<<"process right"<<std::endl;
            }*/
        }
    }
}