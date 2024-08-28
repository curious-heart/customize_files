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

bool FilesCustomizer::overwrite_files(QSettings &cfg, QString ele_sep)
{
    bool ret = true;
    bool worked = false;

    cfg.beginGroup(gs_cfg_sec_overwrite);

    QStringList src_folder_list = cfg.allKeys();
    cp_file_result_list_t cp_ret_list;
    QStringList cp_ret_str_list;
    LOG_LEVEL log_lvl;
    QString log_str;

    for(int idx = 0; idx < src_folder_list.size(); ++idx)
    {
        QString relative_dir = src_folder_list[idx],
                file_names_list_str = cfg.value(relative_dir, "").toString();
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
        cp_ret_list.clear(); cp_ret_str_list.clear();
        ret = copy_files(file_names_list, dst_folder_fpn, CP_FILE_EXIST_OP_OVERWRITE,
                         true, true, &cp_ret_list, &cp_ret_str_list);

        for(int r_idx = 0; r_idx < qMin(cp_ret_list.size(), cp_ret_str_list.size()); ++r_idx)
        {
            log_lvl = (cp_ret_list[r_idx] <= CP_FILE_OK_FLAG) ? LOG_INFO : LOG_ERROR;
            emit log_str_for_user_sig(cp_ret_str_list[r_idx], log_lvl);
        }

        if(!ret)
        {
            break;
        }
        if(cp_ret_list.size() > 0) worked = true;
    }

    if(ret)
    {
        if(worked)
        {
            emit log_str_for_user_sig(QString(gs_cfg_sec_overwrite) + g_str_succeed + "\n", LOG_INFO, Qt::green);
        }
        else
        {
            emit log_str_for_user_sig(QString(gs_cfg_sec_overwrite) + g_str_is_empty + "\n", LOG_INFO);
        }
    }
    else
    {
        emit log_str_for_user_sig(QString(gs_cfg_sec_overwrite) + g_str_fail + "\n", LOG_ERROR);
    }

    cfg.endGroup();

    return ret;
}

bool FilesCustomizer::copy_into_folder(QSettings &cfg, QString ele_sep)
{
    bool ret = true;
    bool worked = false;

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
        QString dst_pths_str = cfg.value(src_list[idx], "").toString();
        QStringList dst_pths = dst_pths_str.split(ele_sep, Qt::SkipEmptyParts);
        QString src_folder;

        for(int dp_idx = 0; dp_idx < dst_pths.size(); ++dp_idx)
        {
            rm_slash_at_ends(dst_pths[dp_idx]);
            if(dst_pths[dp_idx].isEmpty())
            {
                go_on = false;
                break;
            }
        }

        if(!go_on || src_fs.size() < 1)
        {
            go_on = false;
        }
        else
        {
            src_folder = src_fs[0];
            rm_slash_at_ends(src_folder);
            if(src_folder.isEmpty()) go_on = false;
        }

        if(!go_on)
        {
            log_str = QString("[") + gs_cfg_sec_copyinto + "]"
                                + src_list[idx] + "=" + dst_pths_str + " : " + "invalid.";
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

        for(int dp_idx = 0; dp_idx < dst_pths.size(); ++dp_idx)
        {
            cp_ret_list.clear(); cp_ret_str_list.clear();
            ret = copy_files(to_copy_list, m_dst_folder_fpn + "/" + dst_pths[dp_idx],
                         CP_FILE_EXIST_OP_OVERWRITE, true, true, &cp_ret_list, &cp_ret_str_list, false);

            for(int r_idx = 0; r_idx < qMin(cp_ret_list.size(), cp_ret_str_list.size()); ++r_idx)
            {
                log_lvl = (cp_ret_list[r_idx] <= CP_FILE_OK_FLAG) ? LOG_INFO : LOG_ERROR;
                emit log_str_for_user_sig(cp_ret_str_list[r_idx], log_lvl);
            }
            if(!ret) break;
        }

        if(!ret) break;

        if(cp_ret_list.size() > 0) worked = true;
    }

    cfg.endGroup();

    if(ret)
    {
        if(worked)
        {
            emit log_str_for_user_sig(QString(gs_cfg_sec_copyinto) + g_str_succeed + "\n", LOG_INFO, Qt::green);
        }
        else
        {
            emit log_str_for_user_sig(QString(gs_cfg_sec_copyinto) + g_str_is_empty + "\n", LOG_INFO);
        }
    }
    else
    {
        emit log_str_for_user_sig(QString(gs_cfg_sec_copyinto) + g_str_fail + "\n", LOG_ERROR);
    }

    return ret;
}

