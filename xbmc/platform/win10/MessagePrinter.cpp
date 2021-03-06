/*
 *      Copyright (C) 2005-2017 Team Kodi
 *      http://kodi.tv
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "platform/MessagePrinter.h"
#include "CompileInfo.h"
#include "utils/log.h"
#include "platform/win32/CharsetConverter.h"
#include "rendering/dx/DeviceResources.h"
#include "windowing/win10/WinSystemWin10DX.h"

int WINAPI MessageBox(void* hWnd, const char* lpText, const char* lpCaption, UINT uType)
{
  Windows::UI::Core::CoreWindow^ coreWindow = g_Windowing.GetCoreWindow();
  if (!coreWindow)
    return IDOK;

  auto wText = KODI::PLATFORM::WINDOWS::ToW(lpText);
  auto wCaption = KODI::PLATFORM::WINDOWS::ToW(lpCaption);

  auto handler = ref new Windows::UI::Core::DispatchedHandler([wText, wCaption]()
  {
    auto message = ref new Platform::String(wText.c_str());
    auto title = ref new Platform::String(wCaption.c_str());
    // Show the message dialog
    auto msg = ref new Windows::UI::Popups::MessageDialog(message, title);
    // Set the command to be invoked when a user presses 'ESC'
    msg->CancelCommandIndex = 1;
    msg->ShowAsync();
  });

  if (coreWindow->Dispatcher->HasThreadAccess)
    handler->Invoke();
  else
    coreWindow->Dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, handler);

  return IDOK;
}

void CMessagePrinter::DisplayMessage(const std::string& message)
{
  MessageBox(NULL, message.c_str(), CCompileInfo::GetAppName(), MB_OK | MB_ICONINFORMATION);
}

void CMessagePrinter::DisplayWarning(const std::string& warning)
{
  MessageBox(NULL, warning.c_str(), CCompileInfo::GetAppName(), MB_OK | MB_ICONWARNING);
}

void CMessagePrinter::DisplayError(const std::string& error)
{
  MessageBox(NULL, error.c_str(), CCompileInfo::GetAppName(), MB_OK | MB_ICONERROR);
}

void CMessagePrinter::DisplayHelpMessage(const std::vector<std::pair<std::string, std::string>>& help)
{
  //very crude implementation, pretty it up when possible
  std::string message;
  for (const auto& line : help)
  {
    message.append(line.first + "\t" + line.second + "\r\n");
  }

  MessageBox(NULL, message.c_str(), CCompileInfo::GetAppName(), MB_OK | MB_ICONINFORMATION);
}