#include "filescustomizer.h"

#include <QFile>

FilesCustomizer::FilesCustomizer(QObject *parent)
    : QObject{parent}
{

}

static const char* gs_cfg_sec_overwrite = "overwrite";
static const char* gs_cfg_sec_copyinto = "copyinto";
static const char* gs_cfg_sec_modify_ini = "modify_ini";
static const char* gs_cfg_sec_special = "special";

bool FilesCustomizer::overwrite_files(QSettings &cfg)
{
    bool ret = true;

    cfg.beginGroup(gs_cfg_sec_overwrite);

    QStringList src_folder_list = cfg.allKeys();

    for(int idx = 0; idx < src_folder_list.size(); ++idx)
    {
        QString src_file_fpn = m_src_folder_fpn + "/" + cfg.value(src_folder_list[idx]).toString();
        if(QFile::exists(src_file_fpn))
        {
            emit log_str_for_user_sig(src_file_fpn + "-->" + m_dst_folder_fpn);
        }
        else
        {
            emit log_str_for_user_sig(src_file_fpn + "不存在", LOG_ERROR);
            ret = false;
            break;
        }
    }

    cfg.endGroup();


    return ret;
}

void FilesCustomizer::process()
{
    QSettings cfg(m_cfg_file_fpn, QSettings::IniFormat);

    overwrite_files(cfg);
}
