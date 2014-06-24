//--------------------------------------------------------------------
//
// File Name: AudioSignal.cpp
// Author: Xi Chen
// Email Address: chenx19@miamioh.edu
// Description: AudioSinal class that provides a convenient interface to 
// manipulate audio signals. The AudioSignal class encapsulates 
// information about a digital audio signal that has been sampled using 
// Linear Pulse Code Modulation (LPCM). An audio signal is essentially a 
// series of Sample (alias for 16-bit integers defined in RIFF.h) entries.  
// Example: {0, 10, 15, 10, 0, -10, -15, -10, 0} where each value in the 
// list is of data type Sample.  In addition to the actual data, each audio 
// signal also contains the following metadata associated with the signal:
// sampleRate and bitsPerSample
//
// Last Changed: February 16, 2013
//
//---------------------------------------------------------------------

#include "AudioSignal.h"

#include "math.h"
#include <functional>
#include <algorithm>
#include <iterator>

AudioSignal::AudioSignal(const int sampleRate, const short bitsPerSample) { 
    this->sampleRate    = sampleRate;
    this->bitsPerSample = bitsPerSample;
}

AudioSignal::AudioSignal(RIFFReader& inputWaveFile) {
    sampleRate    = inputWaveFile.getSampleRate();
    bitsPerSample = inputWaveFile.getBitsPerSample();
    Sample data;
    while((inputWaveFile >> data)) {
	wave.push_back(data);
    }
}

AudioSignal::AudioSignal(const AudioSignal& src) {
    sampleRate    = src.sampleRate;
    bitsPerSample = src.bitsPerSample;
    wave          = src.wave;
}

AudioSignal::AudioSignal(AudioSignal&& src) {
    sampleRate    = std::move(src.sampleRate);
    bitsPerSample = std::move(src.bitsPerSample);
    wave          = std::move(src.wave);
}

AudioSignal& AudioSignal::operator=(const AudioSignal& src) {
    sampleRate    = src.sampleRate;
    bitsPerSample = src.bitsPerSample;
    wave          = src.wave;
    return *this;
}

AudioSignal::AudioSignal(const int freq, const int lenInMills, const int amplitude,
        const int sampleRate, const short bitsPerSample,
        const bool decay) {
    double PI            = 3.1415926;
    double numOfSineWave = freq * lenInMills / 1000;
    int numOfSample      = sampleRate * lenInMills / 1000;
    double period        = numOfSample / numOfSineWave;
    Sample data;
    int currentAmplitude = amplitude;
    for (int i = 0; i < numOfSample; i++) {
	if (decay) {
	    currentAmplitude = amplitude * exp((double) -i / numOfSample / 0.5); 
	}
	data = currentAmplitude * sin((double) (2 * PI * i / period));
	wave.push_back(data);
    }
    this->sampleRate     = sampleRate;
    this->bitsPerSample  = bitsPerSample;
}

AudioSignal AudioSignal::operator+(const AudioSignal& other) const {
    AudioSignal newAS(*this);
    std::vector<Sample> newVector(std::max(wave.size(), other.wave.size()));
    std::vector<Sample> temp(std::min(wave.size(), other.wave.size()));
    if (wave.size() < other.wave.size()) {
	std::copy(other.wave.begin(), other.wave.begin() + wave.size(), temp.begin());
	std::transform(wave.begin(), wave.end(), temp.begin(), newVector.begin(), std::plus<int>());
	std::copy(other.wave.begin() + wave.size(), other.wave.end(), newVector.begin() + wave.size());
    } else if(other.wave.size() < wave.size()) {
	std::copy(wave.begin(), wave.begin() + other.wave.size(), temp.begin());
	std::transform(other.wave.begin(), other.wave.end(), temp.begin(), newVector.begin(), std::plus<int>());
	std::copy(wave.begin() + other.wave.size(), wave.end(), newVector.begin() + other.wave.size());
    } else
	std::transform(other.wave.begin(), other.wave.end(), wave.begin(), newVector.begin(), std::plus<int>());
    newAS.wave = newVector;
    return newAS;
}

AudioSignal& AudioSignal::operator&(const AudioSignal& other) {
    std::for_each(other.wave.begin(), other.wave.end(), [this](const Sample& s){ wave.push_back(s); });
    return *this;
}

AudioSignal& AudioSignal::operator*(const double factor) {
    std::vector<double> factorList(wave.size(), factor);
    std::transform(wave.begin(), wave.end(), factorList.begin(), wave.begin(), std::multiplies<int>());
    return *this;
}

AudioSignal AudioSignal::operator>>(long timeInMillis) const {
    AudioSignal newAS(*this);
    int numOfSampleToInsert = sampleRate * timeInMillis / 1000;
    newAS.wave.insert(newAS.wave.begin(), numOfSampleToInsert, 0);
    return newAS;
}

AudioSignal AudioSignal::operator<<(long timeInMillis) const {
    AudioSignal newAS(*this);
    int numOfSampleToErase = sampleRate * timeInMillis / 1000;
    newAS.wave.erase(newAS.wave.begin(), newAS.wave.begin() + numOfSampleToErase);
    return newAS;
}

long AudioSignal::duration() const {
    return 1000 * wave.size() / sampleRate;
}

void AudioSignal::write(const std::string& fileName) const throw (std::exception) {
    RIFFWriter wavFile(fileName, sampleRate, bitsPerSample);
    for_each(wave.begin(), wave.end(), [&wavFile](const Sample s){ wavFile << s; });
}

void AudioSignal::changeDurationTo(const int timeInMillis) {
    int originalTime = wave.size() * 1000 / sampleRate;
    int numOfSampleToChange = 0;
    if (originalTime < timeInMillis) {
	numOfSampleToChange = (timeInMillis - originalTime) * sampleRate / 1000;
	wave.insert(wave.begin(), wave.begin(), wave.begin() + numOfSampleToChange);
    } else if (originalTime > timeInMillis) {
	numOfSampleToChange = (originalTime - timeInMillis) * sampleRate / 1000;
	wave.erase(wave.end() - numOfSampleToChange, wave.end());
    }
}
/*
void AudioSignal::print() {
    std::cout << "sampleRate: " << sampleRate << std::endl << "bitsPerSample: "
	       << bitsPerSample << std::endl;
    std::copy(wave.begin(), wave.end(), std::ostream_iterator<Sample>(std::cout, " "));
     std::cout << std::endl;
}
*/
