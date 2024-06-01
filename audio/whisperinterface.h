#ifdef __cplusplus
extern "C" {
#endif

void registerTranscriptionUpdatedSignal(void (*callback)(const char*));
void registerSilenceDetectedSignal(void (*func)(void));
void registerLevelCalculatedSignal(void (*func)(const float));
void registerTimeLimitReachedSignal(void (*func)(void));

float getCurrentLevel();
void setupAudioCapture();
void startAudioCapture();
void stopAudioCapture();

#ifdef __cplusplus
}
#endif
