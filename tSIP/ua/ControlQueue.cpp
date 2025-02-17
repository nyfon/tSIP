//---------------------------------------------------------------------------


#pragma hdrstop

#include "ControlQueue.h"
#include "common/Mutex.h"
#include "common/ScopedLock.h"
#include "common/StaticCheck.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

#include "common/fifo.h"
#include "Command.h"

#define ARRAY_SIZE(a)    (sizeof(a)/sizeof(a[0]))

namespace
{
	Fifo<Command, 1024> fifo;
	Mutex mutex;
}

ControlQueue::ControlQueue()
{

}

int ControlQueue::GetCommand(Command& cmd)
{
	ScopedLock<Mutex> lock(mutex);
	Command *tmpcmd = fifo.getReadable();
	if (tmpcmd == NULL)
	{
		return 1;
	}
	cmd = *tmpcmd;
	fifo.pop();
	return 0;
}

void ControlQueue::ReRegister(int accountId)
{
	ScopedLock<Mutex> lock(mutex);
	Command *cmd = fifo.getWriteable();
	if (!cmd)
		return;
	cmd->type = Command::REREGISTER;
	cmd->accountId = accountId;
	fifo.push();
}

void ControlQueue::UnRegister(int accountId)
{
	ScopedLock<Mutex> lock(mutex);
	Command *cmd = fifo.getWriteable();
	if (!cmd)
		return;
	cmd->type = Command::UNREGISTER;
	cmd->accountId = accountId;
	fifo.push();
}

void ControlQueue::Call(int accountId, unsigned int callUid, AnsiString target, AnsiString extraHeaderLines, bool video, void *vidispParentHandle)
{
	ScopedLock<Mutex> lock(mutex);
	Command *cmd = fifo.getWriteable();
	if (!cmd)
		return;
	cmd->type = Command::CALL;
	cmd->accountId = accountId;
	cmd->callUid = callUid;
	cmd->target = target;
	if (extraHeaderLines != "")
	{
		if (extraHeaderLines.Length() < 2)
		{
			extraHeaderLines += "\r\n";
		}
		else
		{
			AnsiString end = extraHeaderLines.SubString(extraHeaderLines.Length() - 1, 2);
			if (end != "\r\n")
			{
        		extraHeaderLines += "\r\n";
			}
        }
    }
	cmd->extraHeaderLines = extraHeaderLines;
	cmd->video = video;
	cmd->vidispParentHandle = vidispParentHandle;
	fifo.push();
}

void ControlQueue::Answer(unsigned int callUid, AnsiString audioRxMod, AnsiString audioRxDev, bool video, void *vidispParentHandle)
{
	ScopedLock<Mutex> lock(mutex);
	Command *cmd = fifo.getWriteable();
	if (!cmd)
		return;
	cmd->type = Command::ANSWER;
	cmd->audioMod = audioRxMod;
	cmd->audioDev = audioRxDev;
	cmd->callUid = callUid;
	cmd->video = video;
	cmd->vidispParentHandle = vidispParentHandle;
	fifo.push();
}

void ControlQueue::Transfer(unsigned int callUid, AnsiString target)
{
	ScopedLock<Mutex> lock(mutex);
	Command *cmd = fifo.getWriteable();
	if (!cmd)
		return;
	cmd->type = Command::TRANSFER;
	cmd->callUid = callUid;
	cmd->target = target;
	fifo.push();
}

void ControlQueue::TransferReplace(unsigned int callUid, unsigned int callReplaceUid)
{
	ScopedLock<Mutex> lock(mutex);
	Command *cmd = fifo.getWriteable();
	if (!cmd)
		return;
	cmd->type = Command::TRANSFER_REPLACE;
	cmd->callUid = callUid;
	cmd->callReplaceUid = callReplaceUid;
	fifo.push();
}

void ControlQueue::CallStartAudioExtraSource(unsigned int callUid)
{
	ScopedLock<Mutex> lock(mutex);
	Command *cmd = fifo.getWriteable();
	if (!cmd)
		return;
	cmd->type = Command::CALL_START_AUDIO_EXTRA_SOURCE;
	cmd->callUid = callUid;
	fifo.push();
}

void ControlQueue::SendDigit(unsigned int callUid, char key)
{
	ScopedLock<Mutex> lock(mutex);
	Command *cmd = fifo.getWriteable();
	if (!cmd)
		return;
	cmd->type = Command::SEND_DIGIT;
	cmd->callUid = callUid;
	cmd->key = key;
	fifo.push();
}

