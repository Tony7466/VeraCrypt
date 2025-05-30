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

#include "System.h"
#include "Main/Application.h"
#include "UserPreferences.h"
#include "Xml.h"

namespace VeraCrypt
{
	void UserPreferences::SetValue (const wxString &cfgText, bool &cfgVar)
	{
		if (cfgText == L"0")
			cfgVar = false;
		else
			cfgVar = true;
	}

	void UserPreferences::SetValue (const wxString &cfgText, int &cfgVar)
	{
		if (cfgText.empty())
			cfgVar = 0;
		else
			cfgVar = StringConverter::ToUInt32 (wstring (cfgText));
	}

	void UserPreferences::SetValue (const wxString &cfgText, uint64 &cfgVar)
	{
		if (cfgText.empty())
			cfgVar = 0;
		else
			cfgVar = StringConverter::ToUInt64 (wstring (cfgText));
	}

	void UserPreferences::SetValue (const wxString &cfgText, wstring &cfgVar)
	{
		cfgVar = cfgText;
	}

	void UserPreferences::SetValue (const wxString &cfgText, wxString &cfgVar)
	{
		cfgVar = cfgText;
	}

	void UserPreferences::SetValue (const wxString &cfgText, FilesystemPath &cfgVar)
	{
		cfgVar = wstring (cfgText);
	}

	void UserPreferences::Load()
	{
		// first we clear the unknown config map entries
		UnknownConfigMapEntries.clear();

		// Preferences
		FilePath cfgPath = Application::GetConfigFilePath (GetPreferencesFileName());
		if (cfgPath.IsFile())
		{
			map <wxString, wxString> configMap;
			foreach (XmlNode node, XmlParser (cfgPath).GetNodes (L"config"))
			{
				configMap[node.Attributes[L"key"]] = node.InnerText;
			}

#define TC_CONFIG_SET(NAME) if (configMap.count(L###NAME) > 0) { SetValue (configMap[L###NAME], NAME); configMap.erase (L###NAME); }

			TC_CONFIG_SET (BackgroundTaskEnabled);
			TC_CONFIG_SET (BackgroundTaskMenuDismountItemsEnabled);
			TC_CONFIG_SET (BackgroundTaskMenuMountItemsEnabled);
			TC_CONFIG_SET (BackgroundTaskMenuOpenItemsEnabled);
			TC_CONFIG_SET (BeepAfterHotkeyMountDismount);
			if (configMap.count(L"CachePasswords") > 0) { SetValue (configMap[L"CachePasswords"], DefaultMountOptions.CachePassword); configMap.erase (L"CachePasswords"); }
			TC_CONFIG_SET (CloseBackgroundTaskOnNoVolumes);
			TC_CONFIG_SET (CloseExplorerWindowsOnDismount);
			TC_CONFIG_SET (CloseSecurityTokenSessionsAfterMount);
			TC_CONFIG_SET (EMVSupportEnabled);
			TC_CONFIG_SET (DisableKernelEncryptionModeWarning);
			TC_CONFIG_SET (DismountOnInactivity);
			TC_CONFIG_SET (DismountOnLogOff);
			TC_CONFIG_SET (DismountOnPowerSaving);
			TC_CONFIG_SET (DismountOnScreenSaver);
			TC_CONFIG_SET (DisplayMessageAfterHotkeyDismount);
			TC_CONFIG_SET (BackgroundTaskEnabled);
			if (configMap.count(L"FilesystemOptions") > 0) { SetValue (configMap[L"FilesystemOptions"], DefaultMountOptions.FilesystemOptions); configMap.erase (L"FilesystemOptions"); }
			TC_CONFIG_SET (ForceAutoDismount);
			TC_CONFIG_SET (Language);
			TC_CONFIG_SET (LastSelectedSlotNumber);
			TC_CONFIG_SET (MaxVolumeIdleTime);
			TC_CONFIG_SET (MountDevicesOnLogon);
			TC_CONFIG_SET (MountFavoritesOnLogon);

			bool readOnly = false;
			if (configMap.count(L"MountVolumesReadOnly") > 0) { SetValue (configMap[L"MountVolumesReadOnly"], readOnly); configMap.erase (L"MountVolumesReadOnly"); }
			DefaultMountOptions.Protection = readOnly ? VolumeProtection::ReadOnly : VolumeProtection::None;

			if (configMap.count(L"MountVolumesRemovable") > 0) { SetValue (configMap[L"MountVolumesRemovable"], DefaultMountOptions.Removable); configMap.erase (L"MountVolumesRemovable"); }
			if (configMap.count(L"NoHardwareCrypto") > 0) { SetValue (configMap[L"NoHardwareCrypto"], DefaultMountOptions.NoHardwareCrypto); configMap.erase (L"NoHardwareCrypto"); }
			if (configMap.count(L"NoKernelCrypto") > 0) { SetValue (configMap[L"NoKernelCrypto"], DefaultMountOptions.NoKernelCrypto); configMap.erase (L"NoKernelCrypto"); }
			TC_CONFIG_SET (OpenExplorerWindowAfterMount);
			if (configMap.count(L"PreserveTimestamps") > 0) { SetValue (configMap[L"PreserveTimestamps"], DefaultMountOptions.PreserveTimestamps); configMap.erase (L"PreserveTimestamps"); }
			TC_CONFIG_SET (SaveHistory);
			if (configMap.count(L"SecurityTokenLibrary") > 0) { SetValue (configMap[L"SecurityTokenLibrary"], SecurityTokenModule); configMap.erase (L"SecurityTokenLibrary"); }
			TC_CONFIG_SET (StartOnLogon);
			TC_CONFIG_SET (UseKeyfiles);
			TC_CONFIG_SET (WipeCacheOnAutoDismount);
			TC_CONFIG_SET (WipeCacheOnClose);

			wstring defaultPrf;
			if (configMap.count(L"DefaultPRF") > 0) { SetValue (configMap[L"DefaultPRF"], defaultPrf); configMap.erase (L"DefaultPRF"); }

			if (defaultPrf.empty())
				defaultPrf = L"autodetection";
			shared_ptr <Pkcs5Kdf> savedKdf;
			try
			{
				if (defaultPrf != L"autodetection")
					savedKdf = Pkcs5Kdf::GetAlgorithm (defaultPrf);
			}
			catch (ParameterIncorrect&)
			{
			}

			DefaultMountOptions.Kdf = savedKdf;
			DefaultMountOptions.ProtectionKdf = savedKdf;

			// at this point, the configMap should be empty, if not, we have unknown config entries that we need to store
			UnknownConfigMapEntries = configMap;
		}

		// Default keyfiles
		cfgPath = Application::GetConfigFilePath (GetDefaultKeyfilesFileName());
		if (cfgPath.IsFile())
		{
			foreach (const XmlNode &node, XmlParser (cfgPath).GetNodes (L"keyfile"))
			{
				DefaultKeyfiles.push_back (make_shared <Keyfile> ((wstring) node.InnerText));
			}
		}

#ifdef TC_WINDOWS
		// Hotkeys
		Hotkeys = Hotkey::LoadList();
#endif
	}

