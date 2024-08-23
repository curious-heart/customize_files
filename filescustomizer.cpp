#include <QDir>
#include <QFile>

#include "common_tools/common_tool_func.h"
#include "logger/logger.h"
#include "filescustomizer.h"

FilesCustomizer::FilesCustomizer(QObject *parent)
    : QObject{parent}
{

}

static const char* gs_cfg_sec_global = "global_cfg";
static const char* gs_cfg_key_ele_sep = "ele_sep";
static const char* gs_cfg_def_ele_sep_val = ":";

static const char* gs_cfg_sec_overwrite = "overwrite";
static const char* gs_cfg_sec_copyinto = "copyinto";
static const char* gs_cfg_sec_modify_ini = "modify_ini";
static const char* gs_cfg_sec_special = "special";

bool FilesCustomizer::overwrite_files(QSettings &cfg, QString ele_sep)
{
    bool ret = true;

    cfg.beginGroup(gs_cfg_sec_overwrite);

    QStringList src_folder_list = cfg.allKeys();
    cp_file_result_list_t cp_ret_list;
    QStringList cp_ret_str_list;
    LOG_LEVEL log_lvl;
    QString log_str;

    for(int idx = 0; idx < src_folder_list.size(); ++idx)
    {
        QString relative_dir = src_folder_list[idx],
                file_names_list_str = cfg.value(relative_dir).toString();
        QStringList file_names_list = file_names_list_str.split(ele_sep, Qt::SkipEmptyParts);

        rm_slash_at_ends(relative_dir);

        if(relative_dir.isEmpty())
        {
            log_str = QString("[") + gs_cfg_sec_overwrite + "]"
                    + relative_dir + "=" + file_names_list_str + " : " + "invalid.";
            emit log_str_for_user_sig(log_str, LOG_ERROR);
            ret = false;
            break;
        }

        if(file_names_list.size() == 0)
        {
            //copy entire dir
            file_names_list.append(m_src_folder_fpn + "/" + relative_dir);
        }
        else
        {
            for(int f_idx = 0; f_idx < file_names_list.size(); ++ f_idx)
            {
                file_names_list[f_idx].prepend(m_src_folder_fpn + "/" + relative_dir + "/");
            }
        }

        QString dst_folder_fpn = m_dst_folder_fpn + "/" + relative_dir;
        ret = copy_files(file_names_list, dst_folder_fpn, CP_FILE_EXIST_OP_OVERWRITE,
                         true, true, &cp_ret_list, &cp_ret_str_list);

        for(int r_idx = 0; r_idx < qMin(cp_ret_list.size(), cp_ret_str_list.size()); ++r_idx)
        {
            log_lvl = (cp_ret_list[idx] <= CP_FILE_OK_FLAG) ? LOG_INFO : LOG_ERROR;
            emit log_str_for_user_sig(cp_ret_str_list[r_idx], log_lvl);
            DIY_LOG(log_lvl, cp_ret_str_list[r_idx]);
        }

        if(!ret)
        {
            break;
        }
    }

    cfg.endGroup();

    return ret;
}

bool FilesCustomizer::copy_into_folder(QSettings &cfg, QString ele_sep)
{
    bool ret = true;

    cfg.beginGroup(gs_cfg_sec_copyinto);

    QStringList src_list = cfg.allKeys();
    cp_file_result_list_t cp_ret_list;
    QStringList cp_ret_str_list;
    LOG_LEVEL log_lvl;
    QString log_str;

    for(int idx = 0; idx < src_list.size(); ++idx)
    {
        bool go_on = true;

        QStringList src_fs = src_list[idx].split(ele_sep, Qt::SkipEmptyParts);
        QString dst_pth = cfg.value(src_list[idx]).toString();
        QString src_folder;

        rm_slash_at_ends(dst_pth);

        if(src_fs.size() < 1 || dst_pth.isEmpty())
        {
            go_on = false;
        }
        else// if(src_fs.size() >= 1)
        {
            src_folder = src_fs[0];
            rm_slash_at_ends(src_folder);
            if(src_folder.isEmpty()) go_on = false;
        }

        if(!go_on)
        {
            log_str = QString("[") + gs_cfg_sec_copyinto + "]"
                                + src_list[idx] + "=" + dst_pth + " : " + "invalid.";
            emit log_str_for_user_sig(log_str, LOG_ERROR);
            ret = false;
            break;
        }

        QStringList to_copy_list;
        if(1 == src_fs.size())
        {
            //this key identify a folder only.
            to_copy_list.append(m_src_folder_fpn + "/" + src_folder);
        }
        else
        {
            for(int f_idx = 1; f_idx < src_fs.size(); ++f_idx)
            {
                to_copy_list.append(m_src_folder_fpn + "/" + src_folder + "/" + src_fs[f_idx]);
            }
        }


        ret = copy_files(to_copy_list, m_dst_folder_fpn + "/" + dst_pth,
                     CP_FILE_EXIST_OP_OVERWRITE, true, true, &cp_ret_list, &cp_ret_str_list, false);

        for(int r_idx = 0; r_idx < qMin(cp_ret_list.size(), cp_ret_str_list.size()); ++r_idx)
        {
            log_lvl = (cp_ret_list[idx] <= CP_FILE_OK_FLAG) ? LOG_INFO : LOG_ERROR;
            emit log_str_for_user_sig(cp_ret_str_list[r_idx], log_lvl);
            DIY_LOG(log_lvl, cp_ret_str_list[r_idx]);
        }

        if(!ret)
        {
            break;
        }
    }

    cfg.endGroup();

    return ret;
}

void FilesCustomizer::process()
{
    bool ret;

    if(m_src_folder_fpn.endsWith("/")) m_src_folder_fpn.chop(1);
    if(m_dst_folder_fpn.endsWith("/")) m_dst_folder_fpn.chop(1);

    QSettings cfg(m_cfg_file_fpn, QSettings::IniFormat);
    QString ele_sep;

    cfg.beginGroup(gs_cfg_sec_global);
    ele_sep = cfg.value(gs_cfg_key_ele_sep, gs_cfg_def_ele_sep_val).toString();
    cfg.endGroup();

    ret = overwrite_files(cfg, ele_sep);
    if(ret)
    {
        emit log_str_for_user_sig(QString(gs_cfg_sec_overwrite) + g_str_succeed, LOG_INFO,
                                  Qt::green);
        ret = copy_into_folder(cfg, ele_sep);

        if(ret)
        {
            emit log_str_for_user_sig(QString(gs_cfg_sec_copyinto) + g_str_succeed, LOG_INFO,
                                      Qt::green);
        }
        else
        {
            emit log_str_for_user_sig(QString(gs_cfg_sec_copyinto) + g_str_fail, LOG_ERROR);
        }
    }
    else
    {
        emit log_str_for_user_sig(QString(gs_cfg_sec_overwrite) + g_str_fail, LOG_ERROR);
    }
}
