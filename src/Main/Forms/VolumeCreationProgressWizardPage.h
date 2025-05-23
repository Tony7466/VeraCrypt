/*
 Derived from source code of TrueCrypt 7.1a, which is
 Copyright (c) 2008-2012 TrueCrypt Developers Association and which is governed
 by the TrueCrypt License 3.0.

 Modifications and additions to the original source code (contained in this file)
 and all other portions of this file are Copyright (c) 2013-2025 AM Crypto
 and are governed by the Apache License 2.0 the full text of which is
 contained in the file License.txt included in VeraCrypt binary and source
 code distribution packages.
*/

#ifndef TC_HEADER_Main_Forms_VolumeCreationProgressWizardPage
#define TC_HEADER_Main_Forms_VolumeCreationProgressWizardPage

#include "Forms.h"
#include "Core/VolumeCreator.h"

namespace VeraCrypt
{
	class VolumeCreationProgressWizardPage : public VolumeCreationProgressWizardPageBase
	{
	public:
		VolumeCreationProgressWizardPage (wxPanel* parent, bool displayKeyInfo);
		~VolumeCreationProgressWizardPage () { }

		void EnableAbort (bool enable = true) { AbortButton->Enable (enable); }
		bool IsKeyInfoDisplayed () const { return DisplayKeysCheckBox->GetValue(); }
		bool IsValid () { return true; }
		void OnRandomPoolTimer ();
		void SetKeyInfo (const VolumeCreator::KeyInfo &keyInfo);
		void SetMaxStaticTextWidth (int width);
		void SetPageText (const wxString &text) { InfoStaticText->SetLabel (text); }
		void SetProgressRange (uint64 progressBarRange);
		void SetProgressValue (uint64 value);
		void SetProgressState (bool volumeCreatorRunning);
		void IncrementEntropyProgress ();

		Event AbortEvent;

	protected:
		void OnAbortButtonClick (wxCommandEvent& event);
		void OnDisplayKeysCheckBoxClick (wxCommandEvent& event);
		void ShowAsterisks (wxStaticText *textCtrl);
		void ShowBytes (wxStaticText *textCtrl, const ConstBufferPtr &buffer, bool appendDots = true);

		static const size_t MaxDisplayedKeyBytes = 13;

		int PreviousGaugeValue;
		uint64 ProgressBarRange;
		unique_ptr <wxTimer> RandomPoolTimer;
		int RealProgressBarRange;
		wxLongLong StartTime;
		bool VolumeCreatorRunning;
		int MouseEventsCounter;
		Mutex AccessMutex;
	};
}

#endif // TC_HEADER_Main_Forms_VolumeCreationProgressWizardPage