void ControlQueue::GenerateTone(unsigned int callUid,
	float amplitude1, float frequency1,
	float amplitude2, float frequency2,
	float amplitude3, float frequency3,
	float amplitude4, float frequency4)
{
	ScopedLock<Mutex> lock(mutex);
	Command *cmd = fifo.getWriteable();
	if (!cmd)
		return;
	cmd->type = Command::GENERATE_TONES;
	cmd->callUid = callUid;
	STATIC_CHECK(ARRAY_SIZE(cmd->tones) == 4, TonesSizeMismatch);
	cmd->tones[0].amplitude = amplitude1;
	cmd->tones[0].frequency = frequency1;
	cmd->tones[1].amplitude = amplitude2;
	cmd->tones[1].frequency = frequency2;
	cmd->tones[2].amplitude = amplitude3;
	cmd->tones[2].frequency = frequency3;
	cmd->tones[3].amplitude = amplitude4;
	cmd->tones[3].frequency = frequency4;
	fifo.push();
}

void ControlQueue::Hold(unsigned int callUid, bool state)
{
	ScopedLock<Mutex> lock(mutex);
	Command *cmd = fifo.getWriteable();
	if (!cmd)
		return;
	cmd->type = Command::HOLD;
	cmd->callUid = callUid;
	cmd->bEnabled = state;
	fifo.push();
}

void ControlQueue::Mute(unsigned int callUid, bool state)
{
	ScopedLock<Mutex> lock(mutex);
	Command *cmd = fifo.getWriteable();
	if (!cmd)
		return;
	cmd->type = Command::MUTE;
	cmd->callUid = callUid;
	cmd->bEnabled = state;
	fifo.push();
}

void ControlQueue::SetMsgLogging(bool enabled, bool onlyFirstLines)
{
	ScopedLock<Mutex> lock(mutex);
	Command *cmd = fifo.getWriteable();
	if (!cmd)
		return;
	cmd->type = Command::SET_MSG_LOGGING;
	cmd->bEnabled = enabled;
	cmd->bParam = onlyFirstLines;
	fifo.push();
}

void ControlQueue::SetAubufLogging(bool enabled)
{
	ScopedLock<Mutex> lock(mutex);
	Command *cmd = fifo.getWriteable();
	if (!cmd)
		return;
	cmd->type = Command::SET_AUBUF_LOGGING;
	cmd->bEnabled = enabled;
	fifo.push();
}

void ControlQueue::Hangup(unsigned int callUid, int code, AnsiString reason)
{
	ScopedLock<Mutex> lock(mutex);
	Command *cmd = fifo.getWriteable();
	if (!cmd)
		return;
	cmd->type = Command::HANGUP;
	cmd->callUid = callUid;
	cmd->code = code;
	cmd->reason = reason;
	fifo.push();
}

void ControlQueue::StartRing(AnsiString wavFile)
{
	ScopedLock<Mutex> lock(mutex);
	Command *cmd = fifo.getWriteable();
	if (!cmd)
		return;
	cmd->type = Command::START_RING;
	cmd->target = wavFile;
	fifo.push();
}

void ControlQueue::PlayStop(void)
{
	ScopedLock<Mutex> lock(mutex);
	Command *cmd = fifo.getWriteable();
	if (!cmd)
		return;
	cmd->type = Command::PLAY_STOP;
	fifo.push();
}

void ControlQueue::StartRing2(AnsiString wavFile)
{
	ScopedLock<Mutex> lock(mutex);
	Command *cmd = fifo.getWriteable();
	if (!cmd)
		return;
	cmd->type = Command::START_RING2;
	cmd->target = wavFile;
	fifo.push();
}

void ControlQueue::Record(unsigned int callUid, AnsiString wavFile, unsigned int channels, unsigned int side, unsigned int fileFormat, unsigned int bitrate)
{
	ScopedLock<Mutex> lock(mutex);
	Command *cmd = fifo.getWriteable();
	if (!cmd)
		return;
	cmd->type = Command::RECORD;
	cmd->callUid = callUid;
	cmd->channels = channels;
	cmd->recSide = side;
	cmd->recFileFormat = fileFormat;
	cmd->bitrate = bitrate;
	cmd->target = wavFile;
	fifo.push();
}

