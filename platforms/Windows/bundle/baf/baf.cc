// Copyright © 2026 Saleem Abdulrasool <compnerd@compnerd.org>
// SPDX-License-Identifier: Apache-2.0

// This file implements BA function extensions for the Swift installer.
//
// On the Install page, it reserves native checkbox text margin and paints
// a UAC shield inside the per-machine checkbox. On scope changes, it
// swaps InstallRoot between per-user and per-machine shadow variables,
// because thmutil only flushes the edit box during page navigation.
//
// On the Options page, it owner-draws the tab strip and attaches per-tab
// tooltips. When TCN_SELCHANGE fires, it updates OptionsTab and
// synthesizes a click on the hidden BAFRefreshTrigger checkbox so
// thmutil's OnButtonClicked path runs ThemeShowPageEx(REFRESH) and
// re-evaluates every VisibleCondition and EnableCondition on the page. It
// also populates the Default Toolchain combobox and SDK TreeView, because
// thmutil creates those controls but does not load items or bind
// variables. Finally, it snapshots and restores variables that BAF
// manages manually when the user confirms or cancels.

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <CommCtrl.h>
#include <objbase.h>
#include <uxtheme.h>
#include <vsstyle.h>

#include <algorithm>
#include <array>
#include <format>
#include <memory>
#include <string>
#include <type_traits>

// clang-format off
// dutil.h defines DAPI (required by dictutil.h); dictutil.h defines
// STRINGDICT_HANDLE, which balinfo.h pulls in via BalBaseBAFunctions.
#include "dutil.h"
#include "dictutil.h"
#include "locutil.h"
#include "pathutil.h"
#include "strutil.h"
#include "xmlutil.h"

// BootstrapperApplicationBase.h drags in the BOOTSTRAPPER_APPLICATION_MESSAGE
// enum that BAFunctions.h references.
#include "BootstrapperApplicationBase.h"
#include "BAFunctions.h"
#include "IBAFunctions.h"
#include "BalBaseBAFunctions.h"
#include "BalBaseBAFunctionsProc.h"
// clang-format on

static HINSTANCE vhInstance = nullptr;

namespace {

// MARK: - Identifiers

constexpr LPCWSTR kScopeControl = L"InstallPerMachine";
constexpr LPCWSTR kScopeHintControl = L"InstallPerMachineHint";
constexpr LPCWSTR kInstallRoot = L"InstallRoot";
constexpr LPCWSTR kPerUserShadow = L"InstallRootPerUser";
constexpr LPCWSTR kPerMachineShadow = L"InstallRootPerMachine";
constexpr LPCWSTR kOptionsTabs = L"OptionsTabs";
constexpr LPCWSTR kOptionsTabVariable = L"OptionsTab";
constexpr LPCWSTR kRefreshTrigger = L"BAFRefreshTrigger";
constexpr LPCWSTR kDefaultToolchainCombo = L"OptionsDefaultToolchain";
constexpr LPCWSTR kIncludeNoAssertsVariable = L"OptionsIncludeNoAsserts";
constexpr LPCWSTR kSDKTree = L"OptionsSDKsTree";
constexpr LPCWSTR kOptionsButton = L"OptionsButton";
constexpr LPCWSTR kOptionsOkButton = L"OptionsOkButton";
constexpr LPCWSTR kOptionsCancelButton = L"OptionsCancelButton";
constexpr LPCWSTR kLocalizationFile = L"thm.wxl";

// MARK: - Layout Constants

constexpr int kScopeGlyphGap = 6;
constexpr int kScopeShieldTextGap = 4;
constexpr int kScopeFocusPadding = 2;

// MARK: - Message IDs

// These are deferred SetFocus signals posted from BN_CLICKED handlers.
// The page transition runs after our dispatch returns, so we post a
// message and set focus in WndProc after the new page becomes visible.
constexpr UINT kMsgFocusInstallRoot = WM_APP + 1;
constexpr UINT kMsgFocusOptionsTabs = WM_APP + 2;
constexpr UINT kMsgApplyOptionsTabSelection = WM_APP + 3;
constexpr UINT kMsgCycleOptionsTabs = WM_APP + 4;
constexpr UINT kMsgRedrawInstallScope = WM_APP + 5;

// MARK: - Toolchain Variants

constexpr int kToolchainVariantAsserts = 0;
constexpr int kToolchainVariantNoAsserts = 1;
constexpr UINT kTreeStateImageUnchecked = 1;
constexpr UINT kTreeStateImageChecked = 2;

// These names represent toolchain variants for persistence and display in
// the Default Toolchain combobox. Persisted values stay stable
// ("Asserts" and "NoAsserts"), while display labels are localized.
constexpr LPCWSTR kToolchainVariantValues[] = {
    L"Asserts",
    L"NoAsserts",
};
constexpr LPCWSTR kToolchainVariantLabelLocStrings[] = {
    L"#(loc.OptionsDefaultAsserts)",
    L"#(loc.OptionsDefaultNoAsserts)",
};

// MARK: - SDK Tree Model

// SDK TreeView rows are declared in document order with a `depth` value
// that matches the parent-child layout (0 = platform, 1 = SDK, 2 =
// redistributable). The bundle variable names are derived from the row
// kind plus platform/architecture metadata so the table describes the
// installer model without repeating the complete OptionsInstall* and
// BAFAvail* names for every row.
enum class SDKTreeItemKind {
  Platform,
  SDK,
  SharedRedist,
  PrivateRedist,
};

struct SDKTreeItem {
  LPCWSTR platform;
  struct {
    LPCWSTR label;
    LPCWSTR option;
    LPCWSTR availability;
  } architecture;
  SDKTreeItemKind kind;
  int depth;
};

constexpr SDKTreeItem kSDKTreeItems[] = {
    {L"Windows", {}, SDKTreeItemKind::Platform, 0},
    {L"Windows", {L"amd64", L"AMD64", L"AMD64"}, SDKTreeItemKind::SDK, 1},
    {L"Windows", {nullptr, L"AMD64", L"AMD64"},
     SDKTreeItemKind::SharedRedist, 2},
    {L"Windows", {nullptr, L"AMD64", L"AMD64"},
     SDKTreeItemKind::PrivateRedist, 2},
    {L"Windows", {L"arm64", L"ARM64", L"ARM64"}, SDKTreeItemKind::SDK, 1},
    {L"Windows", {nullptr, L"ARM64", L"ARM64"},
     SDKTreeItemKind::SharedRedist, 2},
    {L"Windows", {nullptr, L"ARM64", L"ARM64"},
     SDKTreeItemKind::PrivateRedist, 2},
    {L"Windows", {L"x86", L"X86", L"X86"}, SDKTreeItemKind::SDK, 1},
    {L"Windows", {nullptr, L"X86", L"X86"}, SDKTreeItemKind::SharedRedist,
     2},
    {L"Windows", {nullptr, L"X86", L"X86"}, SDKTreeItemKind::PrivateRedist,
     2},
    {L"Android", {}, SDKTreeItemKind::Platform, 0},
    {L"Android", {L"arm64", L"ARM64", L"ARM64"}, SDKTreeItemKind::SDK, 1},
    {L"Android", {L"amd64", L"AMD64", L"AMD64"}, SDKTreeItemKind::SDK, 1},
    {L"Android", {L"armv7", L"ARM", L"ARMv7"}, SDKTreeItemKind::SDK, 1},
    {L"Android", {L"x86", L"X86", L"X86"}, SDKTreeItemKind::SDK, 1},
};

std::wstring SDKTreeLabelLocString(SDKTreeItem const &item) {
  switch (item.kind) {
  case SDKTreeItemKind::Platform:
    return std::format(L"#(loc.Plt_ProductName_{})", item.platform);
  case SDKTreeItemKind::SDK:
    return std::format(L"#(loc.Sdk_ProductName_{}_{})", item.platform,
                       item.architecture.label);
  case SDKTreeItemKind::SharedRedist:
    return L"#(loc.Redist_shared)";
  case SDKTreeItemKind::PrivateRedist:
    return L"#(loc.Redist_private)";
  }
  return {};
}

std::wstring SDKTreeVariable(SDKTreeItem const &item) {
  std::wstring variable = std::wstring(L"OptionsInstall") + item.platform;
  switch (item.kind) {
  case SDKTreeItemKind::Platform:
    return variable + L"Platform";
  case SDKTreeItemKind::SDK:
    return variable + L"SDK" + item.architecture.option;
  case SDKTreeItemKind::SharedRedist:
    return variable + L"RedistShared" + item.architecture.option;
  case SDKTreeItemKind::PrivateRedist:
    return variable + L"RedistPrivate" + item.architecture.option;
  }
  return {};
}

std::wstring SDKTreeAvailability(SDKTreeItem const &item) {
  std::wstring availability = std::wstring(L"BAFAvail") + item.platform;
  if (item.kind == SDKTreeItemKind::Platform)
    return availability + L"Platform";
  return availability + item.architecture.availability;
}

constexpr UINT_PTR kScopeCheckboxSubclassId = 4;
constexpr UINT_PTR kOptionsTabsSubclassId = 5;
constexpr UINT_PTR kOptionsCtrlTabSubclassId = 6;

// MARK: - RAII Deleters

struct GdiObjectDeleter {
  void operator()(HGDIOBJ hObject) const noexcept {
    if (hObject)
      ::DeleteObject(hObject);
  }
};

struct DcDeleter {
  void operator()(HDC hDC) const noexcept {
    if (hDC)
      ::DeleteDC(hDC);
  }
};

struct ImageListDeleter {
  void operator()(HIMAGELIST hImageList) const noexcept {
    if (hImageList)
      ::ImageList_Destroy(hImageList);
  }
};

struct IconDeleter {
  void operator()(HICON hIcon) const noexcept {
    if (hIcon)
      ::DestroyIcon(hIcon);
  }
};

struct WixStringDeleter {
  void operator()(LPWSTR wszValue) const noexcept {
    StrFree(wszValue);
  }
};

// MARK: - RAII Aliases

template <typename Handle, typename Deleter>
using ScopedHandle =
    std::unique_ptr<std::remove_pointer_t<Handle>, Deleter>;

using ScopedFontHandle = ScopedHandle<HFONT, GdiObjectDeleter>;
using ScopedBrushHandle = ScopedHandle<HBRUSH, GdiObjectDeleter>;
using ScopedBitmapHandle = ScopedHandle<HBITMAP, GdiObjectDeleter>;
using ScopedMemoryDCHandle = ScopedHandle<HDC, DcDeleter>;
using ScopedImageListHandle = ScopedHandle<HIMAGELIST, ImageListDeleter>;
using ScopedIconHandle = ScopedHandle<HICON, IconDeleter>;
using ScopedWixString = ScopedHandle<LPWSTR, WixStringDeleter>;

// MARK: - RAII Helpers

class ScopedWindowDC {
  HWND hWnd_ = nullptr;
  HDC hDC_ = nullptr;

public:
  explicit ScopedWindowDC(HWND hWnd) noexcept
      : hWnd_(hWnd), hDC_(::GetDC(hWnd)) {}

