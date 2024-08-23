#ifndef COMMON_TOOL_FUNC_H
#define COMMON_TOOL_FUNC_H

#include <QTextEdit>
#include <QFont>
#include <QStringList>
#include <QMessageBox>

QString common_tool_get_curr_dt_str();
QString common_tool_get_curr_date_str();
QString common_tool_get_curr_time_str();

bool mkpth_if_not_exists(const QString &pth_str);

#define DEF_SHUTDOWN_WAIT_TIME 3
/*return the shutdown command line.*/
QString shutdown_system(QString reason_str = "", int wait_time = DEF_SHUTDOWN_WAIT_TIME);

#define ROUNDUP_UINT16_TO_10(x) ((quint16)(((quint16)(((x) + 5) / 10)) * 10))
#define ARRAY_COUNT(a) (sizeof((a)) / sizeof((a)[0]))

typedef enum
{
    INT_DATA, FLOAT_DATA,
}common_data_type_enum_t;
class RangeChecker
{
public:
#define EDGE_ITEM(a) a
#define EDGE_LIST \
        EDGE_ITEM(EDGE_INCLUDED),\
        EDGE_ITEM(EDGE_EXCLUDED),\
        EDGE_ITEM(EDGE_INFINITE),\
        EDGE_ITEM(EDGE_COUNT),
    typedef enum
    {
        EDGE_LIST
    }range_edge_enum_t;
#undef EDGE_ITEM
static  const char* range_edge_strs[];

private:
    bool valid;
    double min, max;
    range_edge_enum_t low_edge, up_edge;
    const char* unit_str;
public:
    RangeChecker(double min = -1, double max = 1, const char* unit_str = "",
                 range_edge_enum_t low_edge = EDGE_INCLUDED, range_edge_enum_t up_edge = EDGE_INCLUDED);
public:
    bool range_check(double val);

    void set_min_max(double min_v, double max_v);
    void set_edge(range_edge_enum_t low_e, range_edge_enum_t up_e);
    void set_unit_str(const char* unit_s);
    range_edge_enum_t range_low_edge();
    range_edge_enum_t range_up_edge();
    double range_min();
    double range_max();
    QString range_str(common_data_type_enum_t d_type, double factor = 1, QString new_unit_str = "");
};

void append_str_with_color_and_weight(QTextEdit* ctrl, QString str,
                             QColor new_color = Qt::black,
                             int new_font_weight = -1);

int count_discrete_steps(double low_edge, double up_edge, double step);

int standard_btn_msg_box(QString title, QString info, QMessageBox::StandardButtons btns);

typedef enum
{
    CP_FILE_WRITTEN = 0, //dst folder contains no file with the same name of src file. cp ok.
    CP_FILE_OVERWRITTEN,
    CP_FILE_SKIPPED, //file with the same name of src file is not copied.
    CP_FILE_OK_FLAG = 100, //just use it as seperator, DO NOT return it actually!

    CP_FILE_ERR_NO_SRC_FILE,
    CP_FILE_ERR_NO_DST_FOLDER,
    CP_FILE_ERR_USER_REFUSE_TO_MK_DST_FOLDER,
    CP_FILE_ERR_MK_DST_FOLDER_FAILS,
    CP_FILE_ERR_USER_ABORT,
    CP_FILE_ERR_CALLER_ABORT,
    CP_FILE_ERR_REMOVE_OLD_FILE_FAILS,
    CP_FILE_ERR_UNKNOW_ERR,
}cp_file_result_e_t;
typedef QList<cp_file_result_e_t> cp_file_result_list_t;

typedef enum
{
    CP_FILE_EXIST_OP_OVERWRITE,
    CP_FILE_EXIST_OP_SKIP,
    CP_FILE_EXIST_OP_ASK,
    CP_FILE_EXIST_OP_ABORT,
}cp_file_exist_op_e_t;
/*dst_pth DOES NOT end with "/" */
bool copy_files(QStringList src_list, QString dst_pth, cp_file_exist_op_e_t f_exist_op,
                bool mk_dst_folder_if_not_exists = true, bool stop_on_error = true,
                cp_file_result_list_t * result_list = nullptr, QStringList *result_str_list = nullptr,
                bool ask_when_mk_dst_folder = false);

extern const char* g_str_src;
extern const char* g_str_dst;
extern const char* g_str_folder;
extern const char* g_str_file;
extern const char* g_str_not_exists;
extern const char* g_str_already_exists;
extern const char* g_str_create;
extern const char* g_str_need;
extern const char* g_str_succeed;
extern const char* g_str_fail;
extern const char* g_str_ask_if_want_to_make_it;
extern const char* g_str_user_refuse_to_make;
extern const char* g_str_try_to_make;
extern const char* g_str_try_to_ow;
extern const char* g_str_try_to_rm;
extern const char* g_str_user_skip;
extern const char* g_str_operation_abort_by_caller;
extern const char* g_str_operation_abort_by_user;
extern const char* g_str_overwrite;
extern const char* g_str_remove;
extern const char* g_str_copy;
extern const char* g_str_overwrite_all;
extern const char* g_str_skip;
extern const char* g_str_skip_all;
extern const char* g_str_but;
extern const char* g_str_current;
extern const char* g_str_to;

void rm_slash_at_ends(QString &str, bool s = true, bool e = true);

#endif // COMMON_TOOL_FUNC_H
