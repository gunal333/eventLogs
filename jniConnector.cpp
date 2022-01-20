#include"jniConnector.h"
#include<iostream>
#include <stdio.h>
#include <windows.h>
#include<string.h>
#include <bits/stdc++.h>  
#include <winbase.h>
#include <iostream>  
#include<vector>
using namespace std;
typedef struct _JNI_POSREC {
    jclass cls;
    jmethodID constructortorID;
    jfieldID id;
    jfieldID logSource;
    jfieldID eventType;
    jfieldID eventTime;
    jfieldID eventSource;
    jfieldID eventData;
} JNI_POSREC;
JNI_POSREC * jniPosRec = NULL;
void LoadJniPosRec(JNIEnv * env) {
 
    if (jniPosRec != NULL)
        return;
 
    jniPosRec = new JNI_POSREC;
 
    jniPosRec->cls = env->FindClass("com/logRecordsFetcher/eventRecord");
 
    if(jniPosRec->cls != NULL)
        printf("sucessfully created class");
 
    jniPosRec->constructortorID = env->GetMethodID(jniPosRec->cls, "<init>", "()V");
    if(jniPosRec->constructortorID != NULL){
        printf("sucessfully created ctorID");
    }
 
    jniPosRec->id = env->GetFieldID(jniPosRec->cls, "id", "Ljava/lang/String;");
    jniPosRec->logSource = env->GetFieldID(jniPosRec->cls, "logSource", "Ljava/lang/String;");
    jniPosRec->eventType = env->GetFieldID(jniPosRec->cls, "eventType", "Ljava/lang/String;");
    jniPosRec->eventTime = env->GetFieldID(jniPosRec->cls, "eventTime", "Ljava/lang/String;");
    jniPosRec->eventSource = env->GetFieldID(jniPosRec->cls, "eventSource", "Ljava/lang/String;");
    jniPosRec->eventData = env->GetFieldID(jniPosRec->cls, "eventData", "Ljava/lang/String;");
}
struct SearchRecord {
    string id;
    string logSource;
    string eventType;
    string eventTime;
    string eventSource;
    string eventData;
};

