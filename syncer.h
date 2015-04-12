#ifndef SYNCER_H
#define SYNCER_H

#include <guiddef.h>
#include <QString>

//forwards
class IFileSyncProvider;
class IFileSyncScopeFilter;
class ISyncCallback;
class IApplicationSyncServices;
class ISyncSession;
class ISyncMetadataStore;

class Syncer
{
public:
    Syncer(QString qstr_source, QString qstr_dest, QStringList qstr_subdir_excludelist, QStringList qstr_filename_excludelist);
    ~Syncer();

private:
    GUID m_GUID_source;
    std::wstring m_wstr_source;
    IFileSyncProvider* m_provider_source;

    GUID m_GUID_dest;
    std::wstring m_wstr_dest;
    IFileSyncProvider* m_provider_dest;

    IFileSyncScopeFilter* m_sync_scope;

    ISyncCallback * m_sync_callback;
    IApplicationSyncServices* m_sync_service;
    ISyncSession* m_sync_session;

    ISyncMetadataStore* m_metadata_store;

    Syncer();
    void MkPath(QString qstr_path);
    void Sync();
    void UpdateMetaData();
};

#endif // SYNCER_H
