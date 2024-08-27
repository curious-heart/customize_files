#include <windows.h>

#include "common_tool_func.h"
#include "logger/logger.h"

#include <QDateTime>
#include <QList>
#include <QProcess>
#include <QDir>
#include <QColor>
#include <QFont>
#include <QtMath>
#include <QFileInfo>
#include <QPushButton>

bool exec_external_process(QString cmd, QString cmd_args, bool as_admin = false)
{
    DIY_LOG(LOG_INFO, QString("exec_external_process: %1 %2, as_admin: %3")
                      .arg(cmd, cmd_args).arg((int)as_admin));
    bool ret = false;
    if(!cmd.isEmpty())
    {
        SHELLEXECUTEINFO shellInfo;
        memset(&shellInfo, 0, sizeof(shellInfo));
        shellInfo.cbSize = sizeof(SHELLEXECUTEINFO);
        shellInfo.hwnd = NULL;
        std::wstring wlpstrstd_verb;
        if(as_admin)
        {
            wlpstrstd_verb = QString("runas").toStdWString();
        }
        else
        {
            wlpstrstd_verb = QString("open").toStdWString();
        }
        shellInfo.lpVerb = wlpstrstd_verb.c_str();
        std::wstring wlpstrstd_file = cmd.toStdWString();
        shellInfo.lpFile = wlpstrstd_file.c_str();
        std::wstring wlpstrstd_param = cmd_args.toStdWString();
        shellInfo.lpParameters = wlpstrstd_param.c_str();
        std::wstring wlpstrstd_currpth = QDir::currentPath().toStdWString();
        shellInfo.lpDirectory = wlpstrstd_currpth.c_str();
        shellInfo.nShow = SW_HIDE;
        BOOL bRes = ::ShellExecuteEx(&shellInfo);
        if(bRes)
        {
            ret = true;
            DIY_LOG(LOG_INFO, "ShellExecuteEx ok.");
        }
        else
        {
            ret = false;
            DWORD err = GetLastError();
            DIY_LOG(LOG_ERROR, QString("ShellExecuteEx return false, error: %1").arg((quint64)err));
        }
    }
    else
    {
        DIY_LOG(LOG_WARN, QString("ShellExecuteEx, cmd is empty!"));
    }
    return ret;
}

QString common_tool_get_curr_dt_str()
{
    QDateTime curDateTime = QDateTime::currentDateTime();
    QString dtstr = curDateTime.toString("yyyyMMddhhmmss");
    return dtstr;
}

QString common_tool_get_curr_date_str()
{
    QDateTime curDateTime = QDateTime::currentDateTime();
    QString datestr = curDateTime.toString("yyyyMMdd");
    return datestr;
}

QString common_tool_get_curr_time_str()
{
    QTime curTime = QTime::currentTime();
    QString time_str = curTime.toString("hh:mm:ss");
    return time_str;
}

bool mkpth_if_not_exists(const QString &pth_str)
{
    QDir data_dir(pth_str);
    bool ret = true;
    if(!data_dir.exists())
    {
        data_dir = QDir();
        ret = data_dir.mkpath(pth_str);
    }
    return ret;
}

QString shutdown_system(QString reason_str,int wait_time)
{
    QString s_c = "shutdown";
    QStringList ps_a;
    QProcess ps;
    ps.setProgram(s_c);
    ps_a << "/s" << "/t" << QString("%1").arg(wait_time);
    ps_a << "/d" << "u:4:1" << "/c" << reason_str;
    ps.setArguments(ps_a);
    ps.startDetached();
    return s_c + " " + ps_a.join(QChar(' '));
}

#undef EDGE_ITEM
#define EDGE_ITEM(a) #a
const char* RangeChecker::range_edge_strs[] =
{
    EDGE_LIST
};
#undef EDGE_ITEM
static const char* gs_range_checker_err_msg_invalid_edge_p1 = "low/up edge should be";
static const char* gs_range_checker_err_msg_invalid_edge_p2 = "and can't both be";
static const char* gs_range_checker_err_msg_invalid_eval =
        "Invalid range: min must be less than or equal to max!";
#define RANGE_PARAMS_CHECK(min, max, low_edge, up_edge) \
{\
    valid = ((min) <= (max));\
    if(!valid || ((low_edge) > EDGE_INFINITE) || ((up_edge) > EDGE_INFINITE)\
            || ((low_edge) < EDGE_INCLUDED) || ((up_edge) < EDGE_INCLUDED)\
            || (EDGE_INFINITE == (low_edge) && EDGE_INFINITE == (up_edge))\
            )\
    {\
        DIY_LOG(LOG_ERROR, gs_range_checker_err_msg_invalid);\
        return;\
    }\
}

