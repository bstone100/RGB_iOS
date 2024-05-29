#ifdef __cplusplus
extern "C" {
#endif

void registerTranscriptionUpdateCallback(void (*callback)(const char*));
void setupAudioCapture();
void startAudioCapture();
void stopAudioCapture();

#ifdef __cplusplus
}
#endif
