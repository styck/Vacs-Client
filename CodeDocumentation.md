# VACS Client Code Documentation

This document provides an overview of the VACS (Virtual Audio Console Software) Client codebase, a legacy Windows 95-era application for controlling the Gamble DCX-60 live sound console. The code is written in procedural C, using Win32 APIs for UI, graphics, and hardware interfaces. It relies on global state, custom DLLs, and bitmap-based rendering. Below is a breakdown of the architecture, key components, data flow, and interactions.

## Project Overview
- **Purpose**: Provides a graphical control surface for the DCX-60 console, handling mixing, EQ, cues, groups, sequences, and VU metering. Supports offline mode (simulation) and online mode (via serial/MIDI/ethernet to hardware).
- **Tech Stack**: Win32 API (GDI for drawing, MDI for windows), WinMM for MIDI, raw serial I/O, custom binary formats (.mix for scenes, .dcx.bin for console definition).
- **Build**: CMake-based (modernized from VS6 .dsp files); compiles to Vacs.exe + DLLs (commport, ConsoleDefinition, etc.).
- **Key Files**: 
  - Headers: inc/ (SAMM.h for globals/structs, ConsoleDefinition.h for APIs).
  - Sources: src/ (core logic), subdirs (commport/, ConsoleDefinition/, midi/, DLList32/, usengl/ for resources).
- **Assumptions**: 32-bit Windows; hardcoded paths (e.g., COM1); no Unicode; globals for state (e.g., gpwMemMapBuffer for console data).

## Architecture
The app is monolithic-procedural with modular DLLs:
- **Core App (Vacs_Client.exe)**: Handles UI (MDI windows: FullView, ZoomView, Master), events, file I/O, and orchestration.
- **DLLs**:
  - **ConsoleDefinition.dll**: Parses dcx.bin (console layout/controls), manages device communication (CDef_Init, CDef_GetCSData for online setup), VU data (CDef_StartVuData).
  - **commport.dll**: Serial I/O for DCX protocol (MyOpenCommPort on COM1, Read/WriteCommBlock).
  - **midi.dll**: MIDI in/out via WinMM (midiInOpen/midiOutOpen, callbacks for MTC/SMPTE timecode).
  - **DLList32.dll**: Custom double-linked list for dynamic data (e.g., mixer windows).
  - **usengl.dll**: Resource DLL for bitmaps/icons (loaded in Init.c).
- **Globals/State**:
  - Memory Map (MemoryMap.c): gpwMemMapBuffer (physical console values), gpwMemMapMixer (screen-mapped), gpwMemMapUpdateBufferMask (dirty flags). Updated via SETPHISDATAVALUE/GETPHISDATAVALUE.
  - Zone Maps (ZoneMaps.c): gpZoneMaps_* arrays define UI controls (rects, bitmaps, types like CTRL_TYPE_FADER_VERT).
  - Device Setup (SAMM.h): gDeviceSetup (channel types: inputs/masters/matrix), iaChannelTypes[80].
  - Windows Data: LPMIXERWNDDATA per MDI child (remap tables for scrolling, mouse state).
- **Data Formats**:
  - dcx.bin: Binary console definition (controls, modules).
  - .mix: Scene files (mixer state, groups, sequences).
  - Registry: Preferences/MRU (e.g., last mix file).

## Key Components and Modules
### Entry Point and Initialization (src/Main.c, src/Init.c)
- **WinMain**: Loads accelerators, calls InitializeProc, handles message loop (keys for F-key saves, shift for groups). Autosaves to LA$T.mix every 3min.
- **WndMainProc**: Processes commands (IDM_F_OPEN_FILE for mixes, IDM_V_FULLZOOM for views), timers, close (saves prefs).
- **InitializeProc**: 
  - Loads DLLs/resources (usengl.dll, ConsoleDefinition.dll).
  - Sets up globals (fonts, menus, events like gDisplayEvent).
  - Online/Offline: CDEF_GetCSData (connects to GServer), determines mixer type (e.g., DCX_EVENT_60 for 60 inputs).
  - Registers classes (FullViewClass, ZoomViewClass), loads bitmaps (VU, numbers).
  - Calls Init_MixerData: Allocates memory maps, preps MIDI, loads zone maps/bitmaps.
- **ShutdownProc**: Frees resources (DLLs, bitmaps, memory maps), closes handles (comm, MIDI).

### UI and Rendering (src/Controls.c, src/FullView.c, src/ZoomView.c, src/eqDisplay.c)
- **Drawing**: Bitmap-based via GDI (BitBlt to offscreen DCs like g_hdcTempBuffer).
  - Faders (DrawVertFader/DrawHorizFader): Position knob bitmap based on physical-to-screen conversion (CONVERTPHISICALTOSCREEN).
  - VU Meters (DrawVUData): Log-scaled (gVU_VertDispTable), split peak/average; types (input/comp/gate/master/matrix).
  - Buttons (PushBtn): On/off states from iCtrlBmp[0/1].
  - Text (RdOutText, ChannelNameTextVertical): DrawText with g_hConsoleFont; readouts via CDef_GetCtrlReadout.
  - EQ (DrawEqGraph): Parametric curves from EQ_DISPLAY_DATA.