RangeChecker::RangeChecker(double min, double max, const char* unit_str,
                           range_edge_enum_t low_edge, range_edge_enum_t up_edge)
{
    if((low_edge > EDGE_INFINITE) || (up_edge > EDGE_INFINITE)
        || (low_edge < EDGE_INCLUDED) || (up_edge < EDGE_INCLUDED)
        || (EDGE_INFINITE == low_edge && EDGE_INFINITE == up_edge))
    {
        DIY_LOG(LOG_ERROR, QString("%1 %2, %3, or %4, %5").arg(
                    gs_range_checker_err_msg_invalid_edge_p1,
                    range_edge_strs[EDGE_INCLUDED],
                    range_edge_strs[EDGE_EXCLUDED],
                    range_edge_strs[EDGE_INFINITE],
                    gs_range_checker_err_msg_invalid_edge_p2
                    ));
        return;
    }
    valid = (EDGE_INFINITE == low_edge || EDGE_INFINITE == up_edge) ? true : (min <= max);
    this->min = min; this->max = max;
    this->low_edge = low_edge; this->up_edge = up_edge;
    this->unit_str = unit_str;

    if(!valid)
    {
        DIY_LOG(LOG_ERROR, QString(gs_range_checker_err_msg_invalid_eval));
    }
}

