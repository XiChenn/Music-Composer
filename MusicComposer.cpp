#include <iostream>
#include <iterator>
#include <fstream>
#include <algorithm>
#include <string>

#include "MusicComposer.h"

AudioSignal loadWaveFile(std::string fileName);
AudioSignal addEcho(int i, const AudioSignal& music);
int shortestDuration(const std::vector<AudioSignal> musicTrack);
int longestDuration(const std::vector<AudioSignal> musicTrack);


MusicComposer::MusicComposer() {
    AudioSignal emptySignal;
    musicTrack.push_back(emptySignal);
}

const AudioSignal& MusicComposer::getMusic() {
    AudioSignal newMusic;
    std::for_each(musicTrack.begin(), musicTrack.end(), 
                  [&newMusic](AudioSignal musicInTrack){ newMusic = newMusic + musicInTrack; });
    music = music & newMusic;                                             // Concatenate the combined track to "music"
    musicTrack.clear();                                                   // Clear out all the tracks.
    musicTrack.resize(1);
    return music;
}

AudioSignal& MusicComposer::operator()(const std::string& ent) {
    AudioSignal newMusic;
    std::string upEnt = ent;
    std::transform(ent.begin(), ent.end(), upEnt.begin(), toupper);        // Convert the string to uppercase
    if (ent.compare(0, 1, "*") != 0) {                                     // If the given ent string does not have a ‘*’ as the first character 
        newMusic = nm(upEnt);                                              // pass the string to the NoteMaker’s functor, obtain the AudioSignal      
        musicTrack[currentTrack] = musicTrack[currentTrack] & newMusic;    // append it to the current track
    } else {
	switch (ent[1]) {
        case 't': currentTrack = NoteMaker::toInt(ent.substr(2)) - 1; musicTrack.resize(NoteMaker::toInt(ent.substr(2))); break;
        case 'l': music = loadWaveFile(ent.substr(2)); break;
        case 'e': music = addEcho(NoteMaker::toInt(ent.substr(2)), music); break;
        case '~': musicTrack[NoteMaker::toInt(ent.substr(2)) - 1].changeDurationTo(shortestDuration(musicTrack)); break;
        case '=': musicTrack[NoteMaker::toInt(ent.substr(2)) - 1].changeDurationTo(longestDuration(musicTrack)); break;
        case '.': getMusic(); break;
	}
    }
        
    return music;
}

int main(int argc, char *argv[]) {
    MusicComposer mc;
    std::istream_iterator<std::string> noteStream(std::cin), eof;
    mc = std::for_each(noteStream, eof, mc);
    mc.getMusic().write(argv[1]);
    return 0;
}

AudioSignal loadWaveFile(std::string fileName) {
    RIFFReader waveFile(fileName);
    AudioSignal newMusic(waveFile);
    return newMusic;
}

AudioSignal addEcho(int i, const AudioSignal& music) {
    AudioSignal newMusic(music);                              // Copy the music
    newMusic = newMusic >> i;                                 // Shift to left by i
    newMusic = newMusic * (-1/4);                             // Decrease volume of copy to 1/4 of the track
    return newMusic;
}

int shortestDuration(const std::vector<AudioSignal> musicTrack) {
    int minDuration = 0;
    std::vector<int> musicDuration;
    std::for_each(musicTrack.begin(), musicTrack.end(), [&musicDuration](const AudioSignal& music){ musicDuration.push_back(music.duration()); });
    minDuration = *std::min_element(musicDuration.begin(), musicDuration.end());
    return minDuration;
}

int longestDuration(const std::vector<AudioSignal> musicTrack) {
    int maxDuration = 0;
    std::vector<int> musicDuration;
    std::for_each(musicTrack.begin(), musicTrack.end(), [&musicDuration](const AudioSignal& music){ musicDuration.push_back(music.duration()); });
    maxDuration = *std::max_element(musicDuration.begin(), musicDuration.end());
    return maxDuration;
}