- **Windows**:
  - MDI Client (MDI_Main.c): Hosts child windows (FullView for overview, ZoomView for details, MasterWindow for masters).
  - Scrolling: lpwRemapToScr/Phis tables map physical channels to screen; ScrollImgWindow adjusts offsets.
  - Labels/Groups (Labels and Groups.c, Groups.c): LV_DISPINFO for list views; ActivateGroup links channels.
- **Updates**: InvalidateRect on changes; UpdateControlsByCtrlNum redraws same-control across windows.

### Event Handling and Controls (src/Events interface.c, src/Controls.c)
- **Mouse/Timer**: HandleMouseMove/LBDown (scans zones via ScanCtrlZonesPnt), HandleCtrlTimer (polls VU).
- **Interactions**:
  - Click: Identifies control (ScanCtrlZonesType), calls hook (e.g., UpDownControl increments via SETPHISDATAVALUE).
  - Filters (ControlDataFilters.c): CheckForSpecialFilters (mutes, cues, pre/post); StartControlDataFilter applies (e.g., HandleInputToggleSwtches).
  - Groups/Stereo: UpdateGroupedControls syncs linked channels; FindStereoPair pairs L/R.
  - Cues: HandleCueMasterMuteFilterEx, CancelAllCues; solo mode (g_bIsSoloCueMode).
- **Data Flow**: Event -> FlipHardwareControl -> SendDataToDevice (to console via comm/MIDI) -> UpdateFromExternalInterface (syncs memory map).

### File and Sequence Management (src/Mix Files.c, src/Sequence.c, src/datafile.c)
- **Mix Files**: LoadMixFile reads .mix (sections: mixer, sequence, groups); WriteMixFile saves state.
- **Sequences**: TreeView for scenes (SeqGoToNext/Prev); MTC timecode via MIDI (AssembleSMPTETime).
- **Data Files**: Binary I/O with headers (VACS_DATA_FILE_HEADER); UpdateWithPropagate for deltas.

### Hardware Communication
- **Serial (commport/CommPort.c)**: MyOpenCommPort (CreateFile \\\\.\\COMx, DCB setup: 38400 baud, 8N1), Read/WriteCommBlock (blocking ReadFile/WriteFile with timeouts), PurgeComm.
- **MIDI (midi/SAMMmidi.c)**: Dll_MidiInProc (FIFO buffer for messages, MTC parsing), Dll_MidiOutProc (headers for sysex). Supports control changes (0xB0) and timecode (0xF1).
- **ConsoleDefinition (ConsoleDefinition/)**: dcx_parser.c loads dcx.bin; CDef_DevCommunication sends/receives via callbacks (DefinitionCallback). Online: TCP to GServer (DCXTCP_protocol.h).
- **Sync**: UpdateFromNetwork polls external (serial/MIDI/TCP); ResendControlData on changes.

### Memory and Utilities
- **MemoryMap.c**: Maps physical (gpwMemMapBuffer) to screen (gpwMemMapMixer); PhisDataToScrPos for scaling.
- **DLList32**: Doubly-linked list for dynamic alloc (e.g., mixer windows).
- **Misc**: Misc BinRes.c loads binary resources; Preferences.c for dialogs (MIDI ports, views).

## Data Flow and Interactions
1. **Startup**: WinMain -> InitializeProc -> CDef_Init (load dcx.bin) -> Init_MixerData (alloc maps, load zones) -> CreateMainWindow (MDI setup).
2. **UI Event**: MouseDown -> HandleLBDown (scan zone) -> UpDownControl (update value) -> SendDataToDevice (to DLL) -> CDef_DevCommunication (serial/MIDI out).
3. **Hardware Update**: Callback (e.g., Dll_MidiInProc) -> UpdateFromExternalInterface -> FlipTheControl (apply filters) -> InvalidateRect (redraw).
4. **File Load**: OpenMixFile -> LoadMixFile (read sections) -> SetMemoryMapDefaults -> RecallMemoryMapBuffer (apply to console).
5. **Dependencies**:
   - All src/ include SAMM.h (globals).
   - UI (Controls.c) -> ZoneMaps.c (layouts) -> ConsoleDefinition.dll (defs).
   - Hardware: Main.c -> commport.dll/MIDI.c -> winmm.
   - Filters (ControlDataFilters.c) interact with Groups.c (stereo pairs) and Events interface.c (cues).

## Potential Issues
- Globals cause tight coupling; no threading safety.
- Blocking I/O (serial/MIDI) can freeze UI.
- Bitmap scaling fails on hi-DPI; no error propagation (e.g., failed DLL load crashes).
- Legacy: wsprintf overflows, no Unicode, Win95 assumptions (e.g., 256-color bitmaps).

For modernization, see [modernization.md](modernization.md).