  ~ScopedWindowDC() noexcept {
    if (hDC_)
      ::ReleaseDC(hWnd_, hDC_);
  }

  ScopedWindowDC(ScopedWindowDC const &) = delete;
  ScopedWindowDC &operator=(ScopedWindowDC const &) = delete;

  HDC get() const noexcept {
    return hDC_;
  }

  explicit operator bool() const noexcept {
    return hDC_ != nullptr;
  }
};

class ScopedPaintSession {
  HWND hWnd_ = nullptr;
  PAINTSTRUCT ps_ = {};
  HDC hDC_ = nullptr;

public:
  explicit ScopedPaintSession(HWND hWnd) noexcept : hWnd_(hWnd) {
    hDC_ = ::BeginPaint(hWnd_, &ps_);
  }

  ~ScopedPaintSession() noexcept {
    if (hDC_)
      ::EndPaint(hWnd_, &ps_);
  }

  ScopedPaintSession(ScopedPaintSession const &) = delete;
  ScopedPaintSession &operator=(ScopedPaintSession const &) = delete;

  HDC get() const noexcept {
    return hDC_;
  }

  explicit operator bool() const noexcept {
    return hDC_ != nullptr;
  }
};

template <typename ObjectType>
class ScopedSelectObject {
  HDC hDC_ = nullptr;
  ObjectType hOld_ = nullptr;

public:
  ScopedSelectObject(HDC hDC, ObjectType hObject) noexcept : hDC_(hDC) {
    if (hDC_)
      hOld_ = static_cast<ObjectType>(::SelectObject(hDC_, hObject));
  }

  ~ScopedSelectObject() noexcept {
    if (hDC_ && hOld_)
      ::SelectObject(hDC_, hOld_);
  }

  ScopedSelectObject(ScopedSelectObject const &) = delete;
  ScopedSelectObject &operator=(ScopedSelectObject const &) = delete;

  explicit operator bool() const noexcept {
    return hOld_ != nullptr;
  }
};

// MARK: - String and Control Helpers

// MARK: - String Formatting Helpers

ScopedWixString FormatBalStringScoped(LPCWSTR wszFormat) noexcept {
  LPWSTR wszValue = nullptr;
  if (wszFormat == nullptr || FAILED(BalFormatString(wszFormat, &wszValue)))
    return {};
  return ScopedWixString(wszValue);
}

ScopedWixString
LocalizeStringScoped(WIX_LOCALIZATION const *pWixLoc,
                     LPCWSTR wszLocString) noexcept {
  LPWSTR wszValue = nullptr;
  if (pWixLoc == nullptr || wszLocString == nullptr ||
      FAILED(StrAllocString(&wszValue, wszLocString, 0)))
    return {};
  if (FAILED(LocLocalizeString(pWixLoc, &wszValue))) {
    ReleaseStr(wszValue);
    return {};
  }
  return ScopedWixString(wszValue);
}

ScopedWixString FormatWixVariable(LPCWSTR wszName) noexcept {
  LPWSTR wszRawFormat = nullptr;
  if (wszName == nullptr ||
      FAILED(StrAllocFormatted(&wszRawFormat, L"[%ls]", wszName)))
    return {};
  ScopedWixString wszFormat(wszRawFormat);
  return FormatBalStringScoped(wszFormat.get());
}

ScopedWixString GetBalStringVariableScoped(LPCWSTR wszName) noexcept {
  LPWSTR wszValue = nullptr;
  if (wszName == nullptr || FAILED(BalGetStringVariable(wszName, &wszValue)))
    return {};
  return ScopedWixString(wszValue);
}

std::wstring CopyWindowText(HWND hWnd) noexcept {
  int cch = ::GetWindowTextLengthW(hWnd);
  if (cch <= 0)
    return {};

  std::wstring wszText;
  wszText.resize(static_cast<size_t>(cch + 1));
  int cchCopied = ::GetWindowTextW(hWnd, wszText.data(), cch + 1);
  wszText.resize(cchCopied > 0 ? static_cast<size_t>(cchCopied) : 0);
  return wszText;
}

// MARK: - Generic Control Helpers

bool IsControl(LPCWSTR wszName, LPCWSTR wszTarget) noexcept {
  if (wszName == nullptr || wszTarget == nullptr)
    return false;
  return ::CompareStringOrdinal(wszName, -1, wszTarget, -1, FALSE) == CSTR_EQUAL;
}

bool StartsWithInsensitive(LPCWSTR wszText, LPCWSTR wszPrefix) noexcept {
  if (!wszText || !wszPrefix)
    return false;
  int cchPrefix = ::lstrlenW(wszPrefix);
  if (cchPrefix <= 0)
    return false;
  return ::CompareStringOrdinal(wszText, cchPrefix, wszPrefix, cchPrefix,
                                FALSE) == CSTR_EQUAL;
}

bool IsButtonClicked(WPARAM wParam) noexcept {
  return HIWORD(wParam) == BN_CLICKED;
}

void RedrawControlNow(HWND hControl) noexcept {
  if (hControl == nullptr)
    return;

  ::RedrawWindow(hControl, nullptr, nullptr,
                 RDW_INVALIDATE | RDW_ERASE | RDW_FRAME | RDW_ERASENOW |
                     RDW_UPDATENOW);

  HWND hParent = ::GetParent(hControl);
  if (hParent == nullptr)
    return;

  RECT rc = {};
  if (!::GetWindowRect(hControl, &rc))
    return;
  ::MapWindowPoints(HWND_DESKTOP, hParent, reinterpret_cast<POINT *>(&rc), 2);
  ::InflateRect(&rc, 2, 2);
  ::RedrawWindow(hParent, &rc, nullptr,
                 RDW_INVALIDATE | RDW_ERASE | RDW_ERASENOW | RDW_UPDATENOW |
                     RDW_ALLCHILDREN);
}

// MARK: - Options Tab Mnemonic Helpers

struct TabMnemonicMap {
  std::array<int, 256> bitmap = {};