void ControlQueue::RecordPause(unsigned int callUid)
{
	ScopedLock<Mutex> lock(mutex);
	Command *cmd = fifo.getWriteable();
	if (!cmd)
		return;
	cmd->type = Command::RECORD_PAUSE;
	cmd->callUid = callUid;
	fifo.push();
}

void ControlQueue::PagingTx(AnsiString target, AnsiString pagingTxWaveFile, AnsiString codec, unsigned int ptime)
{
	ScopedLock<Mutex> lock(mutex);
	Command *cmd = fifo.getWriteable();
	if (!cmd)
		return;
	cmd->type = Command::PAGING_TX;
	cmd->target = target;
	cmd->pagingTxWaveFile = pagingTxWaveFile;
	cmd->pagingTxCodec = codec;
	cmd->pagingTxPtime = ptime; 
	fifo.push();
}

void ControlQueue::SwitchAudioSource(unsigned int callUid, AnsiString audioMod, AnsiString audioDev)
{
	ScopedLock<Mutex> lock(mutex);
	Command *cmd = fifo.getWriteable();
	if (!cmd)
		return;
	cmd->type = Command::SWITCH_AUDIO_SOURCE;
	cmd->audioMod = audioMod;
	cmd->audioDev = audioDev;
	cmd->callUid = callUid;
	fifo.push();
}

void ControlQueue::SwitchAudioPlayer(unsigned int callUid, AnsiString audioMod, AnsiString audioDev)
{
	ScopedLock<Mutex> lock(mutex);
	Command *cmd = fifo.getWriteable();
	if (!cmd)
		return;
	cmd->type = Command::SWITCH_AUDIO_PLAYER;
	cmd->audioMod = audioMod;
	cmd->audioDev = audioDev;
	cmd->callUid = callUid;
	fifo.push();
}

void ControlQueue::SwitchVideoSource(unsigned int callUid, AnsiString videoMod, AnsiString videoDev)
{
	ScopedLock<Mutex> lock(mutex);
	Command *cmd = fifo.getWriteable();
	if (!cmd)
		return;
	cmd->type = Command::SWITCH_VIDEO_SOURCE;
	cmd->videoMod = videoMod;
	cmd->videoDev = videoDev;
	cmd->callUid = callUid;
	fifo.push();
}

void ControlQueue::UpdateVolume(void)
{
	ScopedLock<Mutex> lock(mutex);
	Command *cmd = fifo.getWriteable();
	if (!cmd)
		return;
	cmd->type = Command::UPDATE_VOLUME;
	fifo.push();
}

int ControlQueue::SendCustomRequest(int requestId, AnsiString method, AnsiString target, AnsiString extraHeaderLines)
{
	ScopedLock<Mutex> lock(mutex);
	Command *cmd = fifo.getWriteable();
	if (!cmd)
		return -1;
	cmd->type = Command::SEND_CUSTOM_REQUEST;
	cmd->requestId = requestId;
	cmd->method = method;
	cmd->target = target;
	if (extraHeaderLines != "")
	{
		if (extraHeaderLines.Length() < 2)
		{
			extraHeaderLines += "\r\n";
		}
		else
		{
			AnsiString end = extraHeaderLines.SubString(extraHeaderLines.Length() - 1, 2);
			if (end != "\r\n")
			{
        		extraHeaderLines += "\r\n";
			}
        }
    }
	cmd->extraHeaderLines = extraHeaderLines;
	fifo.push();
	return 0;
}

int ControlQueue::SendMessage(int requestId, int accountId, AnsiString target, AnsiString text)
{
	ScopedLock<Mutex> lock(mutex);
	Command *cmd = fifo.getWriteable();
	if (!cmd)
		return -1;
	cmd->type = Command::SEND_MESSAGE;
	cmd->requestId = requestId;
	cmd->accountId = accountId;
	cmd->target = target;
	cmd->text = text;
	fifo.push();
	return 0;
}

int ControlQueue::ZrtpVerifySas(bool state)
{
	ScopedLock<Mutex> lock(mutex);
	Command *cmd = fifo.getWriteable();
	if (!cmd)
		return -1;
	cmd->type = Command::ZRTP_VERIFY_SAS;
	cmd->bParam = state;
	fifo.push();
	return 0;
}

int ControlQueue::ConferenceStart(void)
{
	ScopedLock<Mutex> lock(mutex);
	Command *cmd = fifo.getWriteable();
	if (!cmd)
		return -1;
	cmd->type = Command::CONFERENCE_START;
	fifo.push();
	return 0;
}





