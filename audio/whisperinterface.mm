#include "whisperinterface.h"
#include <dispatch/dispatch.h>
#include "../whisper.cpp/whisper.h"
#include "QDebug"


#import <UIKit/UIKit.h>

#import <AVFoundation/AVFoundation.h>
#import <AudioToolbox/AudioQueue.h>

#define NUM_BUFFERS 3
#define MAX_AUDIO_SEC 30
#define SAMPLE_RATE 16000

#define NUM_BYTES_PER_BUFFER 16*1024

struct StateInp {
    int ggwaveId;
    bool isCapturing;
    bool isTranscribing;
    bool isRealtime;

    AudioQueueRef queue;
    AudioStreamBasicDescription dataFormat;
    AudioQueueBufferRef buffers[NUM_BUFFERS];

    int n_samples;
    int16_t * audioBufferI16;
    float   * audioBufferF32;

    struct whisper_context * ctx;

    // VAD
    float silenceThreshold;
    float silenceTimeOut; // in seconds
    float currentSilenceDuration;
};

static StateInp stateInp;

// Callback function declaration for handling audio input
static void AudioInputCallback(void *userData, AudioQueueRef queue, AudioQueueBufferRef buffer, const AudioTimeStamp *startTime, UInt32 numPacketDescriptions, const AudioStreamPacketDescription *packetDescs);

static void (*transcriptionUpdateCallback)(const char *) = NULL;

void registerTranscriptionUpdateCallback(void (*callback)(const char*)) {
    transcriptionUpdateCallback = callback;
}

void setupAudioCapture() {
    // whisper.cpp initialization
    // load the model
    NSString *modelPath = [[NSBundle mainBundle] pathForResource:@"ggml-base.en" ofType:@"bin"];

    // check if the model exists
    if (![[NSFileManager defaultManager] fileExistsAtPath:modelPath]) {
      NSLog(@"Model file not found");
      return;
    }

    NSLog(@"Loading model from %@", modelPath);

    // create ggml context

    struct whisper_context_params cparams = whisper_context_default_params();
    #if TARGET_OS_SIMULATOR
    cparams.use_gpu = false;
    NSLog(@"Running on simulator, using CPU");
    #endif
    stateInp.ctx = whisper_init_from_file_with_params([modelPath UTF8String], cparams);

    // check if the model was loaded successfully
    if (stateInp.ctx == NULL) {
      NSLog(@"Failed to load model");
      return;
    }

    // initialize audio format and buffers

    stateInp.dataFormat.mSampleRate       = WHISPER_SAMPLE_RATE;
    stateInp.dataFormat.mFormatID         = kAudioFormatLinearPCM;
    stateInp.dataFormat.mFramesPerPacket  = 1;
    stateInp.dataFormat.mChannelsPerFrame = 1;
    stateInp.dataFormat.mBytesPerFrame    = 2;
    stateInp.dataFormat.mBytesPerPacket   = 2;
    stateInp.dataFormat.mBitsPerChannel   = 16;
    stateInp.dataFormat.mReserved         = 0;
    stateInp.dataFormat.mFormatFlags      = kLinearPCMFormatFlagIsSignedInteger;


    stateInp.n_samples = 0;
    stateInp.audioBufferI16 = (int16_t *)malloc(MAX_AUDIO_SEC*SAMPLE_RATE*sizeof(int16_t));
    stateInp.audioBufferF32 = (float *)malloc(MAX_AUDIO_SEC*SAMPLE_RATE*sizeof(float));

    stateInp.isTranscribing = false;
    stateInp.isRealtime = true;

    stateInp.silenceThreshold = 1000.0; // Energy level that defines silence
    stateInp.silenceTimeOut = 2.0; // 2 seconds of silence before stopping
    stateInp.currentSilenceDuration = 0.0;
}

void startAudioCapture() {
    // initiate audio capturing
    NSLog(@"Start capturing");

    stateInp.n_samples = 0;

    OSStatus status = AudioQueueNewInput(&stateInp.dataFormat,
                                         AudioInputCallback,
                                         &stateInp,
                                         CFRunLoopGetCurrent(),
                                         kCFRunLoopCommonModes,
                                         0,
                                         &stateInp.queue);

    if (status == 0) {
        for (int i = 0; i < NUM_BUFFERS; i++) {
            AudioQueueAllocateBuffer(stateInp.queue, NUM_BYTES_PER_BUFFER, &stateInp.buffers[i]);
            AudioQueueEnqueueBuffer (stateInp.queue, stateInp.buffers[i], 0, NULL);
        }

        stateInp.isCapturing = true;
        status = AudioQueueStart(stateInp.queue, NULL);
        if (status == 0) {
            // we're capturing
qDebug() << "capturing audio";
        }
    }

    if (status != 0) {
        stopAudioCapture();
    }
}