  TabMnemonicMap() noexcept {
    bitmap.fill(-1);
  }
};

// This normalizes mnemonic keys to uppercase ASCII letters and digits.
// It returns 0 for unsupported characters.
UINT NormalizeMnemonicKey(WPARAM wParam) noexcept {
  if (wParam >= L'a' && wParam <= L'z')
    return static_cast<UINT>(wParam - (L'a' - L'A'));
  if ((wParam >= L'A' && wParam <= L'Z') || (wParam >= L'0' && wParam <= L'9'))
    return static_cast<UINT>(wParam);
  return 0;
}

// This returns the first single '&' mnemonic marker from a tab label.
// Escaped markers ('&&') are skipped.
wchar_t FindMnemonicCharacter(LPCWSTR wszText) noexcept {
  if (!wszText)
    return L'\0';
  for (size_t i = 0; wszText[i] != L'\0'; ++i) {
    if (wszText[i] != L'&')
      continue;
    wchar_t next = wszText[i + 1];
    if (next == L'&') {
      ++i;
      continue;
    }
    return next;
  }
  return L'\0';
}

// This builds a one-time lookup table from normalized mnemonic key to
// tab index. The first matching mnemonic wins when duplicates exist.
void BuildTabMnemonicMap(HWND hTabs, TabMnemonicMap *pMap) noexcept {
  if (!hTabs || !pMap)
    return;

  pMap->bitmap.fill(-1);
  int iCount = TabCtrl_GetItemCount(hTabs);
  for (int i = 0; i < iCount; ++i) {
    wchar_t wszBuf[128] = {};
    TCITEMW tci = {};
    tci.mask = TCIF_TEXT;
    tci.pszText = wszBuf;
    tci.cchTextMax = static_cast<int>(std::size(wszBuf));
    if (!TabCtrl_GetItem(hTabs, i, &tci))
      continue;
    UINT uKey = NormalizeMnemonicKey(FindMnemonicCharacter(wszBuf));
    if (uKey && pMap->bitmap[uKey] < 0)
      pMap->bitmap[uKey] = i;
  }
}

// This returns the destination tab index for an Alt+key system message.
// It returns -1 when the message has no supported mnemonic.
int FindTabFromSystemKey(WPARAM wParam, LPARAM lParam,
                         TabMnemonicMap const *pMap) noexcept {
  if (!pMap || !(lParam & (1 << 29)))
    return -1;
  UINT uKey = NormalizeMnemonicKey(wParam);
  if (!uKey || uKey >= pMap->bitmap.size())
    return -1;
  return pMap->bitmap[uKey];
}

bool IsVirtualKeyDown(int iVirtualKey) noexcept {
  return ::GetKeyState(iVirtualKey) & 0x8000;
}

bool WantsCtrlTabDialogCode(LPARAM lParam) noexcept {
  MSG const *pMsg = reinterpret_cast<MSG const *>(lParam);
  return pMsg && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB &&
         IsVirtualKeyDown(VK_CONTROL);
}

bool ShouldInstallOptionsCtrlTabRelay(LPCWSTR wszName) noexcept {
  if (!wszName)
    return false;
  if (IsControl(wszName, kDefaultToolchainCombo) ||
      IsControl(wszName, kSDKTree) || IsControl(wszName, kOptionsOkButton) ||
      IsControl(wszName, kOptionsCancelButton))
    return true;
  return StartsWithInsensitive(wszName, L"OptionsInstall");
}

bool SelectOptionsTabAndRefresh(HWND hTabs, int iTab) noexcept {
  if (!hTabs || iTab < 0)
    return false;
  int iSel = TabCtrl_GetCurSel(hTabs);
  if (iSel == iTab)
    return true;
  if (TabCtrl_SetCurSel(hTabs, iTab) < 0)
    return false;
  if (HWND hRoot = ::GetAncestor(hTabs, GA_ROOT))
    ::PostMessageW(hRoot, kMsgApplyOptionsTabSelection, 0, 0);
  return true;
}

bool SelectAdjacentOptionsTab(HWND hTabs, bool bReverse) noexcept {
  if (!hTabs)
    return false;
  int cTabs = TabCtrl_GetItemCount(hTabs);
  if (cTabs <= 0)
    return false;
  int iSel = TabCtrl_GetCurSel(hTabs);
  if (iSel < 0 || iSel >= cTabs)
    iSel = 0;
  int iNext = bReverse ? (iSel + cTabs - 1) % cTabs : (iSel + 1) % cTabs;
  return SelectOptionsTabAndRefresh(hTabs, iNext);
}

LRESULT CALLBACK
OptionsCtrlTabRelayProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                        UINT_PTR, DWORD_PTR) noexcept {
  switch (uMsg) {
  case WM_GETDLGCODE: {
    LRESULT lCode = ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
    if (WantsCtrlTabDialogCode(lParam))
      lCode |= DLGC_WANTTAB;
    return lCode;
  }
  case WM_KEYDOWN:
    if (wParam == VK_TAB && IsVirtualKeyDown(VK_CONTROL))
      if (HWND hRoot = ::GetAncestor(hWnd, GA_ROOT);
          hRoot && ::SendMessageW(hRoot, kMsgCycleOptionsTabs,
                                  IsVirtualKeyDown(VK_SHIFT) ? 1 : 0, 0))
        return 0;
    break;
  case WM_NCDESTROY:
    ::RemoveWindowSubclass(hWnd, OptionsCtrlTabRelayProc,
                           kOptionsCtrlTabSubclassId);
    break;
  }
  return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// This subclasses the options tab control to handle Alt+mnemonics.
// When a mapped mnemonic is pressed, it selects the tab and asks the BA
// window to run ApplyTabSelection on the normal message path.
LRESULT CALLBACK
OptionsTabsMnemonicProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                        UINT_PTR, DWORD_PTR dwRefData) noexcept {
  auto const *pMap = reinterpret_cast<TabMnemonicMap const *>(dwRefData);
  switch (uMsg) {
  case WM_GETDLGCODE: {
    LRESULT lCode = ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
    if (WantsCtrlTabDialogCode(lParam))
      lCode |= DLGC_WANTTAB;
    return lCode;
  }
  case WM_KEYDOWN:
    if (wParam == VK_TAB && IsVirtualKeyDown(VK_CONTROL) &&
        SelectAdjacentOptionsTab(hWnd, IsVirtualKeyDown(VK_SHIFT)))
      return 0;
    break;
  case WM_SYSKEYDOWN:
  case WM_SYSCHAR: {
    int iTab = FindTabFromSystemKey(wParam, lParam, pMap);
    if (iTab >= 0 && SelectOptionsTabAndRefresh(hWnd, iTab))
      return 0;
    break;
  }
  case WM_NCDESTROY:
    ::RemoveWindowSubclass(hWnd, OptionsTabsMnemonicProc,
                           kOptionsTabsSubclassId);
    break;
  }
  return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// MARK: - Toolchain Variant Helpers

int ToolchainVariantIndexFromValue(LPCWSTR wszValue) noexcept {
  if (wszValue == nullptr)
    return kToolchainVariantAsserts;
  for (int i = 0; i < static_cast<int>(std::size(kToolchainVariantValues)); ++i)
    if (IsControl(wszValue, kToolchainVariantValues[i]))
      return i;
  return kToolchainVariantAsserts;
}

bool SelectToolchainVariantInCombo(HWND hCombo, int iVariant) noexcept {
  int iCount = static_cast<int>(::SendMessageW(hCombo, CB_GETCOUNT, 0, 0));
  for (int i = 0; i < iCount; ++i)
    if (::SendMessageW(hCombo, CB_GETITEMDATA, i, 0) == iVariant)
      return ::SendMessageW(hCombo, CB_SETCURSEL, i, 0) != CB_ERR;
  return false;
}

UINT GetTreeStateImageIndex(UINT uState) noexcept {
  return (uState & TVIS_STATEIMAGEMASK) >> 12;
}

// MARK: - Install Scope Shield Helpers

// The checkbox glyph, shield, and label are painted by the checkbox
// subclass itself instead of child windows, so there is no separate HWND to
// intercept clicks. The native control still owns input and checked state;
// BAF draws that state through the themed Button class, then draws the
// shield, localized label, and focus rectangle in a single buffered pass.
struct ScopeCheckboxData {
  HICON hIcon = nullptr;
  std::wstring text;
  int x = 0;
  int y = 0;
  int size = 0;
  int inset = 0;
  bool trackingMouse = false;
};

void FillControlBackground(HWND hWnd, HDC hDC, RECT const &rc) noexcept {
  if (hDC == nullptr)
    return;

  HBRUSH hBrush = nullptr;
  if (HWND hParent = ::GetParent(hWnd))
    hBrush = reinterpret_cast<HBRUSH>(
        ::SendMessageW(hParent, WM_CTLCOLORBTN, reinterpret_cast<WPARAM>(hDC),
                       reinterpret_cast<LPARAM>(hWnd)));
  if (!hBrush)
    hBrush = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
  ::FillRect(hDC, &rc, hBrush);
}

int CheckboxThemeState(HWND hWnd, ScopeCheckboxData const *pData) noexcept {
  bool bEnabled = ::IsWindowEnabled(hWnd) != FALSE;
  bool bChecked = ::SendMessageW(hWnd, BM_GETCHECK, 0, 0) == BST_CHECKED;
  bool bPressed = (::SendMessageW(hWnd, BM_GETSTATE, 0, 0) & BST_PUSHED) != 0;
  bool bHot = pData && pData->trackingMouse;

  if (bChecked) {
    if (!bEnabled)
      return CBS_CHECKEDDISABLED;
    if (bPressed)
      return CBS_CHECKEDPRESSED;
    if (bHot)
      return CBS_CHECKEDHOT;
    return CBS_CHECKEDNORMAL;
  }

  if (!bEnabled)
    return CBS_UNCHECKEDDISABLED;
  if (bPressed)
    return CBS_UNCHECKEDPRESSED;
  if (bHot)
    return CBS_UNCHECKEDHOT;
  return CBS_UNCHECKEDNORMAL;
}

SIZE CheckboxGlyphSize(HWND hWnd) noexcept {
  SIZE szGlyph = {::GetSystemMetrics(SM_CXMENUCHECK),
                  ::GetSystemMetrics(SM_CYMENUCHECK)};

  HTHEME hTheme = ::OpenThemeData(hWnd, L"Button");
  if (hTheme) {
    SIZE sz = {};
    if (SUCCEEDED(::GetThemePartSize(hTheme, nullptr, BP_CHECKBOX,
                                     CBS_UNCHECKEDNORMAL, nullptr, TS_DRAW,
                                     &sz))) {
      szGlyph = sz;
    }
    ::CloseThemeData(hTheme);
  }

  return szGlyph;
}

RECT CheckboxGlyphRect(HWND hWnd) noexcept {
  RECT rc = {};
  ::GetClientRect(hWnd, &rc);
  SIZE szGlyph = CheckboxGlyphSize(hWnd);

  int iY = (rc.bottom - rc.top - szGlyph.cy) / 2;
  return {0, iY, szGlyph.cx, iY + szGlyph.cy};
}

void DrawScopeCheckboxGlyph(HWND hWnd, HDC hDC,
                            ScopeCheckboxData const *pData) noexcept {
  if (hDC == nullptr)
    return;

  RECT rcGlyph = CheckboxGlyphRect(hWnd);
  int iState = CheckboxThemeState(hWnd, pData);

  HTHEME hTheme = ::OpenThemeData(hWnd, L"Button");
  if (hTheme) {
    ::DrawThemeBackground(hTheme, hDC, BP_CHECKBOX, iState, &rcGlyph,
                          nullptr);
    ::CloseThemeData(hTheme);
    return;
  }

  UINT uFlags = DFCS_BUTTONCHECK;
  if (::SendMessageW(hWnd, BM_GETCHECK, 0, 0) == BST_CHECKED)
    uFlags |= DFCS_CHECKED;
  if ((::SendMessageW(hWnd, BM_GETSTATE, 0, 0) & BST_PUSHED) != 0)
    uFlags |= DFCS_PUSHED;
  if (!::IsWindowEnabled(hWnd))
    uFlags |= DFCS_INACTIVE;
  ::DrawFrameControl(hDC, &rcGlyph, DFC_BUTTON, uFlags);
}

void DrawScopeCheckboxExtras(HWND hWnd, HDC hDC,
                             const ScopeCheckboxData &data) noexcept {
  if (hDC == nullptr)
    return;

  RECT rc = {};
  ::GetClientRect(hWnd, &rc);
  rc.left = data.x;
  FillControlBackground(hWnd, hDC, rc);

  if (data.hIcon && data.size > 0)
    ::DrawIconEx(hDC, data.x, data.y, data.hIcon, data.size, data.size, 0,
                 nullptr, DI_NORMAL);

  if (data.text.empty())
    return;

  RECT rcText = rc;
  rcText.left = data.inset;

  HFONT hFont = reinterpret_cast<HFONT>(::SendMessageW(hWnd, WM_GETFONT, 0, 0));
  ScopedSelectObject<HFONT> selectedFont(hDC, hFont);

  int iSavedBkMode = ::SetBkMode(hDC, TRANSPARENT);
  int nIndex = ::IsWindowEnabled(hWnd) ? COLOR_WINDOWTEXT : COLOR_GRAYTEXT;
  COLORREF crOldText = ::SetTextColor(hDC, ::GetSysColor(nIndex));

  UINT uiState = static_cast<UINT>(::SendMessageW(hWnd, WM_QUERYUISTATE, 0, 0));
  UINT uTextFlags = DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS;
  if (uiState & UISF_HIDEACCEL)
    uTextFlags |= DT_HIDEPREFIX;

  ::DrawTextW(hDC, data.text.c_str(), -1, &rcText, uTextFlags);

  ::SetTextColor(hDC, crOldText);
  ::SetBkMode(hDC, iSavedBkMode);

  if (::GetFocus() == hWnd) {
    RECT rcFocus = rcText;
    UINT uMeasureFlags = DT_SINGLELINE | DT_CALCRECT;
    if (uiState & UISF_HIDEACCEL)
      uMeasureFlags |= DT_HIDEPREFIX;
    ::DrawTextW(hDC, data.text.c_str(), -1, &rcFocus, uMeasureFlags);
    int iFocusHeight = rcFocus.bottom - rcFocus.top;
    int iFocusTop = (rc.bottom - rc.top - iFocusHeight) / 2;
    rcFocus.left = data.x;
    rcFocus.top = (std::min)(iFocusTop, data.y);
    rcFocus.bottom =
        (std::max)(iFocusTop + iFocusHeight, data.y + data.size);
    ::InflateRect(&rcFocus, kScopeFocusPadding, kScopeFocusPadding);
    if (rcFocus.left < rc.left)
      rcFocus.left = rc.left;
    if (rcFocus.top < rc.top)
      rcFocus.top = rc.top;
    if (rcFocus.right > rc.right)
      rcFocus.right = rc.right;
    if (rcFocus.bottom > rc.bottom)
      rcFocus.bottom = rc.bottom;
    ::DrawFocusRect(hDC, &rcFocus);
  }
}

bool DrawBufferedScopeCheckbox(HWND hWnd,
                               ScopeCheckboxData *pData) noexcept {
  ScopedPaintSession paint(hWnd);
  if (!paint)
    return false;

  RECT rc = {};
  ::GetClientRect(hWnd, &rc);
  int iWidth = rc.right - rc.left;
  int iHeight = rc.bottom - rc.top;
  if (iWidth <= 0 || iHeight <= 0)
    return true;

  ScopedMemoryDCHandle hMemDC(::CreateCompatibleDC(paint.get()));
  if (!hMemDC)
    return false;
  ScopedBitmapHandle hBitmap(::CreateCompatibleBitmap(paint.get(), iWidth,
                                                      iHeight));
  if (!hBitmap)
    return false;
  ScopedSelectObject<HBITMAP> selectedBitmap(hMemDC.get(), hBitmap.get());
  if (!selectedBitmap)
    return false;

  FillControlBackground(hWnd, hMemDC.get(), rc);
  DrawScopeCheckboxGlyph(hWnd, hMemDC.get(), pData);
  if (pData)
    DrawScopeCheckboxExtras(hWnd, hMemDC.get(), *pData);

  ::BitBlt(paint.get(), 0, 0, iWidth, iHeight, hMemDC.get(), 0, 0, SRCCOPY);
  return true;
}

LRESULT CALLBACK
ScopeCheckboxProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                  UINT_PTR, DWORD_PTR dwRefData) noexcept {
  auto *pData = reinterpret_cast<ScopeCheckboxData *>(dwRefData);
  switch (uMsg) {
  case WM_GETTEXTLENGTH:
    if (pData)
      return static_cast<LRESULT>(pData->text.size());
    return 0;
  case WM_GETTEXT: {
    auto *wszDest = reinterpret_cast<LPWSTR>(lParam);
    int cchDest = static_cast<int>(wParam);
    if (!wszDest || cchDest <= 0)
      return 0;
    if (!pData) {
      *wszDest = L'\0';
      return 0;
    }
    ::lstrcpynW(wszDest, pData->text.c_str(), cchDest);
    return static_cast<LRESULT>(::lstrlenW(wszDest));
  }
  case WM_SETTEXT:
    if (pData) {
      pData->text = lParam ? reinterpret_cast<LPCWSTR>(lParam) : L"";
      ::InvalidateRect(hWnd, nullptr, TRUE);
    }
    return ::DefSubclassProc(hWnd, uMsg, wParam,
                             reinterpret_cast<LPARAM>(L""));
  case WM_ERASEBKGND:
    return TRUE;
  case WM_PRINTCLIENT: {
    HDC hDC = reinterpret_cast<HDC>(wParam);
    RECT rc = {};
    ::GetClientRect(hWnd, &rc);
    FillControlBackground(hWnd, hDC, rc);
    DrawScopeCheckboxGlyph(hWnd, hDC, pData);
    if (pData)
      DrawScopeCheckboxExtras(hWnd, hDC, *pData);
    return TRUE;
  }
  case WM_PAINT: {
    if (DrawBufferedScopeCheckbox(hWnd, pData))
      return 0;
    return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
  }
  case WM_ENABLE:
  case WM_SETFONT:
  case WM_THEMECHANGED:
  case WM_UPDATEUISTATE:
  case WM_SETFOCUS:
  case WM_KILLFOCUS:
    ::InvalidateRect(hWnd, nullptr, TRUE);
    break;
  case WM_MOUSEMOVE: {
    bool bStartedTracking = false;
    if (pData && !pData->trackingMouse) {
      TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, hWnd, 0};
      if (::TrackMouseEvent(&tme))
        pData->trackingMouse = true;
      bStartedTracking = pData->trackingMouse;
    }
    LRESULT lr = ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
    if (bStartedTracking)
      ::InvalidateRect(hWnd, nullptr, FALSE);
    return lr;
  }
  case WM_MOUSELEAVE: {
    LRESULT lr = ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
    if (pData) {
      pData->trackingMouse = false;
      ::InvalidateRect(hWnd, nullptr, FALSE);
    }
    return lr;
  }
  case WM_LBUTTONDOWN:
  case WM_LBUTTONUP:
  case WM_CAPTURECHANGED:
  case BM_SETCHECK:
  case BM_SETSTATE: {
    LRESULT lr = ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
    ::InvalidateRect(hWnd, nullptr, FALSE);
    return lr;
  }
  case WM_NCDESTROY:
    if (pData) {
      if (pData->hIcon)
        ::DestroyIcon(pData->hIcon);
      delete pData;
    }
    ::RemoveWindowSubclass(hWnd, ScopeCheckboxProc,
                           kScopeCheckboxSubclassId);
    break;
  }
  return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// This queues work after the current dispatch unwinds. `hAnchor` can be
