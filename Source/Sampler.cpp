/*
  ==============================================================================

    Sampler.cpp
    Created: 28 Jun 2014 2:03:28pm
    Author:  patrick

  ==============================================================================
*/

#include "Sampler.h"
#include "Adsr.h"


Sampler::Sampler() : AudioSource(), synth(), formatManager(), filter1(), filter2(), fx_selector(nullptr) {
    for (int i=0; i<256; i++){
        synth.addVoice(new SampleVoice());
    }
    formatManager.registerBasicFormats();
    filter1.setCoefficients(IIRCoefficients::makeLowPass(44100.0, 7000.0));
    filter2.setCoefficients(IIRCoefficients::makeLowPass(44100.0, 7000.0));
    
    //filter1.setCoefficients(IIRCoefficients());
    //filter2.setCoefficients(IIRCoefficients());
}
    
void Sampler::addSample(String path, int root_note, int note_low, int note_high, Array<int>& groups){
    ScopedPointer<AudioFormatReader> audioReader(formatManager.createReaderFor(File(path)));
        
    BigInteger allNotes;
    for (int i=note_low; i<note_high; i++){
        allNotes.setBit(i);
    }
        
    synth.addSound(new SampleSound("demo sound", *audioReader,
                                    allNotes, root_note,
                                    0.0, 0.0, 10.0, groups, fx_selector));
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
    filter1.processSamples(bufferToFill.buffer->getWritePointer(0), bufferToFill.buffer->getNumSamples());
    filter2.processSamples(bufferToFill.buffer->getWritePointer(1), bufferToFill.buffer->getNumSamples());
}

SampleVoice::SampleVoice() : SamplerVoice(), /*filter1(), filter2(),*/ samplePosition(0.0f),
                             attackTime(0.1), releaseTime(0.1){
    //filter1.setCoefficients(IIRCoefficients::makeLowPass(44100.0, 500.0));
    //filter2.setCoefficients(IIRCoefficients::makeLowPass(44100.0, 500.0));
}

void SampleVoice::startNote(const int midiNoteNumber,
                            const float velocity,
                            SynthesiserSound* s,
                            const int pitchWheelPosition){
    if (SampleSound* sound = (SampleSound*)s){
        const double a = pow(2.0, 1.0/12.0);
        double old_frequency = 440.0 * pow(a, (double)sound->getRootNote() - 69.0);
        double new_frequency = old_frequency * pow(a, (float)midiNoteNumber - sound->getRootNote());
        pitchRatio = new_frequency/old_frequency;
        releaseStart = 0.0f;
        
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
                    return;
                }
            }
        }
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
        Array<int> groups_for_note = s->getGroups();
        //example of how to not process a sound for a particular Group
        //if (groups_for_note[0] == 0){return;}
    
    
    //SamplerVoice::renderNextBlock(buffer, startSample, numSamples);
    
    //process filter if group 0
    /*if (s != nullptr){
        Array<int> groups_for_note = s->getGroups();
        if (groups_for_note[0] == 0){
            filter1.processSamples(buffer.getWritePointer(0), buffer.getNumSamples());
            filter2.processSamples(buffer.getWritePointer(1), buffer.getNumSamples());
        }
    }*/
        float gain = 1.0f;
        //std::cout<<getCurrentlyPlayingNote()<<" "<<isKeyDown()<<std::endl;
        double sample_length = s->getAudioData()->getNumSamples();
        int num_channels = s->getAudioData()->getNumChannels();
        if (getCurrentlyPlayingSound().get()){
            const float* const inL = s->getAudioData()->getReadPointer(0);
            const float* const inR = num_channels>1 ? s->getAudioData()->getReadPointer(1) : nullptr;
            float* outL = buffer.getWritePointer(0, startSample);
            float* outR = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1, startSample) : nullptr;
    
            if (!isKeyDown() && releaseStart == 0.0f){
                releaseStart = samplePosition;
            }
            double release_sample_length = releaseTime/1000*s->getSampleRate();
            float release_x = 0.0f;
            int start = samplePosition;
            for (int i= start; i<numSamples+start; i++){
                float x = samplePosition/s->getSampleRate()*1000;
                double attack_multiplier = x<attackTime ? getAttackMultiplier(attackTime, attackCurve, x) : 1.0;                      
                                           
                double samples_left = sample_length - samplePosition;
                
                //float release_x = noteDown ? 0.0 : (samplePosition - releaseStart)/s->getSampleRate()*1000;
                if (samples_left < release_sample_length && releaseStart == 0.0){
                    releaseStart = samplePosition;
                }
                if (releaseStart != 0.0){
                    release_x = (samplePosition - releaseStart)/s->getSampleRate()*1000;
                }
                
                double release_multiplier = !isKeyDown() || samples_left < release_sample_length ? getReleaseMultiplier(releaseTime, releaseCurve, release_x) : 1.0;
            
                const int pos = (int) samplePosition;
                const float alpha = (float) (samplePosition - pos);
                const float invAlpha = 1.0f - alpha;
                
                // just using a very simple linear interpolation here..
                float l = (inL [pos] * invAlpha + inL [pos + 1] * alpha);
                float r = inR != nullptr ? inR[pos] * invAlpha + inR[pos + 1] * alpha : l;

                *outL += l*attack_multiplier*release_multiplier;
                *outR += r*attack_multiplier*release_multiplier;
                outL++;
                outR++;
            
                samplePosition += pitchRatio;
                if (samplePosition > sample_length || sample_length - samplePosition < numSamples || release_x >= releaseTime){
                    samplePosition = 0.0;
                    stopNote(false);
                    break;
                }
            }
        }
    }
}