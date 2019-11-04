//---------------------------------------------------------------------------

#ifndef SIMPLE_MessagesH
#define SIMPLE_MessagesH
//---------------------------------------------------------------------------

#include <System.hpp>

/** \note Using just "Messages.h" for name creates some conflict (VCL?)
*/

class TfrmMessage;

namespace SIMPLE_Messages
{
	void RegisterWindow(TfrmMessage *frmMessage);
	void UnregisterWindow(TfrmMessage *frmMessage);
	void CloseAllWindows(void);
	void OnIncomingMessage(AnsiString caller, AnsiString contentType, AnsiString body);
}

#endif