// any BA-owned HWND. We post to its top-level window so
// CBalBaseBAFunctions::WndProc sees the message after thmutil finishes
// its own handling.
void PostRootMessage(HWND hAnchor, UINT uMsg) noexcept {
  if (!hAnchor)
    return;
  if (HWND hRoot = ::GetAncestor(hAnchor, GA_ROOT))
    ::PostMessageW(hRoot, uMsg, 0, 0);
}

void PostFocusRequest(HWND hAnchor, UINT uMsg) noexcept {
  PostRootMessage(hAnchor, uMsg);
}

// This computes the UAC shield edge length. We use the full font cell
// height so the shield has the same visual weight as the native check
// glyph, and we floor the value at SM_CYMENUCHECK so HiDPI and
// small-font configurations never render a shield thinner than the
// checkbox next to it.
int ShieldIconSize(HWND hWnd) noexcept {
  HFONT hFont = reinterpret_cast<HFONT>(::SendMessageW(hWnd, WM_GETFONT, 0, 0));
  ScopedWindowDC hDC(hWnd);
  if (!hDC)
    return ::GetSystemMetrics(SM_CYMENUCHECK);
  ScopedSelectObject<HFONT> selectedFont(hDC.get(), hFont);
  TEXTMETRICW tm = {};
  ::GetTextMetricsW(hDC.get(), &tm);
  int iCheck = ::GetSystemMetrics(SM_CYMENUCHECK);
  return tm.tmHeight > iCheck ? tm.tmHeight : iCheck;
}

