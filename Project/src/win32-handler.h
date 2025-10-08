#pragma once

// window icon related
#define APP_ICON 101

// buttons menu macros
#define IDM_RECORD_ACTIONS      201
#define IDM_STOP_RECORD_ACTIONS 202
#define IDM_PERFORM_ACTIONS     203


// linked list and recording related declarations
typedef struct ActionNode
{
    INPUT input;
    struct ActionNode *next;
} ActionNode;

// global variable declarations
extern BOOL gIsRecording;
extern ActionNode *gpHead;
extern ActionNode *gpTail;

// function declarations
void AddAction(INPUT in);
void PlayRecordedActions(void);
void FreeRecordedActions(void);
