#include "reapack.hpp"

#include "errors.hpp"
#include "resource.hpp"

#include <fstream>

#include "reaper_plugin_functions.h"

using namespace std;

void ReaPack::init(REAPER_PLUGIN_HINSTANCE instance, reaper_plugin_info_t *rec)
{
  m_instance = instance;
  m_rec = rec;
  m_mainHandle = GetMainHwnd();
  m_resourcePath = GetResourcePath();
}

ReaPack::~ReaPack()
{
  while(!m_downloadQueue.empty()) {
    Download *download = m_downloadQueue.front();
    download->stop();
    // delete in the stop callback

    m_downloadQueue.pop();
  }
}

void ReaPack::setupAction(const char *name, const char *desc,
  gaccel_register_t *action, ActionCallback callback)
{
  action->desc = desc;
  action->accel.cmd = m_rec->Register("command_id", (void *)name);

  m_rec->Register("gaccel", action);
  m_actions[action->accel.cmd] = callback;
}

bool ReaPack::execActions(const int id, const int)
{
  if(!m_actions.count(id))
    return false;

  m_actions.at(id)();

  return true;
}

void ReaPack::synchronize()
{
  try {
    m_database = Database::load("/Users/cfillion/Programs/reapack/reapack.xml");

    for(PackagePtr pkg : m_database->packages()) {
      installPackage(pkg);
    }
  }
  catch(const reapack_error &e) {
    ShowMessageBox(e.what(), "Database Error", 0);
  }
}

void ReaPack::installPackage(PackagePtr pkg)
{
  const char *url = pkg->lastVersion()->source(0)->url().c_str();

  queuedDownload(url, [=](const int status, const string &contents) {
    if(status != 200) {
      ShowMessageBox(contents.c_str(), "Download failure (debug)", 0);
      return;
    }


    const Location loc = scriptLocation(pkg);
    switch(pkg->type()) {
    case Package::ScriptType:
      break;
    default:
      return;
    };

    RecursiveCreateDirectory(loc.directory.c_str(), 0);

    ofstream file(loc.path());
    if(file.bad()) {
      ShowMessageBox(strerror(errno), pkg->name().c_str(), 0);
      return;
    }

    file << contents;
    file.close();
  });
}

Location ReaPack::scriptLocation(PackagePtr pkg)
{
  string path = string(m_resourcePath)
    .append("/Scripts/ReaScripts")
    .append("/" + pkg->category()->name())
  ;

  return {path, pkg->name()};
}

void ReaPack::queuedDownload(const char *url, DownloadCallback cb)
{
  Download *download = new Download(url);
  download->addCallback(cb);
  download->addCallback([=](const int, const string &) {
    m_downloadQueue.pop();
    delete download;

    if(!m_downloadQueue.empty())
      m_downloadQueue.front()->start();
  });

  m_downloadQueue.push(download);

  if(m_downloadQueue.size() == 1)
    download->start();
}