// This paints the native UAC shield inside the InstallPerMachine checkbox.
// The shield is sized to match the visual weight of the native check glyph
// (font cell height, floored at SM_CYMENUCHECK) and painted between the
// check glyph and label text. The native label is cleared after BAF saves
// it, so localized strings stay natural in wxl while BAF controls the
// label/shield layout explicitly.
void PlaceShieldNear(HWND hCheckbox) {
  int iCell = ShieldIconSize(hCheckbox);

  RECT rc = {};
  ::GetClientRect(hCheckbox, &rc);
  int iY = (rc.bottom - rc.top - iCell) / 2;

  RECT rcGlyph = CheckboxGlyphRect(hCheckbox);
  int iTextStart = rcGlyph.right + kScopeGlyphGap;
  int iX = iTextStart;

  // LoadIconWithScaleDown scales to the requested size. LoadImage with
  // LR_SHARED would return the cached 32x32 copy, which would overflow.
  ScopedIconHandle hIcon;
  HICON hLoadedIcon = nullptr;
  ::LoadIconWithScaleDown(nullptr, IDI_SHIELD, iCell, iCell, &hLoadedIcon);
  hIcon.reset(hLoadedIcon);
  if (!hIcon)
    return;

  auto pData = std::unique_ptr<ScopeCheckboxData>(
      new (std::nothrow) ScopeCheckboxData());
  if (!pData)
    return;

  std::wstring wszText = CopyWindowText(hCheckbox);
  if (wszText.empty())
    return;

  pData->hIcon = hIcon.get();
  pData->text = wszText;
  pData->x = iX;
  pData->y = iY;
  pData->size = iCell;
  pData->inset = iX + iCell + kScopeShieldTextGap;

  ::SetWindowTextW(hCheckbox, L"");
  if (!::SetWindowSubclass(hCheckbox, ScopeCheckboxProc,
                           kScopeCheckboxSubclassId,
                           reinterpret_cast<DWORD_PTR>(pData.get()))) {
    ::SetWindowTextW(hCheckbox, wszText.c_str());
    return;
  }

  // The subclass now owns the icon and shield data through dwRefData.
  hIcon.release();
  pData.release();
  ::InvalidateRect(hCheckbox, nullptr, TRUE);
}

// This sizes each tab to the widest label in the current HiDPI-scaled
// font plus horizontal padding. It requires TCS_FIXEDWIDTH in the theme
// HexStyle.
void SizeTabsToWidestLabel(HWND hTabs) noexcept {
  HFONT hFont = reinterpret_cast<HFONT>(::SendMessageW(hTabs, WM_GETFONT, 0, 0));
  ScopedWindowDC hDC(hTabs);
  if (!hDC)
    return;
  ScopedSelectObject<HFONT> selectedFont(hDC.get(), hFont);
  int iMaxWidth = 0;
  int iCount = TabCtrl_GetItemCount(hTabs);
  for (int i = 0; i < iCount; ++i) {
    wchar_t wszBuf[128] = {};
    TCITEMW tci = {};
    tci.mask = TCIF_TEXT;
    tci.pszText = wszBuf;
    tci.cchTextMax = static_cast<int>(std::size(wszBuf));
    if (!TabCtrl_GetItem(hTabs, i, &tci))
      continue;
    SIZE sz = {};
    ::GetTextExtentPoint32W(hDC.get(), wszBuf, static_cast<int>(::wcslen(wszBuf)),
                            &sz);
    if (sz.cx > iMaxWidth)
      iMaxWidth = sz.cx;
  }

  RECT rcStrip = {};
  int iHeight = 22;
  if (TabCtrl_GetItemRect(hTabs, 0, &rcStrip))
    iHeight = rcStrip.bottom - rcStrip.top;
  TabCtrl_SetItemSize(hTabs, iMaxWidth + 32, iHeight);
}

// This clones `hWnd`'s current font at the requested weight. The caller
// owns the returned HFONT, and this function returns nullptr on failure.
ScopedFontHandle CloneFontAtWeight(HWND hWnd, LONG lWeight) noexcept {
  HFONT hFont = reinterpret_cast<HFONT>(::SendMessageW(hWnd, WM_GETFONT, 0, 0));
  if (!hFont)
    return {};
  LOGFONTW lf = {};
  if (!::GetObjectW(hFont, sizeof(lf), &lf))
    return {};
  lf.lfWeight = lWeight;
  return ScopedFontHandle(::CreateFontIndirectW(&lf));
}

// MARK: - SDK Tree Helpers

int SDKTreeRowHeight(HWND hTree) noexcept {
  SIZE szGlyph = CheckboxGlyphSize(hTree);
  int iHeight = szGlyph.cy + 2;

  HFONT hFont = reinterpret_cast<HFONT>(::SendMessageW(hTree, WM_GETFONT, 0, 0));
  ScopedWindowDC hDC(hTree);
  if (hDC) {
    ScopedSelectObject<HFONT> selectedFont(hDC.get(), hFont);
    TEXTMETRICW tm = {};
    if (::GetTextMetricsW(hDC.get(), &tm)) {
      int iTextHeight =
          static_cast<int>(tm.tmHeight + tm.tmExternalLeading + 3);
      iHeight = (std::max)(iHeight, iTextHeight);
    }
  }

  // TVM_SETITEMHEIGHT rounds odd values down, so request an even height.
  if (iHeight % 2)
    ++iHeight;
  return iHeight;
}

// This builds a three-entry state image list (blank, unchecked, and
// checked), sized to the themed checkbox glyph plus horizontal padding.
// `kLeftPad` separates the expand/collapse toggle from the checkbox, and
// `kRightPad` separates the checkbox from item text. Glyphs are drawn
// through the visual-styles Button class and fall back to DrawFrameControl
// on classic themes. The fill color matches the tree background so
// ImageList_AddMasked can make the padding transparent.
HIMAGELIST CreateTreeStateImages(HWND hTree) noexcept {
  SIZE szGlyph = CheckboxGlyphSize(hTree);
  int cxGlyph = szGlyph.cx;
  int cyGlyph = szGlyph.cy;
  HTHEME hTheme = ::OpenThemeData(hTree, L"Button");

  constexpr int kLeftPad = 7;
  constexpr int kRightPad = 10;
  int cxCell = cxGlyph + kLeftPad + kRightPad;
  int cyCell = cyGlyph;

  HIMAGELIST hListRaw =
      ::ImageList_Create(cxCell, cyCell, ILC_COLOR32 | ILC_MASK, 3, 0);
  if (!hListRaw) {
    if (hTheme)
      ::CloseThemeData(hTheme);
    return nullptr;
  }

  COLORREF crBg = TreeView_GetBkColor(hTree);
  if (crBg == static_cast<COLORREF>(-1))
    crBg = ::GetSysColor(COLOR_WINDOW);
  HBRUSH hBrushBg = ::CreateSolidBrush(crBg);
  if (!hBrushBg) {
    ::ImageList_Destroy(hListRaw);
    if (hTheme)
      ::CloseThemeData(hTheme);
    return nullptr;
  }

  HDC hDCScreen = ::GetDC(nullptr);
  if (!hDCScreen) {
    ::DeleteObject(hBrushBg);
    ::ImageList_Destroy(hListRaw);
    if (hTheme)
      ::CloseThemeData(hTheme);
    return nullptr;
  }
  HDC hDCMem = ::CreateCompatibleDC(hDCScreen);
  if (!hDCMem) {
    ::ReleaseDC(nullptr, hDCScreen);
    ::DeleteObject(hBrushBg);
    ::ImageList_Destroy(hListRaw);
    if (hTheme)
      ::CloseThemeData(hTheme);
    return nullptr;
  }

  auto paintEntry = [&](int iState) {
    HBITMAP hBitmap = ::CreateCompatibleBitmap(hDCScreen, cxCell, cyCell);
    if (!hBitmap)
      return;
    HBITMAP hOld = static_cast<HBITMAP>(::SelectObject(hDCMem, hBitmap));
    if (!hOld) {
      ::DeleteObject(hBitmap);
      return;
    }
    RECT rcCell = {0, 0, cxCell, cyCell};
    ::FillRect(hDCMem, &rcCell, hBrushBg);
    if (iState) {
      RECT rcGlyph = {kLeftPad, 0, kLeftPad + cxGlyph, cyGlyph};
      if (hTheme) {
        ::DrawThemeBackground(hTheme, hDCMem, BP_CHECKBOX, iState, &rcGlyph,
                              nullptr);
      } else {
        UINT uFlags = DFCS_BUTTONCHECK;
        if (iState == CBS_CHECKEDNORMAL)
          uFlags |= DFCS_CHECKED;
        ::DrawFrameControl(hDCMem, &rcGlyph, DFC_BUTTON, uFlags);
      }
    }
    ::SelectObject(hDCMem, hOld);
    if (::ImageList_AddMasked(hListRaw, hBitmap, crBg) == -1) {
      // Keep painting subsequent entries to avoid partial state mismatch.
    }
    ::DeleteObject(hBitmap);
  };

  paintEntry(0);                    // Index 0 is blank (no checkbox).
  paintEntry(CBS_UNCHECKEDNORMAL);  // Index 1 is unchecked.
  paintEntry(CBS_CHECKEDNORMAL);    // Index 2 is checked.

  ::DeleteDC(hDCMem);
  ::ReleaseDC(nullptr, hDCScreen);
  ::DeleteObject(hBrushBg);
  if (hTheme)
    ::CloseThemeData(hTheme);

  return hListRaw;
}

// MARK: - Options Page Helpers

