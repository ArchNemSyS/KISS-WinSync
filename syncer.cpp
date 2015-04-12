#include "syncer.h"

#include <iostream>

#include <objbase.h>
#include <WinSync.h>

#include <Synchronization.h>
#include <FileSyncProvider.h>
#include <Metastore.h>

#include <QDir>


Syncer::Syncer(QString qstr_source, QString qstr_dest, QStringList qstr_subdir_excludelist, QStringList qstr_filename_excludelist)
{
    //Load COM library - Should be done outside this class
    CoInitialize ( NULL );

    m_wstr_source = qstr_source.toStdWString();
    m_wstr_dest = qstr_dest.toStdWString();

    //Source directory must exist
    //Prepare destination dir and conficts folder
    std::wstring wstr_conflicts_path = m_wstr_dest;
    wstr_conflicts_path.append(L"/~conficts");

    MkPath(QString::fromStdWString(wstr_conflicts_path));

    // init COM objects
    HRESULT     hr;
    hr = CoCreateGuid(&m_GUID_source);
    //std::cout<< " CoCreateGuid | " << (0 <= hr) << std::endl;

    hr = CoCreateGuid(&m_GUID_dest);
    //std::cout<< " CoCreateGuid | "<< (0 <= hr) << std::endl;

    hr = CoCreateInstance ( CLSID_FileSyncProvider, NULL, CLSCTX_INPROC_SERVER, IID_IFileSyncProvider, (void**) &m_provider_source );
    //std::cout<< " CoCreateInstance | "<< (0 <= hr) << std::endl;

    hr = CoCreateInstance ( CLSID_FileSyncProvider, NULL, CLSCTX_INPROC_SERVER, IID_IFileSyncProvider, (void**) &m_provider_dest );
    //std::cout<< " CoCreateInstance | "<< (0 <= hr) << std::endl;

    hr = CoCreateInstance( CLSID_SyncServices, NULL, CLSCTX_INPROC_SERVER, IID_IApplicationSyncServices, (void**) &m_sync_service);
    //std::cout<< " CoCreateInstance  | "<< (0 <= hr) << std::endl;


    hr = m_provider_dest->CreateNewScopeFilter(&m_sync_scope);
    //std::cout<< " CreateNewScopeFilter | "<< (0 <= hr) << std::endl;


    std::wstring wstr_subdir_excludelist = qstr_subdir_excludelist.join(";").toStdWString();
    hr = m_sync_scope->SetSubDirectoryExcludes(wstr_subdir_excludelist.data());
    std::cout<< " SetSubDirectoryExcludes | "<<  qstr_subdir_excludelist.join(";").toStdString() << " | "<< (0 <= hr) << std::endl;


    std::wstring wstr_filename_excludelist = qstr_filename_excludelist.join(";").toStdWString();
    hr = m_sync_scope->SetFilenameExcludes(wstr_filename_excludelist.data());
    std::cout<< " SetFilenameExcludes | "<<  qstr_filename_excludelist.join(";").toStdString() << " | "<< (0 <= hr) << std::endl;



    std::wstring wstr_meta_path = L"\\~MetaData.dat";
    wstr_meta_path.insert(0,m_wstr_source);
    //std::wcout << wstr_meta_path << std::endl;

    hr = m_provider_source->Initialize(m_GUID_source,
                                  m_wstr_source.data(),
                                  wstr_meta_path.data(),
                                  NULL,FILESYNC_INIT_FLAGS_NONE,
                                  m_sync_scope,
                                  NULL,
                                  wstr_conflicts_path.data());

    std::cout<< " m_provider_source Initialize | "<< (0 <= hr) << std::endl;




    wstr_meta_path = L"\\~MetaData.dat";
    wstr_meta_path.insert(0,m_wstr_dest);
    //std::wcout << wstr_meta_path << std::endl;

    hr = m_provider_dest->Initialize(m_GUID_dest,
                                  m_wstr_dest.data(),
                                  wstr_meta_path.data(),
                                  NULL,FILESYNC_INIT_FLAGS_NONE,
                                  m_sync_scope,
                                  NULL,
                                  wstr_conflicts_path.data());

    std::cout<< " m_provider_dest Initialize | "<< (0 <= hr) << std::endl;


    //m_provider_dest->DetectChanges();
    //m_provider_source->DetectChanges();

    Sync();

    //UpdateMetaData();



}

Syncer::~Syncer()
{
    //delete COM objects
    m_sync_service->Release();
    m_provider_dest->Release();
    m_provider_source->Release();

    //Scope should be passed in no need to recreate it every time.
    m_sync_scope->Release();


    //delete unused COM server
    CoFreeUnusedLibraries();

    //Unload COM library - Should be done outside this class
    CoUninitialize();
}

void Syncer::MkPath(QString qstr_path)
{
    QDir qdir(qstr_path);
    if (!qdir.exists())
        qdir.mkpath(qstr_path);
}

void Syncer::Sync()
{
    HRESULT     hr;
    // Cache -> Source
    hr = m_sync_service->CreateSyncSession( m_provider_source, m_provider_dest, &m_sync_session);
    std::cout<< " CreateSyncSession | "<< (0 <= hr) << std::endl;

    /* //  CALLBACKS
    // Need to add IFileSyncProviderCallback *pCallback for provider callback signals
    m_sync_session->QueryInterface(IID_ISyncCallback,(void**) &m_sync_callback);
    hr = m_sync_session->RegisterCallback(m_sync_callback);
    std::cout<< " RegisterCallback | "<< (0 <= hr) << std::endl;
    */


    SYNC_SESSION_STATISTICS* sync_stats = NULL;
    hr = m_sync_session->Start(CRP_DESTINATION_PROVIDER_WINS, sync_stats);
                if (SUCCEEDED(hr))
                {
                std::cout << "Synced Cache -> Source"  << std::endl;
                //std::cout << "Synced -> Changes | " << QString::number(sync_stats->dwChangesApplied) << "| Fails | " << QString::number(sync_stats->dwChangesFailed) <<std::endl;
                }

    // Source -> Cache
    hr = m_sync_service->CreateSyncSession( m_provider_dest, m_provider_source, &m_sync_session);
    std::cout<< " CreateSyncSession | "<< (0 <= hr) << std::endl;


    sync_stats = NULL;
    hr = m_sync_session->Start(CRP_DESTINATION_PROVIDER_WINS, sync_stats);
                if (SUCCEEDED(hr))
                {
                std::cout << "Synced Source -> Cache"  << std::endl;
                }

    m_sync_session->Release();
}

void Syncer::UpdateMetaData()
{
    HRESULT     hr; //IID_ISyncMetadataStore //
    hr = CoCreateInstance ( CLSID_SyncMetadataStore, NULL, CLSCTX_INPROC_SERVER, IID_ISqlSyncMetadataStore, (void**) &m_metadata_store );
    std::cout<< " CoCreateInstance m_metadata_store | "<< (0 <= hr) << std::endl;

   // m_metadata_store->InitializeReplicaMetadata(m_GUID_dest, ,NULL,0,NULL,0);

    /*
     HRESULT InitializeReplicaMetadata(
          BYTE * pbReplicaId,
          ID_PARAMETERS * pIdParameters,
          CUSTOM_FIELD_DEFINITION * pItemCustomFields,
          ULONG cItemCustomFields,
          CUSTOM_FIELDS_INDEX * pItemCustomFieldsIndexes,
          BYTE cItemCustomFieldsIndexes);
     */
    m_metadata_store->Release();
}

