#define RECORD_FINISHED 1
#define RECORD_CONTINUE 0


/* Initialize some frame buffers for recording. Call this function before the main loop. */
void InitiatlizeRecordBuffer();

/* Record currect frame to file. Call this function is each main loop.
Return value: 1, the record procedure is finished. 0, the record procedure should continue.
*/
int RecordFrame();

/* Clean up frame buffers. Call this before the program exits. */
void CleanUpRecord();