int FillStudentRecordDetails(std::vector<SearchRecord*>* searchRecordResult,const char *Src, string &Result, FILE * pFile ){
  DWORD read_len, next_len;

char Buffer[256], Data[4096], *pchar;

HANDLE Handle = OpenEventLog(NULL, Src);

if (Handle==NULL)

{

CloseHandle(Handle);

return -1;

}
int count=0;
while(ReadEventLog(Handle, EVENTLOG_FORWARDS_READ | EVENTLOG_SEQUENTIAL_READ,1, (EVENTLOGRECORD*)Data, sizeof(Data), &read_len, &next_len))

{

for(short i=0; i<read_len;)

{
count++;
EVENTLOGRECORD *ptr = (EVENTLOGRECORD*)(Data+i);
string eventType; 
switch(ptr->EventType) 

{

case EVENTLOG_SUCCESS:

pchar= " Success ";
eventType = "Success";
break;

case EVENTLOG_ERROR_TYPE:

pchar= " error ";
eventType= " error ";

break;

case EVENTLOG_WARNING_TYPE:

pchar= " Warning ";
eventType= "Warning";
break;

case EVENTLOG_INFORMATION_TYPE:

pchar= " Information ";
eventType= " Information";
break;

case EVENTLOG_AUDIT_SUCCESS:

pchar= " Audit successful ";
eventType= "Audit successful ";
break;

case EVENTLOG_AUDIT_FAILURE:

pchar= " Audit failure ";
eventType= " Audit failure ";
break;

default:

continue;

}

int eventId = (short)ptr->EventID;
sprintf(Buffer, " Event\t%u\n", (short)ptr->EventID);

Result += Buffer;

sprintf(Buffer, " Type \t%s\n", pchar);

Result += Buffer;

/*tm *ptm = localtime((const long *)&ptr->TimeWritten);
ostringstream oss;
oss << put_time(ptm, "%d-%m-%Y %H-%M-%S");*/
/*const long int *timeseconds = (const long *)&ptr->TimeWritten;
time_t* timeGMT = (time_t*)(const long *)timeseconds;
cout<<timeGMT;
tm tm = *localtime(timeGMT);
    wstringstream wss;
    wss << put_time(&tm, L"%F %T");
    wstring etime = wss.str();
    wcout<<etime<<endl;
  =(long*)&ptr->TimeWritten;
  time_t* t = pts_sec;
std::string msg;
msg=ctime(pts_sec);
cout<<msg<<endl;*/
string eventTime= to_string((const long )ptr->TimeWritten);
/*tm *ptm = localtime((const long *)&ptr->TimeWritten);

sprintf(Buffer, " Time\t%.4hd-%.2hd-%.2hd %.2hd:%.2hd:%.2hd\n",

ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday,

ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

Result += Buffer;*/

pchar = Data + sizeof(EVENTLOGRECORD);//pchar points toSourceName[]
string eventSource = pchar;
sprintf(Buffer, " source \t%s\n", pchar); 

Result += Buffer;

pchar += strlen(pchar) + 1;//pchar points toComputerName[]

string computerName = pchar;
sprintf(Buffer, " computer \t%s\n", pchar); 

Result += Buffer;

pchar += strlen(pchar) + 1;//pchar points to UserSid

if(ptr->UserSidLength>0)

{

char Name[64];

DWORD Length = sizeof(SID), Length1 = sizeof(Buffer);

SID_NAME_USE Type = SidTypeUser;

SID *sid = (SID *)(Data + ptr->UserSidOffset);

if(LookupAccountSid(NULL, sid, Name, &Length, Buffer, &Length1, &Type))
sprintf(Buffer, " User /t%s/n", Name);
Result+=Buffer;

}
string eventData="";
if(ptr->DataOffset > ptr->StringOffset)
{

Result += "[ Description]\n";

pchar = Data + i + ptr->StringOffset;
for(short j = 0; j < ptr->NumStrings; j++)

{

Result += pchar;
eventData += pchar;
if(j < ptr->NumStrings-1)

Result += ' ';

pchar += strlen(pchar) + 1;
}

Result += '\n';

//Result+="[ data]/n";

}

Result+='\n';

i+=ptr->Length;

stringstream ss;  
  ss<<eventId;  
string s;  
  ss>>s;
    SearchRecord *pRecord1 = new SearchRecord();
    pRecord1->id =s;
    pRecord1->logSource = Src;
    pRecord1->eventType = eventType;
    pRecord1->eventTime = eventTime;
    pRecord1->eventSource = eventSource;
    pRecord1->eventData = eventData;
    searchRecordResult->push_back(pRecord1);

/*cout<<count<<". Event id: "<<eventId<<endl;
cout<<"Event logs source: "<<Src<<endl;
cout<<"Event type: "<<eventType<<endl;
cout<<"Event time: "<<eventTime<<endl;
cout<<"Event source: "<<eventSource<<endl;
cout<<"Event data: "<<eventData<<endl;
cout<<endl;*/
}

}

fwrite(Result.c_str(),Result.length(),1,pFile);

CloseEventLog(Handle);
/*cout<<"Total count:"<<count;*/
return 0;
}
void FillStudentRecValuesToJni(JNIEnv * env, jobject jPosRec, SearchRecord* cPosRec) 
{ 
    env->SetObjectField(jPosRec, jniPosRec->id, env->NewStringUTF(cPosRec->id.c_str()));
    env->SetObjectField(jPosRec, jniPosRec->logSource, env->NewStringUTF(cPosRec->logSource.c_str()));
    env->SetObjectField(jPosRec, jniPosRec->eventType, env->NewStringUTF(cPosRec->eventType.c_str()));
    env->SetObjectField(jPosRec, jniPosRec->eventTime, env->NewStringUTF(cPosRec->eventTime.c_str()));
    env->SetObjectField(jPosRec, jniPosRec->eventSource, env->NewStringUTF(cPosRec->eventSource.c_str()));
    env->SetObjectField(jPosRec, jniPosRec->eventData, env->NewStringUTF(cPosRec->eventData.c_str()));
}
JNIEXPORT jobjectArray JNICALL Java_com_logRecordsFetcher_jniConnector_getEventRecords
  (JNIEnv *env, jclass cls,jstring sourceName) {
    const char* sName = env->GetStringUTFChars(sourceName, NULL);
string result;
FILE *pFile;
pFile=fopen("EventLog.txt","w");
jniPosRec = NULL;
    LoadJniPosRec(env);
    std::vector<SearchRecord*> searchRecordResult ;
    FillStudentRecordDetails(&searchRecordResult,sName,result,pFile);
    printf("\nsearchRecordResult size is"+searchRecordResult.size());
jobjectArray jPosRecArray = env->NewObjectArray(searchRecordResult.size(), jniPosRec->cls, NULL);
for (size_t i = 0; i < searchRecordResult.size(); i++) {
        jobject jPosRec = env->NewObject(jniPosRec->cls, jniPosRec->constructortorID);
        FillStudentRecValuesToJni(env, jPosRec, searchRecordResult[i]);
        env->SetObjectArrayElement(jPosRecArray, i, jPosRec);
    }
fclose(pFile);
std::cout << "Working" << std::endl;
return jPosRecArray;
}