bool FilesCustomizer::modify_ini(QSettings &cfg, QString ele_sep)
{
    static const char* ls_ini_fn_filter_str = "*.ini";
    bool ret = true;
    bool worked = false;
    QStringList dir_str_list;

    cfg.beginGroup(gs_cfg_sec_modify_ini);

    dir_str_list = cfg.allKeys();

    for(int f_idx = 0; f_idx < dir_str_list.size(); ++f_idx)
    {
        QString src_prefix, dst_prefix;
        QStringList file_str_list;
        QStringList src_ini_fpn_list;

        src_prefix = m_src_folder_fpn + "/" + dir_str_list[f_idx];
        dst_prefix = m_dst_folder_fpn + "/" + dir_str_list[f_idx];

        file_str_list = cfg.value(dir_str_list[f_idx], "").toString().split(ele_sep, Qt::SkipEmptyParts);
        if(file_str_list.size() > 0)
        {
            for(int s_idx = 0; s_idx < file_str_list.size(); ++s_idx)
            {
                src_ini_fpn_list.append(src_prefix + "/" + file_str_list[s_idx]);
            }
        }
        else
        {
            collect_files(src_prefix, QStringList(ls_ini_fn_filter_str), src_ini_fpn_list);
        }

        QString log_str;
        for(int ini_idx = 0; ini_idx < src_ini_fpn_list.size(); ++ini_idx)
        {
            QString src_ini_fpn = src_ini_fpn_list[ini_idx];
            if(!QFileInfo::exists(src_ini_fpn))
            {
                log_str = QString(g_str_src) + g_str_file + " \"" + src_ini_fpn + "\" "
                            + g_str_not_exists;
                emit log_str_for_user_sig(log_str, LOG_ERROR);
                ret = false;
                break;
            }
            QString dst_ini_fpn = dst_prefix + src_ini_fpn.mid(src_prefix.size());
            if(QFileInfo::exists(dst_ini_fpn))
            {
                log_str = QString(g_str_update) + g_str_dst + g_str_file + " \"" + dst_ini_fpn
                                + "\" :\n" ;
                //update key=value of src into dst
                QSettings src_ini_settings(src_ini_fpn, QSettings::IniFormat),
                        dst_ini_settings(dst_ini_fpn, QSettings::IniFormat);
                QStringList sec_list = src_ini_settings.childGroups();
                for(int sec_idx = 0; sec_idx < sec_list.size(); ++sec_idx)
                {
                    log_str += QString("[%1]\n").arg(sec_list[sec_idx]);

                    src_ini_settings.beginGroup(sec_list[sec_idx]);
                    dst_ini_settings.beginGroup(sec_list[sec_idx]);
                    QStringList key_list = src_ini_settings.allKeys();
                    for(int key_idx = 0; key_idx < key_list.size(); ++key_idx)
                    {
                        QString s_v = src_ini_settings.value(key_list[key_idx], "").toString();
                        dst_ini_settings.setValue(key_list[key_idx], s_v);

                        log_str += QString("%1=%2\n").arg(key_list[key_idx],s_v);
                    }

                    dst_ini_settings.endGroup();
                    src_ini_settings.endGroup();
                }

                emit log_str_for_user_sig(log_str);
            }
            else
            {
                log_str = QString(g_str_dst) + g_str_file + " \"" + dst_ini_fpn + "\" "
                        + g_str_not_exists + ".\n";
                //just copy the src ini file.
                QString dst_folder_str = QFileInfo(dst_ini_fpn).path();
                QDir dst_folder(dst_folder_str);
                if(!dst_folder.exists())
                {
                    ret = dst_folder.mkpath(dst_folder_str);
                    if(!ret)
                    {
                        log_str += QString(g_str_create) + g_str_folder + " \"" + dst_folder_str
                                + "\" " + g_str_fail + ".";
                        emit log_str_for_user_sig(log_str, LOG_ERROR);
                        break;
                    }
                }
                ret = QFile::copy(src_ini_fpn, dst_ini_fpn);
                if(!ret)
                {
                    log_str += QString(g_str_copy) + g_str_file + " \"" + src_ini_fpn + "\" "
                            + g_str_to + " \"" + dst_ini_fpn + "\" " + g_str_fail + ".";
                    emit log_str_for_user_sig(log_str, LOG_ERROR);
                    break;
                }
                log_str += QString(g_str_copy) + g_str_file + ".";
                emit log_str_for_user_sig(log_str);
            }
        }

        if(!ret) break;

        if(src_ini_fpn_list.size() > 0) worked = true;
    }

    cfg.endGroup();

    if(ret)
    {
        if(worked)
        {
            emit log_str_for_user_sig(QString(gs_cfg_sec_modify_ini) + g_str_succeed + "\n", LOG_INFO, Qt::green);
        }
        else
        {
            emit log_str_for_user_sig(QString(gs_cfg_sec_modify_ini) + g_str_is_empty + "\n", LOG_INFO);
        }
    }
    else
    {
        emit log_str_for_user_sig(QString(gs_cfg_sec_modify_ini) + g_str_fail + "\n", LOG_ERROR);
    }
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
        ret = copy_into_folder(cfg, ele_sep);
        if(ret)
        {
            ret = modify_ini(cfg, ele_sep);
        }
    }

    if(ret)
    {
        emit log_str_for_user_sig(QString("\n") + g_str_operation + g_str_succeed + "！",
                                  LOG_INFO, Qt::darkGreen);
    }
    else
    {
        emit log_str_for_user_sig(QString("\n") + g_str_operation + g_str_fail + "！", LOG_ERROR);
    }
}