void InstallTabTooltips(WIX_LOCALIZATION const *pWixLoc, HWND hTabs,
                        std::array<ScopedWixString, 3> *pTabTooltips) noexcept {
  static constexpr LPCWSTR kTabTooltipLocStrings[] = {
      L"#(loc.OptionsToolchainTabTooltip)",
      L"#(loc.OptionsRuntimesTabTooltip)",
      L"#(loc.OptionsSDKsTabTooltip)",
  };

  if (hTabs == nullptr || pTabTooltips == nullptr)
    return;

  HWND hTT = ::CreateWindowExW(WS_EX_TOPMOST, TOOLTIPS_CLASSW, nullptr,
                               WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
                               CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                               CW_USEDEFAULT, hTabs, nullptr, nullptr, nullptr);
  if (!hTT)
    return;
  ::SendMessageW(hTabs, TCM_SETTOOLTIPS, reinterpret_cast<WPARAM>(hTT), 0);

  int iCount = TabCtrl_GetItemCount(hTabs);
  int iLimit =
      (std::min)(iCount, static_cast<int>(std::size(kTabTooltipLocStrings)));
  for (int i = 0; i < iLimit; ++i) {
    RECT rc = {};
    if (!TabCtrl_GetItemRect(hTabs, i, &rc))
      continue;
    (*pTabTooltips)[i] =
        LocalizeStringScoped(pWixLoc, kTabTooltipLocStrings[i]);
    TOOLINFOW ti = {};
    ti.cbSize = sizeof(ti);
    ti.uFlags = TTF_SUBCLASS;
    ti.hwnd = hTabs;
    ti.uId = static_cast<UINT_PTR>(i);
    ti.rect = rc;
    ti.lpszText = (*pTabTooltips)[i].get();
    ::SendMessageW(hTT, TTM_ADDTOOL, 0, reinterpret_cast<LPARAM>(&ti));
  }
}

void DrawTabItem(DRAWITEMSTRUCT *pdis, HFONT hTabLabelFont) noexcept {
  HDC hDC = pdis->hDC;
  bool bSelected = (pdis->itemState & ODS_SELECTED) != 0;

  ::FillRect(hDC, &pdis->rcItem, reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1));
  if (bSelected) {
    // The accent sits at the top. The body frame's top edge already
    // separates the strip from the body, so a bottom accent would stack
    // into a raised-button shadow.
    RECT rcAccent = pdis->rcItem;
    rcAccent.bottom = rcAccent.top + 5;
    ::FillRect(hDC, &rcAccent, reinterpret_cast<HBRUSH>(COLOR_HOTLIGHT + 1));
  }

  wchar_t wszBuf[128] = {};
  TCITEMW tci = {};
  tci.mask = TCIF_TEXT;
  tci.pszText = wszBuf;
  tci.cchTextMax = static_cast<int>(std::size(wszBuf));
  if (!TabCtrl_GetItem(pdis->hwndItem, pdis->itemID, &tci))
    return;

  HFONT hFont = hTabLabelFont;
  if (!hFont)
    hFont =
        reinterpret_cast<HFONT>(::SendMessageW(pdis->hwndItem, WM_GETFONT, 0,
                                               0));
  HFONT hOldFont = static_cast<HFONT>(::SelectObject(hDC, hFont));
  int iOldBkMode = ::SetBkMode(hDC, TRANSPARENT);
  int iTextColor = bSelected ? COLOR_WINDOWTEXT : COLOR_GRAYTEXT;
  COLORREF crOldText = ::SetTextColor(hDC, ::GetSysColor(iTextColor));
  ::DrawTextW(hDC, wszBuf, -1, &pdis->rcItem,
              DT_CENTER | DT_SINGLELINE | DT_VCENTER);
  ::SetTextColor(hDC, crOldText);
  ::SetBkMode(hDC, iOldBkMode);
  ::SelectObject(hDC, hOldFont);

  if (pdis->itemState & ODS_FOCUS)
    ::DrawFocusRect(hDC, &pdis->rcItem);
}

template <typename Engine>
void PopulateDefaultToolchainCombo(Engine *pEngine,
                                   WIX_LOCALIZATION const *pWixLoc,
                                   HWND hCombo) noexcept {
  if (pEngine == nullptr || hCombo == nullptr)
    return;

  LONGLONG llIncludeNoAsserts = 0;
  pEngine->GetVariableNumeric(kIncludeNoAssertsVariable, &llIncludeNoAsserts);
  ::SendMessageW(hCombo, CB_RESETCONTENT, 0, 0);

  auto addVariantRow = [&](int iVariant) {
    ScopedWixString wszLabel = LocalizeStringScoped(
        pWixLoc, kToolchainVariantLabelLocStrings[iVariant]);
    LPCWSTR wszDisplayLabel =
        wszLabel ? wszLabel.get() : kToolchainVariantValues[iVariant];
    LRESULT iRow = ::SendMessageW(hCombo, CB_ADDSTRING, 0,
                                  reinterpret_cast<LPARAM>(wszDisplayLabel));
    if (iRow != CB_ERR && iRow != CB_ERRSPACE)
      ::SendMessageW(hCombo, CB_SETITEMDATA, iRow, iVariant);
  };

  addVariantRow(kToolchainVariantAsserts);
  if (llIncludeNoAsserts)
    addVariantRow(kToolchainVariantNoAsserts);

  // This reads OptionsDefaultToolchain and selects the matching item.
  // It falls back to index 0 when the value is unset or unavailable
  // (for example, when the variable is "NoAsserts" but only "Asserts"
  // is packaged).
  ScopedWixString wszValue =
      GetBalStringVariableScoped(kDefaultToolchainCombo);
  int iVariant = kToolchainVariantAsserts;
  if (wszValue)
    iVariant = ToolchainVariantIndexFromValue(wszValue.get());
  if (!SelectToolchainVariantInCombo(hCombo, iVariant))
    SelectToolchainVariantInCombo(hCombo, kToolchainVariantAsserts);
}

template <typename Engine>
void HandleDefaultToolchainChange(Engine *pEngine, HWND hCombo) noexcept {
  if (pEngine == nullptr || hCombo == nullptr)
    return;

  int iSel = static_cast<int>(::SendMessageW(hCombo, CB_GETCURSEL, 0, 0));
  if (iSel < 0)
    return;
  int iVariant =
      static_cast<int>(::SendMessageW(hCombo, CB_GETITEMDATA, iSel, 0));
  if (iVariant < 0 ||
      iVariant >= static_cast<int>(std::size(kToolchainVariantValues)))
    return;
  pEngine->SetVariableString(kDefaultToolchainCombo,
                             kToolchainVariantValues[iVariant], FALSE);
}

class SwiftBAFunctions : public CBalBaseBAFunctions {
  WIX_LOCALIZATION *pWixLoc_ = nullptr;
  bool bXmlInitialized_ = false;
  HWND hInstallRoot_ = nullptr;
  HWND hScopeHint_ = nullptr;
  HWND hTabs_ = nullptr;
  HWND hRefreshTrigger_ = nullptr;
  HWND hOptionsButton_ = nullptr;
  // This maps Alt+mnemonic keys to tab indexes on the Options page.
  TabMnemonicMap optionsTabMnemonics_;
  HWND hSDKTree_ = nullptr;
  HWND hDefaultToolchainCombo_ = nullptr;
  ScopedImageListHandle hSDKStateImages_;
  bool bSuppressSDKTreeNotify_ = false;
  std::array<HTREEITEM, std::size(kSDKTreeItems)> aSDKTreeItems_ = {};
  // This semibold font is used for owner-drawn tab labels.
  ScopedFontHandle hTabLabelFont_;
  // This stores per-tab tooltip text. TTM_ADDTOOL does not copy
  // `lpszText`, so BAF must retain these strings for the process lifetime.
  std::array<ScopedWixString, 3> apwszTabTooltips_ = {};
  // This snapshot is captured when OptionsButton is clicked and restored
  // when OptionsCancelButton is clicked. It covers variables that BAF
  // manages manually, while thmutil handles its own auto-bound controls.
  std::array<LONGLONG, std::size(kSDKTreeItems)> SDKSnapshot_ = {};
  ScopedWixString wszDefaultToolchainSnapshot_;

public:
  SwiftBAFunctions(HMODULE hModule) : CBalBaseBAFunctions(hModule) {}
  ~SwiftBAFunctions() override { UninitializeLocalization(); }

  // MARK: - BA Entry Points

  STDMETHODIMP OnCreate(IBootstrapperEngine *pEngine,
                        BOOTSTRAPPER_COMMAND *pCommand) override {
    HRESULT hr = CBalBaseBAFunctions::OnCreate(pEngine, pCommand);
    LPWSTR wszLocPath = nullptr;
    ExitOnFailure(hr, "Failed to initialize BAFunctions.");

    hr = XmlInitialize();
    ExitOnFailure(hr, "Failed to initialize XML for BA localization.");
    bXmlInitialized_ = true;

    hr = PathRelativeToModule(&wszLocPath, kLocalizationFile, m_hModule);
    ExitOnFailure(hr, "Failed to locate the BA localization file.");

    hr = LocLoadFromFile(wszLocPath, &pWixLoc_);
    ExitOnFailure(hr, "Failed to load the BA localization file.");

  LExit:
    ReleaseStr(wszLocPath);
    return hr;
  }

  STDMETHODIMP OnDestroy(BOOL fReload) override {
    UninitializeLocalization();
    return CBalBaseBAFunctions::OnDestroy(fReload);
  }

  STDMETHODIMP OnThemeControlLoaded(LPCWSTR wszName, WORD, HWND hWnd,
                                    BOOL *) override {
    if (IsControl(wszName, kScopeControl)) {
      PlaceShieldNear(hWnd);
      return S_OK;
    }
    if (IsControl(wszName, kInstallRoot)) {
      hInstallRoot_ = hWnd;
      // The initial page at launch is Install. We focus the edit box
      // after the window is shown by posting a message, so SetFocus runs
      // only after the page becomes visible.
      PostFocusRequest(hWnd, kMsgFocusInstallRoot);
      return S_OK;
    }
    if (IsControl(wszName, kScopeHintControl)) {
      hScopeHint_ = hWnd;
      return S_OK;
    }
    if (IsControl(wszName, kOptionsTabs)) {
      hTabs_ = hWnd;
      SizeTabsToWidestLabel(hWnd);
      hTabLabelFont_ = CloneFontAtWeight(hWnd, FW_MEDIUM);
      BuildTabMnemonicMap(hWnd, &optionsTabMnemonics_);
      ::SetWindowSubclass(hWnd, OptionsTabsMnemonicProc,
                          kOptionsTabsSubclassId,
                          reinterpret_cast<DWORD_PTR>(&optionsTabMnemonics_));
      InstallTabTooltips(pWixLoc_, hWnd, &apwszTabTooltips_);
      return S_OK;
    }
    if (IsControl(wszName, kRefreshTrigger)) {
      hRefreshTrigger_ = hWnd;
      return S_OK;
    }
    if (IsControl(wszName, kOptionsButton)) {
      hOptionsButton_ = hWnd;
      ::EnableWindow(hWnd, TRUE);
      return S_OK;
    }
    if (IsControl(wszName, kDefaultToolchainCombo)) {
      hDefaultToolchainCombo_ = hWnd;
      PopulateDefaultToolchainCombo(m_pEngine, pWixLoc_, hWnd);
      return S_OK;
    }
    if (IsControl(wszName, kSDKTree)) {
      hSDKTree_ = hWnd;
      SetupSDKTree(hWnd);
    }
    if (ShouldInstallOptionsCtrlTabRelay(wszName))
      ::SetWindowSubclass(hWnd, OptionsCtrlTabRelayProc,
                          kOptionsCtrlTabSubclassId, 0);
    return S_OK;
  }

