/* ReaPack: Package manager for REAPER
 * Copyright (C) 2015-2016  Christian Fillion
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

#ifndef REAPACK_TRANSACTION_HPP
#define REAPACK_TRANSACTION_HPP

#include "download.hpp"
#include "path.hpp"
#include "registry.hpp"

#include <boost/signals2.hpp>
#include <functional>
#include <set>

class InstallTask;
class Remote;
class RemoteIndex;
class RemoteList;
class RemoveTask;
class Task;

class Transaction {
public:
  typedef boost::signals2::signal<void ()> Signal;
  typedef Signal::slot_type Callback;

  typedef std::pair<const Version *, const Registry::QueryResult> InstallTicket;
  typedef std::vector<InstallTicket> InstallTicketList;

  struct Error {
    std::string message;
    std::string title;
  };

  typedef std::vector<Error> ErrorList;

  Transaction();
  ~Transaction();

  void onFinish(const Callback &callback) { m_onFinish.connect(callback); }
  void onDestroy(const Callback &callback) { m_onDestroy.connect(callback); }

  void synchronize(const Remote &, bool userAction = true);
  void uninstall(const Remote &);
  void registerAll(const Remote &);
  void unregisterAll(const Remote &);
  void runTasks();

  bool isCancelled() const { return m_isCancelled; }
  bool isReportEnabled() const { return m_enableReport && !m_isCancelled; }
  bool isRestartNeeded() const { return m_needRestart; }
  DownloadQueue *downloadQueue() { return &m_downloadQueue; }
  size_t taskCount() const { return m_tasks.size(); }

  const InstallTicketList &newPackages() const { return m_new; }
  const InstallTicketList &updates() const { return m_updates; }
  const std::set<Path> &removals() const { return m_removals; }
  const ErrorList &errors() const { return m_errors; }

  bool saveFile(Download *, const Path &);
  void addError(const std::string &msg, const std::string &title);

private:
  struct HostRegistration { bool add; Registry::Entry entry; std::string file; };

  typedef std::function<void ()> IndexCallback;

  void fetchIndex(const Remote &, const IndexCallback &cb);
  void installQueued();
  void installTicket(const InstallTicket &);
  void finish();

  void saveIndex(Download *, const std::string &remoteName);
  void upgrade(const Package *pkg);
  bool allFilesExists(const std::set<Path> &) const;
  void addTask(Task *);

  void registerInHost(bool add, const Registry::Entry &);
  void registerQueued();
  void registerScript(const HostRegistration &);
  void inhibit(const Remote &);

  bool m_isCancelled;
  bool m_enableReport;
  bool m_needRestart;
  Registry *m_registry;

  std::multimap<std::string, IndexCallback> m_remotes;
  std::vector<const RemoteIndex *> m_remoteIndexes;
  std::vector<Task *> m_tasks;

  DownloadQueue m_downloadQueue;
  std::queue<InstallTicket> m_installQueue;
  std::queue<Task *> m_taskQueue;
  std::queue<HostRegistration> m_regQueue;

  InstallTicketList m_new;
  InstallTicketList m_updates;
  std::set<Path> m_removals;
  ErrorList m_errors;

  Signal m_onFinish;
  Signal m_onDestroy;
};

#endif
