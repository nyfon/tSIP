//---------------------------------------------------------------------------

#ifndef CallbackH
#define CallbackH
//---------------------------------------------------------------------------

#include <System.hpp>
#include <vector>
#include "baresip_dialog_info_status.h"

class Callback
{
public:
	enum Type
	{
		CALL_STATE = 0,
		CALL_REINVITE_RECEIVED,
		CALL_DTMF_STATE,
		REG_STATE,
		APP_STATE,
		DLG_INFO_STATE,
		MWI_STATE,
		PRESENCE_STATE,
		PAGING_TX_STATE,
		EVENT_TALK,
		AUDIO_CODEC_LIST,			///< audio codec list sent after static and dynamic modules are loaded
		SET_CALL_DATA,
		AUDIO_ERROR,
		CUSTOM_REQUEST_STATUS,
		SIMPLE_MESSAGE,
		SIMPLE_MESSAGE_STATUS,		///< status for sent MESSAGE; using requestUid, requestError, scode
		RECORDER_STATE,
		ENCRYPTION_STATE,

		TYPE__LIMITER
	} type;

	enum ua_state_e
	{
		CALL_STATE_CLOSED = 0,
		CALL_STATE_INCOMING,
		CALL_STATE_OUTGOING,
		CALL_STATE_TRYING,
		CALL_STATE_RINGING,
		CALL_STATE_PROGRESS,
		CALL_STATE_ESTABLISHED,
		CALL_STATE_TRANSFER,			///< REFER request received
		CALL_STATE_TRANSFER_OOD,			///< REFER outside of dialog received

		CALL_STATE__LIMITER
	} state;

	static const char* GetCallStateName(enum ua_state_e state);
	static const char* GetCallStateDescription(enum ua_state_e state);

	enum reg_state_e
	{
		REG_STATE_UNREGISTER_OK = 0,
		REG_STATE_REGISTERING,
		REG_STATE_REGISTER_OK,
		REG_STATE_REGISTER_FAIL,
		REG_STATE_UNREGISTERING,
		REG_STATE_UNREGISTER_FAIL,

		REG_STATE__LIMITER
	} reg_state;
	AnsiString prm;

	enum paging_tx_state_e
	{
		PAGING_TX_ENDED = 0,
		PAGING_TX_STARTED,

		PAGING_TX__LIMITER
	} paging_tx_state;

	enum app_state_e
	{
		APP_STATE_UNKNOWN = 0,
		APP_STARTED,
		APP_INIT_FAILED,
		APP_START_FAILED,

		APP_STATE__LIMITER
	} app_state;

	enum rec_state_e
	{
		RECORDER_STATE_IDLE = 0,
		RECORDER_STATE_ACTIVE,
		RECORDER_STATE_PAUSED,

		RECORDER_STATE__LIMITER
	} rec_state;

	AnsiString caller;
	AnsiString callerName;
	int scode;			// SIP code
	AnsiString reason;	// text from 1st line of the SIP reply
	AnsiString alertInfo;
	AnsiString accessUrl;
	int accessUrlMode;
	int accountId;
	unsigned int callUid;
	int contactId;
	int callAnswerAfter;
	AnsiString initialRxInvite;
	AnsiString paiPeerUri;
	AnsiString paiPeerName;
	AnsiString codecName;

	unsigned int ddata_cnt;
	struct dialog_data ddata[MAX_DIALOG_DATA_CNT];

	int presenceState;
	AnsiString presenceNote;

	int mwiNewMsg;
	int mwiOldMsg;

	std::vector<AnsiString> audioCodecs;

	AnsiString dtmf;
	bool dtmfActive;	

	int requestUid;
	int requestError;	
	AnsiString requestReplyText;

	int recorderId;

	AnsiString contentType;		// MESSAGE
	AnsiString body;			// MESSAGE

	// encryption
	struct Zrtp {
		int sessionId;
		bool active;
		AnsiString sas;
		AnsiString cipher;
		bool verified;
		Zrtp(void):
			sessionId(-1),
			active(false),
			verified(false)
		{}
	} zrtp;

	Callback(void):
		type(TYPE__LIMITER),
		state(CALL_STATE__LIMITER),
		reg_state(REG_STATE__LIMITER),
		paging_tx_state(PAGING_TX__LIMITER),
		app_state(APP_STATE__LIMITER),
		rec_state(RECORDER_STATE__LIMITER),
		scode(-1),
		accessUrlMode(-1),
		accountId(-1),
		callUid(0),
		contactId(-1),
		callAnswerAfter(-1),
		ddata_cnt(0),
		presenceState(-1),
		mwiNewMsg(-1),
		mwiOldMsg(-1),
		dtmfActive(false),
		requestUid(0),
		requestError(0),
		recorderId(-1)
	{
	}
};

#endif