  // This handler dispatches WM_COMMAND and WM_NOTIFY events. WiX 7.0.0
  // currently misroutes the NOTIFY proc slot through WMCOMMAND, so
  // notify-routed events (such as TCN_SELCHANGE and TVN_ITEMCHANGED)
  // arrive here with `wParam` containing an LPNMHDR instead of the packed
  // notification code and control ID. We dispatch by `wszName` and then
  // interpret `wParam` accordingly.
  STDMETHODIMP OnThemeControlWmCommand(WPARAM wParam, LPCWSTR wszName, WORD,
                                       HWND hWnd, BOOL *, LRESULT *) override {
    if (IsControl(wszName, kOptionsTabs)) {
      LPNMHDR pnmhdr = reinterpret_cast<LPNMHDR>(wParam);
      if (pnmhdr && pnmhdr->code == TCN_SELCHANGE)
        ApplyTabSelection();
      return S_OK;
    }
    if (IsControl(wszName, kSDKTree)) {
      LPNMHDR pnmhdr = reinterpret_cast<LPNMHDR>(wParam);
      if (pnmhdr && pnmhdr->code == TVN_ITEMCHANGEDW)
        HandleSDKTreeItemChanged(reinterpret_cast<NMTVITEMCHANGE *>(pnmhdr));
      return S_OK;
    }
    if (IsControl(wszName, kDefaultToolchainCombo)) {
      if (HIWORD(wParam) == CBN_SELCHANGE)
        HandleDefaultToolchainChange(m_pEngine, hWnd);
      return S_OK;
    }
    if (IsControl(wszName, kScopeControl)) {
      if (IsButtonClicked(wParam))
        HandleScopeToggle(hWnd);
      return S_OK;
    }
    if (IsControl(wszName, kOptionsButton)) {
      if (IsButtonClicked(wParam) && ::IsWindowVisible(hWnd) &&
          ::IsWindowEnabled(hWnd)) {
        SnapshotOptionsState();
        SetOptionsButtonEnabled(false);
        PostFocusRequest(hWnd, kMsgFocusOptionsTabs);
      }
      return S_OK;
    }
    if (IsControl(wszName, kOptionsOkButton)) {
      if (IsButtonClicked(wParam)) {
        SetOptionsButtonEnabled(true);
        PostFocusRequest(hWnd, kMsgFocusInstallRoot);
      }
      return S_OK;
    }
    if (IsControl(wszName, kOptionsCancelButton)) {
      if (IsButtonClicked(wParam)) {
        SetOptionsButtonEnabled(true);
        RestoreOptionsState();
        PostFocusRequest(hWnd, kMsgFocusInstallRoot);
      }
      return S_OK;
    }
    return S_OK;
  }

  // This handles deferred focus-request messages (kMsgFocus*) and
  // WM_DRAWITEM for the owner-drawn tab strip. All other messages fall
  // through to the BA's default handling.
  STDMETHODIMP WndProc(HWND, UINT uMsg, WPARAM wParam, LPARAM lParam,
                       BOOL *pfProcessed, LRESULT *plResult) override {
    if (pfProcessed)
      *pfProcessed = FALSE;
    if (plResult)
      *plResult = 0;

    switch (uMsg) {
    case kMsgFocusInstallRoot: {
      HWND hTarget = hInstallRoot_;
      if (hTarget)
        ::SetFocus(hTarget);
      if (pfProcessed)
        *pfProcessed = TRUE;
      return S_OK;
    }
    case kMsgFocusOptionsTabs: {
      ApplyTabSelection();
      HWND hTarget = hTabs_;
      if (hTarget)
        ::SetFocus(hTarget);
      if (pfProcessed)
        *pfProcessed = TRUE;
      return S_OK;
    }
    case kMsgApplyOptionsTabSelection:
      ApplyTabSelection();
      if (pfProcessed)
        *pfProcessed = TRUE;
      return S_OK;
    case kMsgCycleOptionsTabs: {
      bool bReverse = wParam != 0;
      bool bHandled = SelectAdjacentOptionsTab(hTabs_, bReverse);
      if (pfProcessed)
        *pfProcessed = TRUE;
      if (plResult)
        *plResult = bHandled ? TRUE : FALSE;
      return S_OK;
    }
    case kMsgRedrawInstallScope:
      if (hInstallRoot_)
        RedrawControlNow(hInstallRoot_);
      if (hScopeHint_)
        RedrawControlNow(hScopeHint_);
      if (pfProcessed)
        *pfProcessed = TRUE;
      return S_OK;
    case WM_DRAWITEM: {
      DRAWITEMSTRUCT *pdis = reinterpret_cast<DRAWITEMSTRUCT *>(lParam);
      if (!pdis || pdis->CtlType != ODT_TAB || !hTabs_ ||
          pdis->hwndItem != hTabs_)
        return S_OK;
      DrawTabItem(pdis, hTabLabelFont_.get());
      if (pfProcessed)
        *pfProcessed = TRUE;
      if (plResult)
        *plResult = TRUE;
      return S_OK;
    }
    default:
      return S_OK;
    }
  }

private:
  void UninitializeLocalization() noexcept {
    LocFree(pWixLoc_);
    pWixLoc_ = nullptr;
    if (bXmlInitialized_) {
      XmlUninitialize();
      bXmlInitialized_ = false;
    }
  }

  // MARK: - Install Page (UAC Shield + Scope Toggle)

  // This saves the edit box text under the departing scope's shadow
  // variable and restores the arriving scope's remembered path into
  // InstallRoot and the edit box HWND. thmutil only flushes edit box text
  // to its bound variable during page navigation. thmutil's
  // OnButtonClicked handler runs after this dispatch, so we read the
  // checkbox state directly with BM_GETCHECK instead of using the stale
  // bundle variable.
  void HandleScopeToggle(HWND hCheckbox) {
    bool bPerMachine =
        ::SendMessageW(hCheckbox, BM_GETCHECK, 0, 0) == BST_CHECKED;
    BalLog(BOOTSTRAPPER_LOG_LEVEL_STANDARD, "baf: scope %ls",
           bPerMachine ? L"per-machine" : L"per-user");
    LPCWSTR wszOldShadow = bPerMachine ? kPerUserShadow : kPerMachineShadow;
    LPCWSTR wszNewShadow = bPerMachine ? kPerMachineShadow : kPerUserShadow;

    if (hInstallRoot_) {
      std::wstring wszInstallRoot = CopyWindowText(hInstallRoot_);
      m_pEngine->SetVariableString(wszOldShadow, wszInstallRoot.c_str(),
                                   FALSE);
    }

    // Shadow defaults are formatted strings (for example,
    // `[LocalAppDataFolder]Programs\Swift`). We expand the value before
    // storing it so InstallRoot contains a literal path.
    ScopedWixString wszValue = FormatWixVariable(wszNewShadow);
    if (wszValue) {
      m_pEngine->SetVariableString(kInstallRoot, wszValue.get(), FALSE);
      if (hInstallRoot_) {
        ::SetWindowTextW(hInstallRoot_, wszValue.get());
        RedrawControlNow(hInstallRoot_);
      }
    }
    PostRootMessage(hInstallRoot_ ? hInstallRoot_ : hCheckbox,
                    kMsgRedrawInstallScope);
  }

  // MARK: - Options Page (Tab Strip)

  void SetOptionsButtonEnabled(bool bEnabled) noexcept {
    if (!hOptionsButton_)
      return;
    ::EnableWindow(hOptionsButton_, bEnabled ? TRUE : FALSE);
  }

  // This updates OptionsTab and synthesizes a click on the hidden
  // BAFRefreshTrigger checkbox so thmutil's OnButtonClicked path runs
  // ThemeShowPageEx(REFRESH). That refresh re-evaluates every
  // VisibleCondition and EnableCondition on the page against the new
  // OptionsTab value.
  void ApplyTabSelection() {
    int iSel = hTabs_ ? TabCtrl_GetCurSel(hTabs_) : 0;
    m_pEngine->SetVariableNumeric(kOptionsTabVariable, iSel);
    if (hRefreshTrigger_ == nullptr)
      return;
    HWND hParent = ::GetParent(hRefreshTrigger_);
    LONG lId = ::GetDlgCtrlID(hRefreshTrigger_);
    ::SendMessageW(hParent, WM_COMMAND,
                   MAKEWPARAM(lId, BN_CLICKED),
                   reinterpret_cast<LPARAM>(hRefreshTrigger_));
  }

  // MARK: - Options Page (SDK TreeView)