void stopAudioCapture() {
    NSLog(@"Stop capturing");

    stateInp.isCapturing = false;

    AudioQueueStop(stateInp.queue, true);
    for (int i = 0; i < NUM_BUFFERS; i++) {
        AudioQueueFreeBuffer(stateInp.queue, stateInp.buffers[i]);
    }

    AudioQueueDispose(stateInp.queue, true);
}







void onTranscribe() {
    if (stateInp.isTranscribing) {
        return;
    }

    NSLog(@"Processing %d samples", stateInp.n_samples);

    stateInp.isTranscribing = true;

    // dispatch the model to a background thread
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        // process captured audio
        // convert I16 to F32
        for (int i = 0; i < stateInp.n_samples; i++) {
            stateInp.audioBufferF32[i] = (float)stateInp.audioBufferI16[i] / 32768.0f;
        }

        // run the model
        struct whisper_full_params params = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);

        // get maximum number of threads on this device (max 8)
        const int max_threads = MIN(8, (int)[[NSProcessInfo processInfo] processorCount]);

        params.print_realtime   = true;
        params.print_progress   = false;
        params.print_timestamps = true;
        params.print_special    = false;
        params.translate        = false;
        params.language         = "en";
        params.n_threads        = max_threads;
        params.offset_ms        = 0;
        params.no_context       = true;
        params.single_segment   = stateInp.isRealtime;
        params.no_timestamps    = params.single_segment;

        CFTimeInterval startTime = CACurrentMediaTime();

        whisper_reset_timings(stateInp.ctx);

        if (whisper_full(stateInp.ctx, params, stateInp.audioBufferF32, stateInp.n_samples) != 0) {
            NSLog(@"Failed to run the model");
            return;
        }

        whisper_print_timings(stateInp.ctx);

        CFTimeInterval endTime = CACurrentMediaTime();

        NSLog(@"\nProcessing time: %5.3f, on %d threads", endTime - startTime, params.n_threads);

        // result text
        NSString *result = @"";

        int n_segments = whisper_full_n_segments(stateInp.ctx);
        for (int i = 0; i < n_segments; i++) {
            const char * text_cur = whisper_full_get_segment_text(stateInp.ctx, i);

            // append the text to the result
            result = [result stringByAppendingString:[NSString stringWithUTF8String:text_cur]];
        }

        const float tRecording = (float)stateInp.n_samples / (float)stateInp.dataFormat.mSampleRate;

        // append processing time
        result = [result stringByAppendingString:[NSString stringWithFormat:@"\n\n[recording time:  %5.3f s]", tRecording]];
        result = [result stringByAppendingString:[NSString stringWithFormat:@"  \n[processing time: %5.3f s]", endTime - startTime]];

        // dispatch the result to the main thread
        dispatch_async(dispatch_get_main_queue(), ^{
            stateInp.isTranscribing = false;
            transcriptionUpdateCallback(result.UTF8String);
        });
    });
}



void AudioInputCallback(void * inUserData,
                        AudioQueueRef inAQ,
                        AudioQueueBufferRef inBuffer,
                        const AudioTimeStamp * inStartTime,
                        UInt32 inNumberPacketDescriptions,
                        const AudioStreamPacketDescription * inPacketDescs)
{
    StateInp * stateInp = (StateInp*)inUserData;

    if (!stateInp->isCapturing) {
        NSLog(@"Not capturing, ignoring audio");
        return;
    }

    const int n = inBuffer->mAudioDataByteSize / 2;

    NSLog(@"Captured %d new samples", n);

    if (stateInp->n_samples + n > MAX_AUDIO_SEC*SAMPLE_RATE) {
        NSLog(@"Too much audio data, ignoring");

        dispatch_async(dispatch_get_main_queue(), ^{
            stopAudioCapture();
        });

        return;
    }

    float sumEnergy = 0;
    for (int i = 0; i < n; i++) {
        stateInp->audioBufferI16[stateInp->n_samples + i] = ((short*)inBuffer->mAudioData)[i];
        sumEnergy += pow(((short*)inBuffer->mAudioData)[i], 2);
    }

    float averageEnergy = sumEnergy / n;
    NSLog(@"Average energy: %f", averageEnergy);

    stateInp->n_samples += n;

    if (averageEnergy < stateInp->silenceThreshold) {
        stateInp->currentSilenceDuration += ((float)n / stateInp->dataFormat.mSampleRate);
    } else {
        stateInp->currentSilenceDuration = 0;
    }

    NSLog(@"Current silence duration: %f", stateInp->currentSilenceDuration);

    // Check if it's time to stop capturing
    if (stateInp->currentSilenceDuration >= stateInp->silenceTimeOut) {
        NSLog(@"Stopping due to silence.");

        dispatch_async(dispatch_get_main_queue(), ^{
            stopAudioCapture();
        });

        return;
    }

    // put the buffer back in the queue
    AudioQueueEnqueueBuffer(stateInp->queue, inBuffer, 0, NULL);

    if (stateInp->isRealtime) {
        // dipatch onTranscribe() to the main thread
        dispatch_async(dispatch_get_main_queue(), ^{
            onTranscribe();
        });
    }
}






