/* ReaPack: Package manager for REAPER
 * Copyright (C) 2015-2017  Christian Fillion
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef REAPACK_IMPORT_HPP
#define REAPACK_IMPORT_HPP

#include "dialog.hpp"

#include "encoding.hpp"
#include "remote.hpp"

#include <queue>
#include <string>

class MemoryDownload;
class ReaPack;
class ThreadPool;

class Import : public Dialog
{
public:
  Import(ReaPack *);

protected:
  void onInit() override;
  void onCommand(int, int) override;
  void onTimer(int) override;

private:
  enum State {
    OK,
    Aborted,
    Close,
  };

  struct ImportData { Remote remote; std::string contents; };

  ThreadPool *setupPool();
  void fetch();
  bool read(MemoryDownload *);
  void processQueue();
  bool import(const ImportData &);
  void setWaiting(bool);

  ReaPack *m_reapack;
  ThreadPool *m_pool;
  State m_state;
  std::queue<ImportData> m_queue;
  short m_fakePos;

  HWND m_url;
  HWND m_progress;
  HWND m_discover;
};

#endif