	void UserPreferences::Save() const
	{
		// Preferences
		class ConfigXmlFormatter
		{
		public:
			void AddEntry (const wchar_t *key, const wxString &value)
			{
				if (!value.empty())
				{
					XmlNode config (L"config");
					config.Attributes[L"key"] = key;
					config.InnerText = value;
					XmlConfig.InnerNodes.push_back (config);
				}
			}

			void AddEntry (const wchar_t *key, bool value)
			{
				AddEntry (key, wxString (value ? L"1" : L"0"));
			}

			void AddEntry (const wchar_t *key, int value)
			{
				wstringstream s;
				s << value;
				AddEntry (key, s.str());
			}

			void AddEntry (const wchar_t *key, uint64 value)
			{
				wstringstream s;
				s << value;
				AddEntry (key, s.str());
			}

			XmlNode XmlConfig;
		};

		ConfigXmlFormatter formatter;
		formatter.XmlConfig.Name = L"configuration";

#define TC_CONFIG_ADD(NAME) formatter.AddEntry (L###NAME, NAME)

		TC_CONFIG_ADD (BackgroundTaskEnabled);
		TC_CONFIG_ADD (BackgroundTaskMenuDismountItemsEnabled);
		TC_CONFIG_ADD (BackgroundTaskMenuMountItemsEnabled);
		TC_CONFIG_ADD (BackgroundTaskMenuOpenItemsEnabled);
		TC_CONFIG_ADD (BeepAfterHotkeyMountDismount);
		formatter.AddEntry (L"CachePasswords", DefaultMountOptions.CachePassword);
		TC_CONFIG_ADD (CloseBackgroundTaskOnNoVolumes);
		TC_CONFIG_ADD (CloseExplorerWindowsOnDismount);
		TC_CONFIG_ADD (CloseSecurityTokenSessionsAfterMount);
        TC_CONFIG_ADD (EMVSupportEnabled);
		TC_CONFIG_ADD (DisableKernelEncryptionModeWarning);
		TC_CONFIG_ADD (DismountOnInactivity);
		TC_CONFIG_ADD (DismountOnLogOff);
		TC_CONFIG_ADD (DismountOnPowerSaving);
		TC_CONFIG_ADD (DismountOnScreenSaver);
		TC_CONFIG_ADD (DisplayMessageAfterHotkeyDismount);
		TC_CONFIG_ADD (BackgroundTaskEnabled);
		formatter.AddEntry (L"FilesystemOptions", DefaultMountOptions.FilesystemOptions);
		TC_CONFIG_ADD (ForceAutoDismount);
		TC_CONFIG_ADD (Language);
		TC_CONFIG_ADD (LastSelectedSlotNumber);
		TC_CONFIG_ADD (MaxVolumeIdleTime);
		TC_CONFIG_ADD (MountDevicesOnLogon);
		TC_CONFIG_ADD (MountFavoritesOnLogon);
		formatter.AddEntry (L"MountVolumesReadOnly", DefaultMountOptions.Protection == VolumeProtection::ReadOnly);
		formatter.AddEntry (L"MountVolumesRemovable", DefaultMountOptions.Removable);
		formatter.AddEntry (L"NoHardwareCrypto", DefaultMountOptions.NoHardwareCrypto);
		formatter.AddEntry (L"NoKernelCrypto", DefaultMountOptions.NoKernelCrypto);
		TC_CONFIG_ADD (OpenExplorerWindowAfterMount);
		formatter.AddEntry (L"PreserveTimestamps", DefaultMountOptions.PreserveTimestamps);
		TC_CONFIG_ADD (SaveHistory);
		formatter.AddEntry (L"SecurityTokenLibrary", wstring (SecurityTokenModule));
		TC_CONFIG_ADD (StartOnLogon);
		TC_CONFIG_ADD (UseKeyfiles);
		TC_CONFIG_ADD (WipeCacheOnAutoDismount);
		TC_CONFIG_ADD (WipeCacheOnClose);

		wstring defaultPrf = L"autodetection";
		if (DefaultMountOptions.Kdf)
			defaultPrf = DefaultMountOptions.Kdf->GetName ();
		formatter.AddEntry (L"DefaultPRF", defaultPrf);

		// add unknown config entries by iterating over all elements of the UnknownConfigMapEntries map
		for (map<wxString, wxString>::const_iterator it = UnknownConfigMapEntries.begin(); it != UnknownConfigMapEntries.end(); ++it)
		{
			formatter.AddEntry(it->first.c_str(), it->second);
		}

		XmlWriter writer (Application::GetConfigFilePath (GetPreferencesFileName(), true));
		writer.WriteNode (formatter.XmlConfig);
		writer.Close();

		// Default keyfiles
		FilePath keyfilesCfgPath = Application::GetConfigFilePath (GetDefaultKeyfilesFileName(), true);

		if (DefaultKeyfiles.empty())
		{
			if (keyfilesCfgPath.IsFile())
				keyfilesCfgPath.Delete();
		}
		else
		{
			XmlNode keyfilesXml (L"defaultkeyfiles");

			foreach_ref (const Keyfile &keyfile, DefaultKeyfiles)
			{
				keyfilesXml.InnerNodes.push_back (XmlNode (L"keyfile", wxString (wstring(FilesystemPath (keyfile)))));
			}

			XmlWriter keyfileWriter (keyfilesCfgPath);
			keyfileWriter.WriteNode (keyfilesXml);
			keyfileWriter.Close();
		}

#ifdef TC_WINDOWS
		// Hotkeys
		Hotkey::SaveList (Hotkeys);
#endif
	}
}