  // This populates the SDK TreeView with the Platform/SDK/Redistributable
  // hierarchy and synchronizes each row's check state from its bundle
  // variable. The tree starts collapsed, and users can expand platforms
  // as needed.
  void SetupSDKTree(HWND hTree) {
    bSuppressSDKTreeNotify_ = true;
    aSDKTreeItems_.fill(nullptr);

    TreeView_SetIndent(hTree, 36);

    // TVS_CHECKBOXES is documented to create a state image list with
    // blank, unchecked, and checked entries. On recent Windows builds, no
    // list is attached, so the tree has nothing to render for the
    // state-image indices we set. We create our own list with horizontal
    // padding so labels do not need a leading-space workaround.
    hSDKStateImages_.reset(CreateTreeStateImages(hTree));
    if (hSDKStateImages_)
      TreeView_SetImageList(hTree, hSDKStateImages_.get(), TVSIL_STATE);

    // For each row, this checks the BAFAvail* variable declared in
    // installer.wxs. Build-time settings for IncludeWindows,
    // IncludeAndroid, and per-architecture flags drive those values. We
    // skip rows that are not packaged, and a skipped parent clears its
    // depth anchor so descendants are skipped too.
    std::array<HTREEITEM, 3> hDepthAnchor = {TVI_ROOT, nullptr, nullptr};
    for (size_t iSDKItem = 0; iSDKItem < std::size(kSDKTreeItems); ++iSDKItem) {
      SDKTreeItem const &item = kSDKTreeItems[iSDKItem];
      if (item.depth > 0 && !hDepthAnchor[item.depth - 1])
        continue;

      std::wstring availability = SDKTreeAvailability(item);
      LONGLONG llAvail = 0;
      m_pEngine->GetVariableNumeric(availability.c_str(), &llAvail);
      if (!llAvail) {
        hDepthAnchor[item.depth] = nullptr;
        continue;
      }

      HTREEITEM hParent = item.depth == 0
                              ? TVI_ROOT
                              : hDepthAnchor[item.depth - 1];

      std::wstring labelLocString = SDKTreeLabelLocString(item);
      ScopedWixString wszLabel =
          LocalizeStringScoped(pWixLoc_, labelLocString.c_str());

      LONGLONG llVal = 0;
      std::wstring variable = SDKTreeVariable(item);
      m_pEngine->GetVariableNumeric(variable.c_str(), &llVal);

      // TVS_CHECKBOXES only assigns a state image to items inserted with
      // TVIF_STATE set. Otherwise, the item state remains 0 (no
      // checkbox), and a later TreeView_SetCheckState call has no effect.
      TVINSERTSTRUCTW tvis = {};
      tvis.hParent = hParent;
      tvis.hInsertAfter = TVI_LAST;
      tvis.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_STATE;
      tvis.item.pszText = wszLabel
                              ? wszLabel.get()
                              : const_cast<LPWSTR>(labelLocString.c_str());
      tvis.item.lParam = static_cast<LPARAM>(iSDKItem);
      tvis.item.state =
          INDEXTOSTATEIMAGEMASK(llVal ? kTreeStateImageChecked
                                      : kTreeStateImageUnchecked);
      tvis.item.stateMask = TVIS_STATEIMAGEMASK;

      HTREEITEM hItem = TreeView_InsertItem(hTree, &tvis);
      aSDKTreeItems_[iSDKItem] = hItem;
      hDepthAnchor[item.depth] = hItem;
    }

    TreeView_SetItemHeight(hTree, SDKTreeRowHeight(hTree));

    bSuppressSDKTreeNotify_ = false;
  }

  // This handles TVN_ITEMCHANGED notifications from the SDK tree. When a
  // checkbox state changes, it writes the backing variable. When a row is
  // unchecked, it cascades the unchecked state to descendants so children
  // cannot be installed without their parent platform or SDK.
  void HandleSDKTreeItemChanged(NMTVITEMCHANGE const *pInfo) {
    if (pInfo == nullptr || bSuppressSDKTreeNotify_ ||
        !(pInfo->uChanged & TVIF_STATE))
      return;
    UINT uOld = GetTreeStateImageIndex(pInfo->uStateOld);
    UINT uNew = GetTreeStateImageIndex(pInfo->uStateNew);
    if (uOld == uNew)
      return;

    TVITEMW tvi = {};
    tvi.mask = TVIF_PARAM;
    tvi.hItem = pInfo->hItem;
    if (!TreeView_GetItem(hSDKTree_, &tvi))
      return;
    size_t idx = static_cast<size_t>(tvi.lParam);
    if (idx >= std::size(kSDKTreeItems))
      return;

    bool bChecked = uNew == kTreeStateImageChecked;
    std::wstring variable = SDKTreeVariable(kSDKTreeItems[idx]);
    m_pEngine->SetVariableNumeric(variable.c_str(), bChecked ? 1 : 0);

    if (bChecked) {
      // Checking a child implicitly enables its parent, so the tree never
      // reaches a state where a descendant is checked under an unchecked
      // parent. TreeView_SetCheckState triggers another TVN_ITEMCHANGED
      // notification, so recursion naturally continues to the root.
      HTREEITEM hParent = TreeView_GetParent(hSDKTree_, pInfo->hItem);
      if (hParent)
        TreeView_SetCheckState(hSDKTree_, hParent, TRUE);
    } else {
      CascadeUncheckChildren(pInfo->hItem);
    }
  }

  // This walks a tree item's direct children and clears their check
  // state. Each TreeView_SetCheckState call triggers TVN_ITEMCHANGED
  // again, so recursion naturally continues through deeper levels.
  void CascadeUncheckChildren(HTREEITEM hItem) {
    HTREEITEM hChild = TreeView_GetChild(hSDKTree_, hItem);
    while (hChild) {
      TreeView_SetCheckState(hSDKTree_, hChild, FALSE);
      hChild = TreeView_GetNextSibling(hSDKTree_, hChild);
    }
  }

  // MARK: - Options Page (Cancel/OK Snapshot and Restore)

  // This records the current Options page state so OptionsCancelButton
  // can restore it. It covers variables that BAF manages manually, such
  // as SDK tree rows and the default-toolchain combobox. thmutil already
  // saves and restores auto-bound checkboxes and edit boxes with its own
  // snapshot.
  void SnapshotOptionsState() {
    for (size_t iSDKItem = 0; iSDKItem < std::size(kSDKTreeItems); ++iSDKItem) {
      SDKTreeItem const &item = kSDKTreeItems[iSDKItem];
      SDKSnapshot_[iSDKItem] = 0;
      std::wstring variable = SDKTreeVariable(item);
      m_pEngine->GetVariableNumeric(variable.c_str(), &SDKSnapshot_[iSDKItem]);
    }
    wszDefaultToolchainSnapshot_ =
        GetBalStringVariableScoped(kDefaultToolchainCombo);
  }

  // This rolls BAF-managed Options page variables back to the snapshot
  // taken when the page was entered, then drives the tree and combobox to
  // match so the UI remains synchronized with the restored state.
  void RestoreOptionsState() {
    bSuppressSDKTreeNotify_ = true;
    for (size_t iSDKItem = 0; iSDKItem < std::size(kSDKTreeItems); ++iSDKItem) {
      SDKTreeItem const &item = kSDKTreeItems[iSDKItem];
      std::wstring variable = SDKTreeVariable(item);
      m_pEngine->SetVariableNumeric(variable.c_str(), SDKSnapshot_[iSDKItem]);
      if (aSDKTreeItems_[iSDKItem])
        TreeView_SetCheckState(hSDKTree_, aSDKTreeItems_[iSDKItem],
                               SDKSnapshot_[iSDKItem] != 0);
    }
    if (wszDefaultToolchainSnapshot_) {
      m_pEngine->SetVariableString(kDefaultToolchainCombo,
                                   wszDefaultToolchainSnapshot_.get(),
                                   FALSE);
      if (hDefaultToolchainCombo_) {
        int iVariant = ToolchainVariantIndexFromValue(
            wszDefaultToolchainSnapshot_.get());
        SelectToolchainVariantInCombo(hDefaultToolchainCombo_, iVariant);
      }
    }

    bSuppressSDKTreeNotify_ = false;
  }
};

} // namespace

extern "C" {

BOOL WINAPI
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID) {
  switch (dwReason) {
  case DLL_PROCESS_ATTACH:
    ::DisableThreadLibraryCalls(hInstance);
    vhInstance = hInstance;
    break;
  case DLL_PROCESS_DETACH:
    vhInstance = nullptr;
    break;
  }
  return TRUE;
}

__declspec(dllexport)
HRESULT WINAPI
BAFunctionsCreate(const BA_FUNCTIONS_CREATE_ARGS *pArgs,
                  BA_FUNCTIONS_CREATE_RESULTS *pResults) {
  HRESULT hr = S_OK;
  SwiftBAFunctions *pBAFunctions = nullptr;

  BalInitialize(pArgs->pEngine);
  BalLog(BOOTSTRAPPER_LOG_LEVEL_STANDARD, "baf: initialized");

  pBAFunctions = new SwiftBAFunctions(vhInstance);
  ExitOnNull(pBAFunctions, hr, E_OUTOFMEMORY,
             "Failed to allocate SwiftBAFunctions.");

  hr = pBAFunctions->OnCreate(pArgs->pEngine, pArgs->pCommand);
  ExitOnFailure(hr, "SwiftBAFunctions OnCreate failed.");

  pResults->pfnBAFunctionsProc = BalBaseBAFunctionsProc;
  pResults->pvBAFunctionsProcContext = pBAFunctions;
  pBAFunctions = nullptr;

LExit:
  ReleaseObject(pBAFunctions);
  return hr;
}

__declspec(dllexport)
void WINAPI
BAFunctionsDestroy(const BA_FUNCTIONS_DESTROY_ARGS *,
                   BA_FUNCTIONS_DESTROY_RESULTS *) {
  BalUninitialize();
}

} // extern "C"
