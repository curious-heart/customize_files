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

static bool exec_external_process(QString cmd, QString cmd_args, bool as_admin = false)
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
