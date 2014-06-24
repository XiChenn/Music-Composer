#ifndef MUSICCOMPOSER_H
#define MUSICCOMPOSER_H

#include <vector>

#include "AudioSignal.h"
#include "NoteMaker.h"

class MusicComposer {
 private:
    /**
       An overall AudioSignal (called music in this document) that indicates the music generated so far by this class.
     */
    AudioSignal music;

    /**
       A NoteMaker object to convert note strings (in the form: C#, C4, C#1 etc.) to AudioSignal objects
     */
    NoteMaker nm;

    /**
       A vector of AudioSignal objects that represents various tracks currently being operated upon by the user. By 
       default the first track is set as the track to which audio signals are appended by the MusicComposer::operator() 
     */
    std::vector<AudioSignal> musicTrack;

    /**
       To track the current track
     */
    int currentTrack = 0;

 public:

    /**
       Default constructor.
     */
    MusicComposer();

    /**
       This method must add all of the current tracks into a single AudioSignal and append it to the overall audio signal. 
       All the current tracks are cleared / deleted. A new track-1 is started as the only track.  This method return a r
       eference to the overall audio signal.
     */
    const AudioSignal& getMusic();

    /**
       This method must suitably process ent as described below, append the resulting audio signal (if any) to the current 
       audio track and returns the current audio track back. Note that an audio track is simply an AudioSignal object
     */
    AudioSignal& operator()(const std::string& ent);

};

#endif