#define RANGE_CHECKER(val, type) \
{\
    if(EDGE_INCLUDED == type##_low_edge) ret = (ret && ((type)(val) >= min_##type));\
    else if(EDGE_EXCLUDED == type##_low_edge) ret = (ret && ((type)(val) > min_##type));\
\
    if(EDGE_INCLUDED == type##_up_edge) ret = (ret && ((type)(val) <= max_##type));\
    else if(EDGE_EXCLUDED == type##_up_edge) ret = (ret && ((type)(val) < max_##type));\
}
bool RangeChecker::range_check(double val)
{
    bool ret = true;
    if(!valid)
    {
        DIY_LOG(LOG_ERROR, "Invalid Range Checker!")
        return false;
    }

    if(EDGE_INCLUDED == low_edge) ret = (ret && (val >= min));
    else if(EDGE_EXCLUDED == low_edge) ret = (ret && (val > min));

    if(EDGE_INCLUDED == up_edge) ret = (ret && (val <= max));
    else if(EDGE_EXCLUDED == up_edge) ret = (ret && (val < max));
    return ret;
}
#undef RANGE_PARAMS_CHECK
#undef RANGE_CHECKER

RangeChecker::range_edge_enum_t RangeChecker::range_low_edge()
{
    return low_edge;
}
RangeChecker::range_edge_enum_t RangeChecker::range_up_edge()
{
    return up_edge;
}
double RangeChecker::range_min()
{
    return min;
}
double RangeChecker::range_max()
{
    return max;
}
QString RangeChecker::range_str(common_data_type_enum_t d_type, double factor, QString new_unit_str )
{
    QString ret_str;

    if(!valid) return ret_str;

    ret_str = (EDGE_INCLUDED == low_edge ? "[" : "(");
    ret_str += (EDGE_INFINITE == low_edge) ? "" :
                ((INT_DATA == d_type) ? QString::number((int)(min * factor)) :
                                        QString::number((float)(min * factor)));
    ret_str += ", ";
    ret_str += (EDGE_INFINITE == up_edge) ? "" :
                ((INT_DATA == d_type) ? QString::number((int)(max * factor)) :
                                        QString::number((float)(max * factor)));
    ret_str += (EDGE_INCLUDED == up_edge) ? "]" : ")";
    ret_str += ((1 == factor) || new_unit_str.isEmpty()) ? QString(unit_str) : new_unit_str;
    return ret_str;
}

void RangeChecker::set_min_max(double min_v, double max_v)
{
    if(EDGE_INFINITE == low_edge || EDGE_INFINITE == up_edge || min_v <= max_v)
    {
        min = min_v;
        max = max_v;
    }
}
void RangeChecker::set_edge(range_edge_enum_t low_e, range_edge_enum_t up_e)
{
    low_edge = low_e; up_edge = up_e;
}

void RangeChecker::set_unit_str(const char* unit_s)
{
    unit_str = unit_s;
}

void append_str_with_color_and_weight(QTextEdit* ctrl, QString str,
                                      QColor new_color, int new_font_weight)
{
    QColor curr_color = ctrl->textColor();
    QFont curr_font = ctrl->currentFont(), new_font = curr_font;
    bool modify_color = !(new_color == curr_color),
         modify_font_weight = !(new_font_weight < 0 || new_font_weight == curr_font.weight());

    if(modify_color) ctrl->setTextColor(new_color);
    if(modify_font_weight)
    {
        new_font.setWeight(new_font_weight);
        ctrl->setCurrentFont(new_font);
    }

    ctrl->append(str);

    if(modify_font_weight) ctrl->setCurrentFont(curr_font);
    if(modify_color) ctrl->setTextColor(curr_color);
}

int count_discrete_steps(double low_edge, double up_edge, double step)
{
    if(low_edge == up_edge) return 1;
    if(0 == step) return 0;

    double tmp = (up_edge - low_edge) / step;
    if(tmp < 0) return 0;

    return qCeil(tmp) + 1;
}

int standard_btn_msg_box(QString title, QString info, QMessageBox::StandardButtons btns)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(title);
    msgBox.setText(info);
    msgBox.setStandardButtons(btns);
    return msgBox.exec();
}

const char* g_str_src = "源";
const char* g_str_dst = "目标";
const char* g_str_folder = "文件夹";
const char* g_str_file = "文件";
const char* g_str_not_exists = "不存在";
const char* g_str_already_exists = "已存在";
const char* g_str_create = "创建";
const char* g_str_need = "需要";
const char* g_str_succeed = "成功";
const char* g_str_fail = "失败";
const char* g_str_ask_if_want_to_make_it = "是否需要创建？";
const char* g_str_user_refuse_to_make = "用户拒绝创建";
const char* g_str_try_to_make = "尝试创建";
const char* g_str_try_to_ow = "尝试覆盖";
const char* g_str_try_to_rm = "尝试删除";
const char* g_str_user_skip = "用户忽略";
const char* g_str_operation_abort_by_caller = "操作被调用者中止";
const char* g_str_operation_abort_by_user = "操作被用户中止";
const char* g_str_overwrite = "覆盖";
const char* g_str_remove = "删除";
const char* g_str_copy = "复制";
const char* g_str_overwrite_all = "全部覆盖";
const char* g_str_skip = "忽略";
const char* g_str_skip_all = "全部忽略";
const char* g_str_but = "但";
const char* g_str_current = "当前";
const char* g_str_to = "至";
const char* g_str_update = "更新";
const char* g_str_operation = "操作";

/*dst_pth DOES NOT end with "/" */
bool copy_files(QStringList src_list, QString dst_pth, cp_file_exist_op_e_t f_exist_op,
                bool mk_dst_folder_if_not_exists, bool stop_on_error,
                cp_file_result_list_t * result_list, QStringList *result_str_list,
                bool ask_when_mk_dst_folder)
{
    QFileInfo file_info;
    QString err_str;
    bool ret = true;
    cp_file_result_e_t cp_result;

    QDir dst_dir(dst_pth);

    if(!dst_dir.exists())
    {
        err_str = QString(g_str_dst) + g_str_folder + " \"" + dst_pth + "\" " + g_str_not_exists
                + ".";
        if(!mk_dst_folder_if_not_exists)
        {
            ret = false;
            if(result_list) result_list->append(CP_FILE_ERR_NO_DST_FOLDER);
            if(result_str_list) result_str_list->append(err_str);
            return ret;
        }

        if(ask_when_mk_dst_folder)
        {
            QMessageBox::StandardButton sel = (QMessageBox::StandardButton)standard_btn_msg_box(
                        "???", err_str + "\n\n" + g_str_ask_if_want_to_make_it,
                        QMessageBox::Yes | QMessageBox::No);
            if(QMessageBox:: No == sel)
            {
                ret = false;
                if(result_list) result_list->append(CP_FILE_ERR_USER_REFUSE_TO_MK_DST_FOLDER);
                if(result_str_list) result_str_list->append(err_str + " "
                                                            + g_str_user_refuse_to_make
                                                            + g_str_dst + g_str_folder + ".");
                return ret;
            }
        }

        ret = dst_dir.mkpath(dst_pth);
        if(!ret)
        {
            if(result_list) result_list->append(CP_FILE_ERR_MK_DST_FOLDER_FAILS);
            if(result_str_list) result_str_list->append(err_str + " "
                                                        + g_str_try_to_make + g_str_dst
                                                        + g_str_folder + g_str_fail + ".");
            return ret;
        }
    }

    bool user_sel_skip_all = false, user_sel_overwrite_all = false;
    cp_file_exist_op_e_t this_one_op;
    for(int idx = 0; idx < src_list.size(); ++idx)
    {
        ret = true;
        file_info = QFileInfo(src_list[idx]);
        if(!file_info.exists())
        {
            ret = false;
            if(result_list) result_list->append(CP_FILE_ERR_NO_SRC_FILE);
            if(result_str_list) result_str_list->append(QString(g_str_src) + g_str_file
                                                        +" \"" + src_list[idx]
                                                        + "\" " + g_str_not_exists + ".");
            if(stop_on_error) break;
            else continue;
        }

        //copy contents in dir
        if(file_info.isDir())
        {
            QDir src_dir(src_list[idx]);
            QStringList file_list = src_dir.entryList(QDir::Files),
                    dir_list = src_dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
            if(!file_list.isEmpty())
            {
                for(int f_idx = 0; f_idx < file_list.size(); ++f_idx)
                {
                    file_list[f_idx].prepend(src_list[idx] + "/");
                }
                ret = copy_files(file_list, dst_pth, f_exist_op, mk_dst_folder_if_not_exists,
                           stop_on_error, result_list, result_str_list, ask_when_mk_dst_folder);
                if(!ret && stop_on_error) break;
                else continue;
            }

            if(!dir_list.isEmpty())
            {
                for(int d_idx = 0; d_idx < dir_list.size(); ++d_idx)
                {
                    QString new_src_dir_str = src_list[idx] + "/" + dir_list[d_idx],
                            new_dst_pth = dst_pth + "/" + dir_list[d_idx];

                    ret = copy_files(QStringList(new_src_dir_str), new_dst_pth,
                             f_exist_op, mk_dst_folder_if_not_exists,
                             stop_on_error, result_list, result_str_list, ask_when_mk_dst_folder);
                    if(!ret && stop_on_error) break;
                    else continue;
                }
            }

            if(!ret && stop_on_error) break;
            else continue;
        }

        //copy file
        QString file_name = file_info.fileName();
        QString dst_file_fpn = dst_pth + "/" + file_name;
        QFile dst_file(dst_file_fpn);
        if(dst_file.exists())
        {
            this_one_op = f_exist_op;
            err_str = QString(g_str_dst) + g_str_file +  " \"" + dst_file_fpn + "\" " +
                        g_str_already_exists + ".";
            if(CP_FILE_EXIST_OP_ABORT == f_exist_op)
            {
                ret = false;
                if(result_list) result_list->append(CP_FILE_ERR_CALLER_ABORT);
                if(result_str_list) result_str_list->append(err_str + " "
                                                           + g_str_operation_abort_by_caller + ".");
                break;
            }

            if(!user_sel_skip_all && !user_sel_overwrite_all && CP_FILE_EXIST_OP_ASK == f_exist_op)
            {
                QMessageBox op_box;
                QAbstractButton *user_sel;
                QPushButton *ow_btn = op_box.addButton(g_str_overwrite, QMessageBox::AcceptRole),
                        *ow_a_btn = op_box.addButton(g_str_overwrite_all, QMessageBox::AcceptRole),
                        *skip_btn = op_box.addButton(g_str_skip, QMessageBox::AcceptRole),
                        *skip_a_btn = op_box.addButton(g_str_skip_all, QMessageBox::AcceptRole);
                op_box.addButton(QMessageBox::Abort);

                op_box.exec();
                user_sel = op_box.clickedButton();
                if(user_sel == ow_btn)
                {
                    this_one_op = CP_FILE_EXIST_OP_OVERWRITE;
                } else if(user_sel == ow_a_btn)
                {
                    this_one_op = CP_FILE_EXIST_OP_OVERWRITE;
                    user_sel_overwrite_all = true;
                } else if(user_sel == skip_btn)
                {
                    this_one_op = CP_FILE_EXIST_OP_SKIP;
                } else if(user_sel == skip_a_btn)
                {
                    this_one_op = CP_FILE_EXIST_OP_SKIP;
                    user_sel_skip_all = true;
                } else //abort_btn
                {
                    this_one_op = CP_FILE_EXIST_OP_ABORT;
                }
            }
            else
            {
                if(user_sel_skip_all) this_one_op = CP_FILE_EXIST_OP_SKIP;
                else if(user_sel_overwrite_all) this_one_op = CP_FILE_EXIST_OP_OVERWRITE;
            }

            switch(this_one_op)
            {
                case CP_FILE_EXIST_OP_OVERWRITE:
                    ret = dst_file.remove();
                    if(!ret)
                    {
                        err_str += QString(" ") + g_str_try_to_ow + g_str_but +
                                g_str_remove + g_str_current + g_str_file + g_str_fail + ".";
                        cp_result = CP_FILE_ERR_REMOVE_OLD_FILE_FAILS;
                        break;
                    }
                    ret = QFile::copy(src_list[idx], dst_file_fpn);
                    if(!ret)
                    {
                        err_str += QString(" ") + g_str_try_to_ow + ","
                                + g_str_remove + g_str_current + g_str_file + g_str_succeed
                                + g_str_but + g_str_copy + g_str_fail + ".";
                        cp_result = CP_FILE_ERR_UNKNOW_ERR;
                    }
                    else
                    {
                        err_str += QString(" ") + g_str_overwrite + ".";
                        cp_result = CP_FILE_OVERWRITTEN;
                    }
                    break;

                case CP_FILE_EXIST_OP_SKIP:
                    err_str += QString(" ") + g_str_user_skip + ".";
                    cp_result = CP_FILE_SKIPPED;
                    break;

                case CP_FILE_EXIST_OP_ABORT:
                default:
                    ret = false;
                    err_str += QString(" ") + g_str_operation_abort_by_user + ".";
                    cp_result = CP_FILE_ERR_USER_ABORT;
                    break;
            }
            if(result_list) result_list->append(cp_result);
            if(result_str_list) result_str_list->append(err_str);
            if(!ret && (stop_on_error || (CP_FILE_EXIST_OP_ABORT == this_one_op))) break;
        }
        else //dst folder does not contain a file with the same name as src file. so just copy.
        {
            err_str = QString(g_str_copy) + g_str_file + " \"" + src_list[idx] + "\" "
                    + g_str_to + " \"" + dst_pth + "\"";
            ret = QFile::copy(src_list[idx], dst_file_fpn);
            if(!ret)
            {
                err_str += QString(" ") + g_str_fail + ".";
                cp_result = CP_FILE_ERR_UNKNOW_ERR;
            }
            else
            {
                err_str += QString(" ") + g_str_succeed + ".";
                cp_result = CP_FILE_WRITTEN;
            }
            if(result_list) result_list->append(cp_result);
            if(result_str_list) result_str_list->append(err_str);
            if(!ret && stop_on_error) break;
        }
    }
    return ret;
}

void rm_slash_at_ends(QString &str, bool s, bool e)
{
    if(s && str.startsWith("/")) str.remove(0, 1);
    if(e && str.endsWith(("/"))) str.chop(1);
}

void walkout_dir(QString root_dir, QStringList file_filter, dir_walkout_ret_t & ret)
{
    QDir curr_dir;
    dir_walk_node_s_t curr_node;

    curr_node.curr_pth = root_dir;
    curr_dir.setPath(curr_node.curr_pth);
    curr_node.dir_str_list = curr_dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    curr_node.file_str_list = curr_dir.entryList(file_filter, QDir::Files);
    ret.append(curr_node);

    int n_idx = 0;
    while(true)
    {
        for(int d_idx = 0; d_idx < ret[n_idx].dir_str_list.size(); ++d_idx)
        {
            curr_node.curr_pth = ret[n_idx].curr_pth + "/" + ret[n_idx].dir_str_list[d_idx];
            curr_dir.setPath(curr_node.curr_pth);
            curr_node.dir_str_list = curr_dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
            curr_node.file_str_list = curr_dir.entryList(file_filter, QDir::Files);
            ret.append(curr_node);
        }

        ++n_idx;
        if(n_idx >= ret.size()) break;
    }
}

void collect_files(QString root_dir, QStringList file_filter, QStringList& ret)
{
    dir_walkout_ret_t walkout_ret;

    walkout_dir(root_dir, file_filter, walkout_ret);
    for(int n_idx = 0; n_idx < walkout_ret.size(); ++n_idx)
    {
        QString prefix = walkout_ret[n_idx].curr_pth;
        for(int f_idx = 0; f_idx < walkout_ret[n_idx].file_str_list.size(); ++f_idx)
        {
            ret.append(prefix + "/" + walkout_ret[n_idx].file_str_list[f_idx]);
        }
    }
}
